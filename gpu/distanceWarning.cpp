%%writefile concurrent_tasks.cu
#include <stdio.h>
#include <cuda_runtime.h>


// Kernel for receiveData
__global__ void receiveData(int *data) {
  //printf("test");
    int receiveDistanceData = 0;
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    
    for(int i=0; i<100; i++){
        data[idx] = receiveDistanceData++;
        printf("Distance is: %d\n", data[idx]);
    }
}

// Kernel for monitorDistance
__global__ void monitorDistance(int *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    printf("Test");
    if (data[idx] < 50) {
        printf("Warning: Distance is less than 10\n");
    } else{
      printf("Safe\n");
    }
}

int main() {
    int *d_data, *h_data;
    size_t size = sizeof(int);

    // Allocate memory on the host
    h_data = (int*)malloc(size);

    // Initialize vectors
    h_data = 0;

    // Allocate memory on the device
    cudaMalloc((void**)&d_data, size);

    // Copy data to device
    cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);

    // Create streams for concurrent execution
    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    // Launch kernels in different streams
    int threads_per_block = 1;
    int blocks_per_grid = 1;

    receiveData<<<blocks_per_grid, threads_per_block, 0, stream1>>>(d_data);
    monitorDistance<<<blocks_per_grid, threads_per_block, 0, stream2>>>(d_data);
    

    // Synchronize streams
    cudaStreamSynchronize(stream1);
    cudaStreamSynchronize(stream2);

    // Copy results back to host
    //cudaMemcpy(h_c_add, d_c_add, size, cudaMemcpyDeviceToHost);
    

    

    // Free memory
    cudaFree(d_data);
    free(h_data);

    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);

    return 0;
}
