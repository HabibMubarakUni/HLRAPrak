// -*- C++ Header -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#ifndef HPCA_PC_MLP_UTILS_H
#define HPCA_PC_MLP_UTILS_H

#include <vector>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <random>
#include <stdexcept>
#include <cmath>
#include <algorithm> // für Shuffle()
#include "P4_F32vec4.h"

namespace Utils
{
  /**
   * Matrix-vector multiplication: r = A * x
   * @param matrix std::vector<std::vector<float>> reference to matrix A
   * @param vector std::vector<float> reference to vector b
   * @param result std::vector<float> reference to result r
   */
  void MatVecMul(const std::vector<std::vector<float>>& matrix, const std::vector<float>& vector,
                 std::vector<float>& result);

  void MatTransposeVecMul(const std::vector<std::vector<float>>& matrix, const std::vector<float>& vector,
                          std::vector<float>& result);

  /**
 * Matrix transposition
 * @param matrix std::vector<std::vector<float>> reference to matrix that is transposed.
 * @param result std::vector<std::vector<float>> reference to matrix which is the output of transpose.
 */
  void Transpose(const std::vector<std::vector<float>>& matrix, std::vector<std::vector<float>>& result);

  /**
   * Vector addition elementwise r = a + b
   * @param vectorA std::vector<float> reference to vector a
   * @param vectorB std::vector<float> reference to vector b
   * @param result std::vector<float> reference to result r
   */
  void VecAdd(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result);

  void VecSub(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result);

  void VecSca(std::vector<float>& vector, float scalar, std::vector<float>& result);

  /**
 * @brief Affine transformation for r = Ax + b
 * @param matrixA std::vector<std::vector<float>> reference to matrix A
 * @param vectorX std::vector<float> reference to vector x
 * @param vectorB std::vector<float> reference to vector b
 * @param result std::vector<float> reference to result r
 */
  void AffineTransform(const std::vector<std::vector<float>>& matrixA,
                       std::vector<float>& vectorX,
                       std::vector<float>& vectorB,
                       std::vector<float>& result);

  /**
   * Outer Product of two vectors: result = a * b^T
   * @param a std::vector<float> reference to vector a
   * @param b std::vector<float> reference to vector b
   * @param result std::vector<std::vector<float>> reference to result matrix
   */
  void OuterProduct(const std::vector<float>& a, const std::vector<float>& b, std::vector<std::vector<float>> result);

  /**
   * Outer Product of two vectors: result += a * b^T
   * @param a std::vector<float> reference to vector a
   * @param b std::vector<float> reference to vector b
   * @param result std::vector<std::vector<float>> reference to result matrix
   */
  void OuterProductAdd(const std::vector<float>& a,
                       const std::vector<float>& b,
                       std::vector<std::vector<float>>& result);

  /**
   * Hadamard Product (elementwise multiplication) of two vectors: result = a * b
   * @param vectorA std::vector<float> reference to vector a
   * @param vectorB std::vector<float> reference to vector b
   * @param result std::vector<float> reference to result vector
   */
  void HadamardProduct(const std::vector<float>& vectorA,
                       const std::vector<float>& vectorB,
                       std::vector<float>& result);

  /**
   * Filling a vector with random values in range [lowerBound, upperBound]
   * @param vector std::vector<float> reference to vector that is filled
   * @param lowerBound size_t lower bound of random values
   * @param upperBound size_t upper bound of random values
   */
  void FillRandomly(std::vector<float>& vector, float lowerBound, float upperBound);

  /**
   * Filling a matrix with random values in range [lowerBound, upperBound]
   * @param matrix std::vector<std::vector<float>> reference to matrix that is filled
   * @param lowerBound size_t lower bound of random values
   * @param upperBound size_t upper bound of random values
   */
  void FillRandomly(std::vector<std::vector<float>>& matrix, float lowerBound, float upperBound);

  /**
   * Filling a vector with random values based on PyTorch weight initialization
   * @param vector std::vector<float> reference to vector that is filled
   * @param nInputFeatures size_t number of input features used for random values
   */
  void FillRandomlyPyTorch(std::vector<float>& vector, size_t nInputFeatures);

  /**
   * Filling a matrix with random values based on PyTorch weight initialization
   * @param matrix std::vector<std::vector<float>> reference to matrix that is filled
   * @param nInputFeatures size_t number of input features used for random values
   */
  void FillRandomlyPyTorch(std::vector<std::vector<float>>& matrix, size_t nInputFeatures);

  /**
   * Shuffle function to permutate the order of input features and labels
   * @param inputFeatures std::vector<std::vector<float>> reference to input features
   * @param labels std::vector<size_t> reference to labels
   */
  void Shuffle(std::vector<std::vector<float>>& inputFeatures, std::vector<size_t>& labels);

  /**
   * Function to make the given vector a 0-vector
   * @param vector std::vector<float> reference to vector that is filled with 0
   */
  void Zeros(std::vector<float>& vector);

  /**
   * Function to make the given matrix a 0-matrix
   * @param matrix std::vector<std::vector<float>> reference to matrix that is filled with 0
   */
  void Zeros(std::vector<std::vector<float>>& matrix);

  /**
   * Function to print a matrix
   * @param matrix std::vector<std::vector<float>> reference to matrix that is printed
   */
  void Print(std::vector<std::vector<float>>& matrix);

  /**
   * Function to print a vector
   * @param vector std::vector<float> reference to vector that is printed
   */
  void Print(std::vector<float>& vector);

}

#endif //HPCA_PC_MLP_UTILS_H
