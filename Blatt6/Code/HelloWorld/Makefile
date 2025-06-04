# Set your C++ compiler here
CXX = g++
SRC = HelloWorld.cpp
OUT = HelloWorld.out

CXXFLAGS = -O3 -fopenmp

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)