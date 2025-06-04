/*
This program will numerically compute the integral of

                  4/(1+x*x) 

from 0 to 1. The value of this integral is pi -- which is great since it provides an easy way to check the solution.

To compile and run your code, please use:
<Compiler> Pi.cpp -o Pi.out -O3 -fopenmp && ./Pi.out
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

#include <omp.h>

// ANSI color codes
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define RED "\033[31m"


// Constants
constexpr size_t NUM_STEPS = 100000000;
constexpr size_t NUM_THREADS = 4;

constexpr double STEP = 1.0 / static_cast<double>(NUM_STEPS);

// Function declarations
double calculatePiSequential();
double calculatePiThreadSums();
double calculatePiReduction();

bool comparePiValues(double pi1, double pi2, double tolerance = 1e-8);

int main() 
{
    omp_set_num_threads(NUM_THREADS);

    std::cout << "Number of steps: \t" << NUM_STEPS << "\n";
    std::cout << "Number of threads: \t" << NUM_THREADS << "\n\n";

    // Sequential calculation (reference for speedup)
    double start_time = omp_get_wtime();
    double piSequential = calculatePiSequential();
    double runtimeSequential = omp_get_wtime() - start_time;

    // Thread-local sums calculation
    start_time = omp_get_wtime();
    double piThreadSums = calculatePiThreadSums();
    double runtimeThreadSums = omp_get_wtime() - start_time;

    // OpenMP reduction calculation
    start_time = omp_get_wtime();
    double piReduction = calculatePiReduction();
    double runtimeReduction = omp_get_wtime() - start_time;

    // Results
    std::cout << std::fixed << std::setprecision(5);
    std::cout << "--- Results ---\n";
    std::cout << "Sequential:\n"
              << "  Value:       " << piSequential << "\n"
              << "  Runtime:     " << runtimeSequential << " seconds\n"
              << std::endl;

    std::cout << "Thread-local sums method:\n"
              << "  Value:       " << piThreadSums << "\n"
              << "  Runtime:     " << runtimeThreadSums << " seconds\n"
              << "  Speedup:     " << runtimeSequential / runtimeThreadSums << "\n"
              << std::endl;

    std::cout << "OpenMP reduction method:\n"
              << "  Value:       " << piReduction << "\n"
              << "  Runtime:     " << runtimeReduction << " seconds\n"
              << "  Speedup:     " << runtimeSequential / runtimeReduction << "\n"
              << std::endl;

    if (comparePiValues(piSequential, piThreadSums)) {
      std::cout << GREEN << "[PASSED] " << RESET << "Thread-local sums method matches sequential result." << std::endl;
    } 
    else {
      std::cerr << RED << "[FAILED] " << RESET << "Thread-local sums method does NOT match sequential result." << std::endl;
    }

    if (comparePiValues(piSequential, piReduction)) {
        std::cout << GREEN << "[PASSED] " << RESET << "OpenMP reduction method matches sequential result." << std::endl;
    } else {
        std::cerr << RED << "[FAILED] " << RESET << "OpenMP reduction method does NOT match sequential result." << std::endl;
    }

    return 0;
}

/**
Method 1: Sequential calculation (for speedup reference)
 */
double calculatePiSequential() 
{
    double sum = 0.0;

    for (size_t i = 1; i <= NUM_STEPS; ++i) {
        double x = (i - 0.5) * STEP;
        sum += 4.0 / (1.0 + x * x);
    }

    return STEP * sum;
}

/**
Method 2: Calculate Pi using thread-local sums
 */
double calculatePiThreadSums() 
{
    // TODO: Implement thread-local sums method to calculate Pi
    return 0.;
}

/**
Method 3: Calculate Pi using OpenMP reduction
 */
double calculatePiReduction() 
{
    // TODO: Implement OpenMP reduction method to calculate Pi
    return 0.;
}

/**
Function to compare Pi values within a given tolerance
 */
bool comparePiValues(double pi1, double pi2, double tolerance) 
{
    return std::fabs(pi1 - pi2) < tolerance;
}
