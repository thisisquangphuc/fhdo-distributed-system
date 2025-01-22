%%writefile concurrent.cu
#include <cuda_runtime.h>
#include <iostream>

#define N 1

__global__ void receiveData(float *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    data[idx]++;
    printf("Distance: %f m\n", data[idx]);

}

__global__ void monitorDistance(float *data) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (data[idx] < 10) {
        printf("Warning: Distance is less than 10 m\n");
    } else if ((data[idx] >= 10) & (data[idx] <= 12)){
      printf("Safe\n");
    } else{
      printf("Warning: Distance is too far m\n");
    }
}

int main() {
    float *d_data1; //*d_data2;
    float *h_data = new float[N];

    for (int i = 0; i < N; ++i) {
        h_data[i] = 0;
    }

    cudaMalloc(&d_data1, N * sizeof(float));
    //cudaMalloc(&d_data2, N * sizeof(float));

    cudaStream_t stream1, stream2;
    cudaStreamCreate(&stream1);
    cudaStreamCreate(&stream2);

    cudaMemcpyAsync(d_data1, h_data, N * sizeof(float), cudaMemcpyHostToDevice, stream1);
    //cudaMemcpyAsync(d_data2, h_data, N * sizeof(float), cudaMemcpyHostToDevice, stream2);

    int threadsPerBlock = 1;
    int blocksPerGrid = 1;
    for(int i=0; i<20; i++){
      receiveData<<<blocksPerGrid, threadsPerBlock, 0, stream1>>>(d_data1);
      monitorDistance<<<blocksPerGrid, threadsPerBlock, 0, stream2>>>(d_data1);
    }


    cudaMemcpyAsync(h_data, d_data1, N * sizeof(float), cudaMemcpyDeviceToHost, stream1);
    //cudaMemcpyAsync(h_data, d_data2, N * sizeof(float), cudaMemcpyDeviceToHost, stream2);

    cudaStreamSynchronize(stream1);
    cudaStreamSynchronize(stream2);



    cudaFree(d_data1);
    //cudaFree(d_data2);
    cudaStreamDestroy(stream1);
    cudaStreamDestroy(stream2);
    delete[] h_data;

    return 0;
}
