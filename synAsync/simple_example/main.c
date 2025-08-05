#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>

#define N 1000000

// A simple kernel to add two arrays
void add_arrays(float *a, float *b, float *c, int n) {
    #pragma acc parallel loop present(a[0:n], b[0:n], c[0:n]) async(1)
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    float *a, *b, *c;
    
    // Allocate host memory
    a = (float*)malloc(N * sizeof(float));
    b = (float*)malloc(N * sizeof(float));
    c = (float*)malloc(N * sizeof(float));
    
    // Initialize arrays on the host
    for (int i = 0; i < N; i++) {
        a[i] = i * 1.0f;
        b[i] = (N - i) * 2.0f;
    }
    
    // Allocate device memory and copy input data
    #pragma acc enter data copyin(a[0:N], b[0:N]) create(c[0:N])
    
    // Launch the kernel asynchronously
    add_arrays(a, b, c, N);
    
    // Copy the result back from device to host
    #pragma acc update host(c[0:N]) async(1)
    
    // Synchronize to ensure completion
    #pragma acc wait(1)
    
    // Verify results
    for (int i = 0; i < N; i++) {
        if (c[i] != (a[i] + b[i])) {
            printf("Mismatch at index %d: expected %f but got %f\n", i, a[i] + b[i], c[i]);
            break;
        }
    }
    
    printf("Array addition completed successfully!\n");
    
    // Clean up device memory
    #pragma acc exit data delete(a[0:N], b[0:N], c[0:N])
    
    // Clean up host memory
    free(a);
    free(b);
    free(c);
    
    return 0;
}
