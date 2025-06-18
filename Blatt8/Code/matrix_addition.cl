void kernel matrix_add(global const float* A, global const float* B, global float* C, const int N) {
    int row = get_global_id(0);
    int col = get_global_id(1);

    C[row * N + col] = A[row * N + col] + B[row * N + col];
}
