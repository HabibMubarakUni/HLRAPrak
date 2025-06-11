# TBB library paths
TBB_HOME = $(HOME)/TBB_Lib/tbb
TBB_INCLUDE = $(TBB_HOME)/include
TBB_LIB = $(TBB_HOME)/lib64

# Vc library paths
VC_INCLUDE = $(HOME)/Vc/include
VC_LIB = $(HOME)/Vc/lib

# Compiler and flags
CXX = clang++
CXXFLAGS = -O3 -fno-tree-vectorize -msse -I$(TBB_INCLUDE) -I$(VC_INCLUDE)

# Source and target
SRC = matrix.cpp
TARGET = matrix

# Linker flags
LDFLAGS = -L$(TBB_LIB) -L$(VC_LIB) -ltbb -ltbbmalloc
LIBS = $(VC_LIB)/libVc.a

# Build target
$(TARGET): $(SRC)
	$(CXX) $(SRC) -o $(TARGET) $(CXXFLAGS) $(LDFLAGS) $(LIBS)

# Run target
run: $(TARGET)
	LD_LIBRARY_PATH=$(TBB_LIB):$$LD_LIBRARY_PATH ./$(TARGET)

# Clean target
clean:
	rm -f $(TARGET)

