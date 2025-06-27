#include "includes/World.cuh"
#include <cstdio>
#include <cuda_runtime.h>

inline cudaError_t checkCuda(cudaError_t result) {
    if (result != cudaSuccess) {
        fprintf(stderr, "CUDA Runtime Error: %s\n", cudaGetErrorString(result));
    }
    return result;
}


__device__ int CountAliveNeighbours(int* grid, int i, int height, int width) {
    int aliveNeighbours = 0;
    int y = i / width;
    int x = i % width;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
  
        // Berechnung der Position der benachbarten Zelle unter Berücksichtigung von Randbedingungen
        int sy = (y + dy + height) % height;
        int sx = (x + dx + width) % width;
  
        // Die aktuelle Zelle selbst überspringen indem man die Indices überprüft
        if (dx != 0 || dy != 0) {
          aliveNeighbours += grid[sy * width + sx];
        }
      }
    }
  
    return aliveNeighbours;
}

__global__ void EvolveKernel(int* grid, int* grid_for_evolve, int height, int width) {
    int indexWithinTheGrid = threadIdx.x + blockIdx.x * blockDim.x;
    int gridStride = gridDim.x * blockDim.x;

    for (int i = indexWithinTheGrid; i < height*width; i += gridStride) {
        int aliveNeighbours = CountAliveNeighbours(grid, i, height, width);

        // Regel 1: Lebende Zelle mit weniger als 2 lebenden Nachbarn stirbt
        if (grid[i] == 1 && aliveNeighbours < 2) {
            grid_for_evolve[i] = 0;
        } 
        // Regel 2: Lebende Zelle mit 2 oder 3 lebenden Nachbarn bleibt am Leben
        else if (grid[i] == 1 && (aliveNeighbours == 2 || aliveNeighbours == 3)) {
            grid_for_evolve[i] = 1;
        }
        // Regel 3: Lebende Zelle mit mehr als 3 lebenden Nachbarn stirbt
        else if (grid[i] == 1 && aliveNeighbours > 3) {
            grid_for_evolve[i] = 0;
        }
        // Regel 4: Tote Zelle mit genau 3 lebenden Nachbarn wird lebendig
        else if (grid[i] == 0 && aliveNeighbours == 3) {
            grid_for_evolve[i] = 1;
        }
        else {
            grid_for_evolve[i] = grid[i];
        }
    }
}


// Neu hinzugefügt für Blatt 9
void EvolveWrapper(int generations_amount, int height, int width, std::vector<std::vector<int>>& grid) {
    // Erstelle einen Stream
    cudaStream_t stream;
    cudaStreamCreate(&stream);

    checkCuda(cudaGetLastError());
    
    int *h_grid;
    // Allokiere Speicher auf Host
    cudaMallocHost(&h_grid, sizeof(int) * height * width);

    int *d_grid, *d_grid_for_evolve;
    // Allokiere Speicher auf Device/VRAM (für 2 Gitter/Arrays, denn evolve arbeitet mit 2 Gitter)
    cudaMalloc(&d_grid, sizeof(int) * height * width);
    cudaMalloc(&d_grid_for_evolve, sizeof(int) * height * width);
    
    // Daten von 2D-vector in 1D-array
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            h_grid[y * width + x] = grid[y][x];
        }
    }

    // Kopiere Daten von Host nach VRAM
    cudaMemcpy(d_grid, h_grid, sizeof(int) * height * width, cudaMemcpyHostToDevice);

    checkCuda(cudaGetLastError());

    // 64 * 256 = 16384 > 10000 = Anzahl der Zellen im p67_snark_loop Gitter => Jede Zelle kriegt einen Thread, es gibt genug
    constexpr int n_blocks = 64;
    constexpr int n_threads = 256;

    for (int gen = 0; gen < generations_amount; gen++) {
        EvolveKernel<<<n_blocks, n_threads, 0, stream>>>(d_grid, d_grid_for_evolve, height, width);
        checkCuda(cudaGetLastError());
        cudaDeviceSynchronize();
        checkCuda(cudaGetLastError());
        std::swap(d_grid, d_grid_for_evolve); // vertausche Pointers
    }

    // nicht d_grid_for_evolve, denn es wurde zuletzt std::swap oben angewendet
    cudaMemcpy(h_grid, d_grid, sizeof(int) * height * width, cudaMemcpyDeviceToHost);

    checkCuda(cudaGetLastError());

    // Daten von 1D-array in 2D-vector
    for (int i = 0; i < height * width; i++) {
        grid[i / width][i % width] = h_grid[i];
    }

    cudaFree(d_grid);
    cudaFree(d_grid_for_evolve);
    cudaFreeHost(h_grid);
    cudaStreamDestroy(stream);

    checkCuda(cudaGetLastError());
}