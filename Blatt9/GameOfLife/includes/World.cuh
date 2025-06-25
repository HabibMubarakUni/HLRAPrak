//! #pragma once

__device__ int CountAliveNeighbours(int* grid, int i, int height, int width);

__global__ void UpdateCellsCUDA(int* grid, int* newGrid, int height, int width);