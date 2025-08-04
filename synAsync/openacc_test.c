#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <openacc.h>
#include <time.h>
#include <nvToolsExt.h>

#define N 1000000
#define ITERATIONS 100

void initialize_arrays(float *a, float *b, float *c, int size) {
    for (int i = 0; i < size; i++) {
        a[i] = i * 0.001f;
        b[i] = i * 0.002f;
        c[i] = 0.0f;
    }
}

int verify_results(float *ref, float *test, int size) {
    int errors = 0;
    const float tol = 1e-4;
    for (int i = 0; i < size; i++) {
        if (fabsf(ref[i] - test[i]) > tol) {
            if (errors < 5) {
                printf("Mismatch at %d: ref=%.6f, test=%.6f\n", i, ref[i], test[i]);
            }
            errors++;
        }
    }
    return errors == 0;
}

void synchronous_version(float *a, float *b, float *c, int size) {

    nvtxRangePush("Synchronous Version");

    printf("\n--- Running synchronous version ---\n");
    clock_t start = clock();

    #pragma acc data copyin(a[0:size], b[0:size]) copy(c[0:size])
    {
        for (int iter = 0; iter < ITERATIONS; iter++) {
            #pragma acc parallel loop
            for (int i = 0; i < size; i++) {
                c[i] = a[i] + b[i];
            }

            #pragma acc parallel loop
            for (int i = 0; i < size; i++) {
                c[i] = c[i] * 2.0f + a[i] * 0.5f;
            }

            #pragma acc parallel loop
            for (int i = 0; i < size; i++) {
                float denom = c[i] + 1.0f;
                if (denom != 0.0f) {
                    c[i] = c[i] + (a[i] * b[i]) / denom;
                } else {
                    c[i] = c[i] + (a[i] * b[i]);
                }
            }
        }
    }

    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Synchronous version time: %.4f s\n", time);
    nvtxRangePop();
}

void asynchronous_version(float *a, float *b, float *c, int size) {

    nvtxRangePush("Asynchronous Version");	

    printf("\n--- Running asynchronous version ---\n");

    clock_t start = clock();
    int q1 = 1, q2 = 2;

    // Copy to device
    #pragma acc enter data copyin(a[0:size], b[0:size]) create(c[0:size]) async(q1)

    // Dummy flush kernel to ensure context is initialized
    #pragma acc parallel loop present(a[0:1]) async(q1)
    for (int i = 0; i < 1; i++) a[i] += 0;

    #pragma acc wait(q1)

    // Main loop
    for (int iter = 0; iter < ITERATIONS; iter++) {
        #pragma acc parallel loop async(q1) present(a[0:size], b[0:size], c[0:size])
        for (int i = 0; i < size; i++) {
            c[i] = a[i] + b[i];
        }

        #pragma acc parallel loop async(q2) wait(q1) present(a[0:size], c[0:size])
        for (int i = 0; i < size; i++) {
            c[i] = c[i] * 2.0f + a[i] * 0.5f;
        }

        #pragma acc parallel loop async(q1) wait(q2) present(a[0:size], b[0:size], c[0:size])
        for (int i = 0; i < size; i++) {
            float denom = c[i] + 1.0f;
            if (denom != 0.0f) {
                c[i] = c[i] + (a[i] * b[i]) / denom;
            } else {
                c[i] = c[i] + (a[i] * b[i]);
            }
        }
    }

    // Copy result back and clean up
    #pragma acc exit data copyout(c[0:size]) async(q1)
    #pragma acc wait(q1)

    #pragma acc exit data delete(a[0:size], b[0:size], c[0:size])

    clock_t end = clock();
    double time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Asynchronous version time: %.4f s\n", time);
    nvtxRangePop();
}

int main() {
    printf("OpenACC Async vs Sync Comparison (N = %d, ITERATIONS = %d)\n", N, ITERATIONS);

    float *a = (float*)malloc(N * sizeof(float));
    float *b = (float*)malloc(N * sizeof(float));
    float *c_sync = (float*)malloc(N * sizeof(float));
    float *c_async = (float*)malloc(N * sizeof(float));

    if (!a || !b || !c_sync || !c_async) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    initialize_arrays(a, b, c_sync, N);
    synchronous_version(a, b, c_sync, N);

    initialize_arrays(a, b, c_async, N);
    asynchronous_version(a, b, c_async, N);

    printf("\n--- Verifying results ---\n");
    if (verify_results(c_sync, c_async, N)) {
        printf("✓ Sync and Async results match.\n");
    } else {
        printf("✗ Results differ between Sync and Async versions.\n");
    }

    printf("\nSample output (first 5 elements):\n");
    for (int i = 0; i < 5; i++) {
        printf("Index %d: Sync = %.6f, Async = %.6f\n", i, c_sync[i], c_async[i]);
    }

    free(a); free(b); free(c_sync); free(c_async);

    printf("\nTo profile with Nsight Systems:\n");
    printf("nsys profile -o acc_profile --trace=openacc,cuda ./a.out\n");

    return 0;
}

