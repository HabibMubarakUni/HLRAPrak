#include "includes/World.cuh"

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

__global__ void UpdateCellsCUDA(int* grid, int* newGrid, int height, int width) {
    int indexWithinTheGrid = threadIdx.x + blockIdx.x * blockDim.x;
    int gridStride = gridDim.x * blockDim.x;

    for (int i = indexWithinTheGrid; i < height*width; i += gridStride) {
        int aliveNeighbours = CountAliveNeighbours(grid, i, height, width);

        // Regel 1: Lebende Zelle mit weniger als 2 lebenden Nachbarn stirbt
        if (grid[i] == 1 && aliveNeighbours < 2) {
            newGrid[i] = 0;
        } 
        // Regel 2: Lebende Zelle mit 2 oder 3 lebenden Nachbarn bleibt am Leben
        else if (grid[i] == 1 && (aliveNeighbours == 2 || aliveNeighbours == 3)) {
            newGrid[i] = 1;
        }
        // Regel 3: Lebende Zelle mit mehr als 3 lebenden Nachbarn stirbt
        else if (grid[i] == 1 && aliveNeighbours > 3) {
            newGrid[i] = 0;
        }
        // Regel 4: Tote Zelle mit genau 3 lebenden Nachbarn wird lebendig
        else if (grid[i] == 0 && aliveNeighbours == 3) {
            newGrid[i] = 1;
        }
        else {
            newGrid[i] = grid[i];
        }
    }
}