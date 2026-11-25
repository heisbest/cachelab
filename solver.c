/*
 * solver.c - Starter for Cache Lab Bonus.
 * solve() is called once per mystery cache. See solver.h and README.md.
 */
#include "solver.h"

void solve(void)
{
    int b = 0, s, E, policy, write_allocate;
    int lo, hi, mid, count, i, misses;
    uint32_t block_size, blocks, stride;

    /* The first power-of-two offset outside block zero is the block size. */
    cache_flush();
    cache_load(0);
    while (b < 31 && cache_load(1u << b))
        b++;
    block_size = 1u << b;

    /*
     * Find the total number of lines.  A working set of n distinct blocks
     * has no misses on a second pass exactly when n fits in the cache.  This
     * does not depend on the replacement policy.
     */
    lo = 1;
    hi = 8193;                 /* maximum is 2^9 sets times 16 ways, plus 1 */
    while (lo + 1 < hi) {
        mid = lo + (hi - lo) / 2;
        cache_flush();
        for (i = 0; i < mid; i++)
            cache_load((uint32_t)i * block_size);
        for (i = 0; i < mid && cache_load((uint32_t)i * block_size); i++)
            ;
        if (i == mid)
            lo = mid;
        else
            hi = mid;
    }
    blocks = (uint32_t)lo;

    /*
     * For a candidate set count 2^s, repeatedly overflow the set selected
     * by one stride.  It is set zero precisely when the candidate is at
     * least the real set count; therefore the first such stride gives s.
     * Repeating 256 times also makes a RANDOM replacement evict block zero
     * with overwhelming probability (the largest associativity is 16).
     */
    for (s = 1; s <= 9; s++) {
        stride = 1u << s;
        cache_flush();
        for (i = 0; i < (int)blocks; i++)
            cache_load((uint32_t)i * block_size);
        for (i = 1; i <= 256; i++)
            cache_load(((uint32_t)i * blocks + stride) * block_size);
        if (!cache_load(0))
            break;
    }
    E = (int)(blocks >> s);

    /* A store miss followed by a load distinguishes write allocate. */
    cache_flush();
    cache_store(0);
    write_allocate = cache_load(0);

    if (E == 1) {
        policy = POLICY_LRU;
    } else {
        /*
         * Refreshing block zero before one overflowing load preserves it for
         * LRU and evicts it for FIFO.  RANDOM gives a mixture of outcomes
         * over independent flushed trials.
         */
        misses = 0;
        stride = 1u << s;
        for (count = 0; count < 128; count++) {
            cache_flush();
            for (i = 0; i < E; i++)
                cache_load((uint32_t)i * stride * block_size);
            cache_load(0);
            cache_load((uint32_t)E * stride * block_size);
            if (!cache_load(0))
                misses++;
        }
        if (misses == 0)
            policy = POLICY_LRU;
        else if (misses == 128)
            policy = POLICY_FIFO;
        else
            policy = POLICY_RANDOM;
    }

    report(b, s, E, policy, write_allocate);
}
