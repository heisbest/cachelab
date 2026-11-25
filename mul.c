/*
 * mul.c - Matrix multiply C = A * B
 *
 * Each multiply function must have a prototype of the form:
 * void mul(int M, int N, int A[N][M], int B[M][N], int C[N][N]);
 *
 * A multiply function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_mul(int M, int N, int A[N][M], int B[M][N], int C[N][N]);

/*
 * mul_submit - This is the solution multiply function that you will be
 *     graded on for Part C of the assignment. Do not change the
 *     description string "Multiply submission", as the driver searches
 *     for that string to identify the multiply function to be graded.
 *     C starts out holding garbage, so every element of C must be written.
 */
char mul_submit_desc[] = "Multiply submission";
void mul_submit(int M, int N, int A[N][M], int B[M][N], int C[N][N])
{
    int i, j, k, ii, jj, kk;
    int b0, b1, b2, b3, b4, b5, b6, b7;
    int c0, c1, c2, c3, c4, c5, c6, c7;

    for (ii = 0; ii < N; ii += 8) {
        for (jj = 0; jj < N; jj += 8) {
            if (ii + 8 <= N && jj + 8 <= N) {
                for (kk = 0; kk < M; kk += 8) {
                    for (i = ii; i < ii + 8; i++) {
                        if (kk == 0) {
                            c0 = c1 = c2 = c3 = c4 = c5 = c6 = c7 = 0;
                        } else {
                            c0 = C[i][jj];     c1 = C[i][jj + 1];
                            c2 = C[i][jj + 2]; c3 = C[i][jj + 3];
                            c4 = C[i][jj + 4]; c5 = C[i][jj + 5];
                            c6 = C[i][jj + 6]; c7 = C[i][jj + 7];
                        }
                        for (k = kk; k < M && k < kk + 8; k++) {
                            b0 = B[k][jj];     b1 = B[k][jj + 1];
                            b2 = B[k][jj + 2]; b3 = B[k][jj + 3];
                            b4 = B[k][jj + 4]; b5 = B[k][jj + 5];
                            b6 = B[k][jj + 6]; b7 = B[k][jj + 7];
                            c0 += A[i][k] * b0; c1 += A[i][k] * b1;
                            c2 += A[i][k] * b2; c3 += A[i][k] * b3;
                            c4 += A[i][k] * b4; c5 += A[i][k] * b5;
                            c6 += A[i][k] * b6; c7 += A[i][k] * b7;
                        }
                        C[i][jj] = c0;     C[i][jj + 1] = c1;
                        C[i][jj + 2] = c2; C[i][jj + 3] = c3;
                        C[i][jj + 4] = c4; C[i][jj + 5] = c5;
                        C[i][jj + 6] = c6; C[i][jj + 7] = c7;
                    }
                }
            } else {
                for (i = ii; i < N && i < ii + 8; i++)
                    for (j = jj; j < N && j < jj + 8; j++) {
                        c0 = 0;
                        for (k = 0; k < M; k++)
                            c0 += A[i][k] * B[k][j];
                        C[i][j] = c0;
                    }
            }
        }
    }
}

/*
 * mul - A simple baseline multiply function, not optimized for the cache.
 */
char mul_desc[] = "Simple i-j-k multiply";
void mul(int M, int N, int A[N][M], int B[M][N], int C[N][N])
{
    int i, j, k, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            tmp = 0;
            for (k = 0; k < M; k++)
                tmp += A[i][k] * B[k][j];
            C[i][j] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your multiply functions
 *     with the driver. At runtime, the driver will evaluate each of the
 *     registered functions and summarize their performance. This is a
 *     handy way to experiment with different multiply strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerMulFunction(mul_submit, mul_submit_desc);

    /* Register any additional multiply functions */
    registerMulFunction(mul, mul_desc);
}

/*
 * is_mul - This helper function checks if C is the product of A and B.
 *     You can check the correctness of your multiply by calling it
 *     before returning from the multiply function.
 */
int is_mul(int M, int N, int A[N][M], int B[M][N], int C[N][N])
{
    int i, j, k, num;

    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            num = 0;
            for (k = 0; k < M; k++)
                num += A[i][k] * B[k][j];
            if (num != C[i][j])
                return 0;
        }
    }
    return 1;
}
