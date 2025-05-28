# Makefile for SIMD demo (AVX/SSE)

# Set your C++ compiler here
CXX     = g++
SRC     = newton.cpp
OUT     = newton.out

CXXFLAGS = -O3 -fno-tree-vectorize

SSEFLAGS = -msse
AVXFLAGS = -mavx

.PHONY: all run run-avx run-sse avx sse clean

all: $(OUT)

run: $(OUT)
	@echo "Usage: make run avx  # or make run sse"

run-avx: avx

run-sse: sse

avx: $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(AVXFLAGS) -o $(OUT) && ./$(OUT)

sse: $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(SSEFLAGS) -o $(OUT) && ./$(OUT)

clean:
	rm -f $(OUT)
