// -*- C++ -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#include "Utils.h"


namespace Utils
{
  void MatVecMul(const std::vector<std::vector<float>>& matrix,
                 const std::vector<float>& vector,
                 std::vector<float>& result)
  {
    // nxm (nxm-Matrix) * mx1 (m-Vektor) = nx1 (n-Vektor) 
    size_t n = matrix.size();
    size_t m = vector.size();

    if ((result.size() != n) ||
      (!matrix.empty() && (matrix[0].size() != m)) ||
      matrix.empty() && (m != 0)) {
        throw std::invalid_argument("Größen stimmen nicht überein");
      }

    float sum;

    // "Zeile mal Spalte"
    for (size_t i = 0; i < n; i++) {
      sum = 0.0f;
      for (size_t j = 0; j < m; j++) {
        sum += matrix[i][j] * vector[j];
      }
      result[i] = sum;
    }
  }

  void MatVecMulSimd(const std::vector<std::vector<float>>& matrix,
                          const std::vector<float>& vector,
                          std::vector<float>& result)
  {
      size_t n = matrix.size();   // Anzahl der Zeilen
      size_t m = vector.size();   // Anzahl der Spalten

      // Überprüfen, ob die Größen der Eingaben zusammenpassen
      if ((result.size() != n) || 
          (!matrix.empty() && (matrix[0].size() != m)) ||
          (matrix.empty() && (m != 0))) {
          throw std::invalid_argument("Größen stimmen nicht überein");
      }

      const size_t fvecLen = 4;

      // Für jede Zeile der Matrix
      for (size_t i = 0; i < n; ++i) {
          float sum = 0.0f;                    // Summe der aktuellen Zeile[i] * Spalte[i]

          fvec sum_simd(0.0f); 
          size_t j = 0;
          // wird außerhalb defineriert um es später für die restlichen Werte zu benutzen
          for (; j + fvecLen <= m; j += fvecLen) {
              // Lade 4 Werte aus der Matrixzeile und dem Vektor als SIMD-Vektoren
              // die Elemente werden nur als ein SIMD Vektor gelesen und interpretiert, 
              // aber die originalen Daten wenn nicht modifiziert
              const fvec& row_simd = *reinterpret_cast<const fvec*>(&matrix[i][j]);
              const fvec& vec_simd = *reinterpret_cast<const fvec*>(&vector[j]);
              // Multipliziere und aktualisiere die Summe
              sum_simd = sum_simd + row_simd * vec_simd;
          }
          // Die 4 SIMD-Ergebnisse aufsummieren
          sum += sum_simd[0] + sum_simd[1] + sum_simd[2] + sum_simd[3];

          // Restliche Elemente (falls m nicht durch 4 teilbar) manuell berechnen
          // j ist an der Stelle wo es beim letzten SIMD Index aufgehört hat zu rechnen
          for (; j < m; ++j) {
              sum += matrix[i][j] * vector[j];
          }
          result[i] = sum; // Ergebnis für diese Zeile speichern
      }
  }


  void MatTransposeVecMul(const std::vector<std::vector<float>>& matrix, const std::vector<float>& vector,
                          std::vector<float>& result)
  {
    // Da matrix eine const-Variable ist, ist es nicht unser Ziel, die Struktur von matrix zu ändern.
    // Also können wir die eigentliche Transponierung von matrix vermeiden.
    // Wir brauchen nur matrix "transponiert" durchzulaufen beim Rechnen.

    // nxm-Matrix, n-Vektor (Eingabe), m-Vektor (Ausgabe)

    size_t n = matrix.size();
    size_t m = result.size();

    if ((vector.size() != n) ||
      (!matrix.empty() && (matrix[0].size() != m)) ||
      matrix.empty() && (m != 0)) {
        throw std::invalid_argument("Größen stimmen nicht überein");
      }

    float sum;

    // "Spalte mal Spalte"
    for (size_t j = 0; j < m; j++) {
      sum = 0.0f;
      for (size_t i = 0; i < n; i++) {
        sum += matrix[i][j] * vector[i];
      }
      result[j] = sum;
    }
  }


  void Transpose(const std::vector<std::vector<float>>& matrix, std::vector<std::vector<float>>& result)
  {
    // matrix hat die Struktur: nxm
    size_t n = matrix.size();
    size_t m;

    if (!matrix.empty()) {
      m = matrix[0].size();
    }
    else {
      m = 0;
    }

    // Indizes für for-Schleifen
    size_t i;
    size_t j;

    // Wir wissen result ist leer, aber sie soll diese Struktur haben: mxn
    result.resize(m);

    //! Hinzugefügt nach der Abgabe als Korrigierung, löst evtl. immer noch nicht die wrong results
    //! Beispiel: Sei result == {{1,2}, {3,4}}
    //! Dann wäre result.resize(3) == {{1,2}, {3,4}, {}} <- also sind die inneren Vektoren nicht gecleart!
    //! Somit muss man clear() anwenden, wenn man reserve + push_back verwenden möchte.
    //! (Effizienter wäre dennoch resize + Indizes und nicht clear + reserve + push_back)
    for (j = 0; j < m; j++) {
      result[j].clear();
      result[j].reserve(n);
    }

    // Füge die Daten transponiert in result hinzu
    for (i = 0; i < n; i++) {
      for (j = 0; j < m; j++) {
        result[j].push_back(matrix[i][j]);
      }
    }
  }


  void VecAdd(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result)
  {
    size_t size = vectorA.size();

    if ((vectorB.size() != size) || (result.size() != size)) {
      throw std::invalid_argument("Dimensionen stimmen nicht überein");
    }

    for (size_t i = 0; i < size; i++) {
      result[i] = vectorA[i] + vectorB[i]; 
    }
  }


  void VecSub(std::vector<float>& vectorA, std::vector<float>& vectorB, std::vector<float>& result)
  {
    size_t size = vectorA.size();

    for (size_t idx = 0; idx < size; idx++) {
      result[idx] = vectorA[idx] - vectorB[idx];
    }
  }


  void VecSca(std::vector<float>& vector, float scalar, std::vector<float>& result)
  {
    size_t size = vector.size();

    for (size_t idx = 0; idx < size; idx++) {
      result[idx] = vector[idx] * scalar;
    }
  }


  void AffineTransform(const std::vector<std::vector<float>>& matrixA,
                       std::vector<float>& vectorX,
                       std::vector<float>& vectorB,
                       std::vector<float>& result)
  {
    // MatVecMul(matrixA, vectorX, result); // Scalar version
    MatVecMulSimd(matrixA, vectorX, result); // SIMD SSE version
    VecAdd(result, vectorB, result);
  }


  void OuterProduct(const std::vector<float>& a, const std::vector<float>& b, std::vector<std::vector<float>> result)
  {
    for (size_t i = 0; i < a.size(); ++i) {
      for (size_t j = 0; j < b.size(); ++j) {
        result[i][j] = a[i] * b[j];
      }
    }
  }


  void OuterProductAdd(const std::vector<float>& a,
                       const std::vector<float>& b,
                       std::vector<std::vector<float>>& result)
  {
    for (size_t i = 0; i < a.size(); ++i) {
      for (size_t j = 0; j < b.size(); ++j) {
        result[i][j] += a[i] * b[j];
      }
    }
  }


  void HadamardProduct(const std::vector<float>& vectorA, const std::vector<float>& vectorB, std::vector<float>& result)
  {
    size_t size = vectorA.size();

    if ((vectorB.size() != size) || (result.size() != size)) {
      throw std::invalid_argument("Dimensionen stimmen nicht überein");
    }

    for (size_t i = 0; i < size; i++) {
      result[i] = vectorA[i] * vectorB[i]; 
    }
  }


  void FillRandomly(std::vector<float>& vector, float lowerBound, float upperBound)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(lowerBound, upperBound);
    for (float& element: vector) {
      element = dis(gen);
    }
  }


  void FillRandomly(std::vector<std::vector<float>>& matrix, float lowerBound, float upperBound)
  {
    for (std::vector<float>& row: matrix) {
      FillRandomly(row, lowerBound, upperBound);
    }
  }


  void FillRandomlyPyTorch(std::vector<float>& vector, size_t nInputFeatures)
  {
    float k = sqrtf(1.f / float(nInputFeatures));
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<float> dist(-k, k);

    for (float& element: vector) {
      element = dist(generator);
    }
  }


  void FillRandomlyPyTorch(std::vector<std::vector<float>>& matrix, size_t nInputFeatures)
  {
    for (std::vector<float>& row: matrix) {
      FillRandomlyPyTorch(row, nInputFeatures);
    }
  }


  void Shuffle(std::vector<std::vector<float>>& inputFeatures, std::vector<size_t>& labels)
  {
    size_t size = labels.size();

    if (inputFeatures.size() != size) {
      throw std::invalid_argument("Dimensionen stimmen nicht überein");
    }

    // Ein Index-Vektor wird erstellt, damit die Paarungen der Inputs und Labels beibehalten werden kann
    std::vector<size_t> index_vector;
    index_vector.reserve(size);

    for (size_t i = 0; i < size; i++) {
      index_vector.push_back(i);
    }

    
    std::random_device rd;  // Erstelle einen Seed
    std::default_random_engine rng(rd()); // Erstelle einen Pseudo-Zufallsgenerator mit dem Seed
    std::shuffle(index_vector.begin(), index_vector.end(), rng);

    std::vector<std::vector<float>> inputFeatures_shuffled;
    std::vector<size_t> labels_shuffled;

    inputFeatures_shuffled.reserve(size);
    labels_shuffled.reserve(size);

    // Anhand des geshuffleten Index-Vektors, werden somit die Input- und Label-Vektoren gleich geshufflet
    for (size_t i : index_vector) {
      inputFeatures_shuffled.push_back(inputFeatures[i]);
      labels_shuffled.push_back(labels[i]);
    }

    inputFeatures = std::move(inputFeatures_shuffled);
    labels = std::move(labels_shuffled);
  }


  void Zeros(std::vector<float>& vector)
  {
    std::fill(vector.begin(), vector.end(), 0.f);
  }

  void Zeros(std::vector<std::vector<float>>& matrix)
  {
    for (std::vector<float>& row: matrix) {
      Zeros(row);
    }
  }


  void Print(std::vector<std::vector<float>>& matrix)
  {
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();

    std::cout << "Matrix: " << rows << " x " << cols << std::endl;

    std::cout << "{ ";
    for (size_t row = 0; row < rows; row++) {
      std::cout << "[" << row << "]\t" << "{ ";

      for (size_t col = 0; col < cols; col++) {
        std::cout << matrix[row][col];

        if (col != cols - 1) std::cout << ", ";
      }

      std::cout << " }" << std::endl;
    }

    std::cout << " }" << std::endl;
  }


  void Print(std::vector<float>& vector)
  {
    size_t cols = vector.size();

    std::cout << "Vector: " << cols << " x 1" << std::endl;

    std::cout << "{ ";
    for (size_t col = 0; col < cols; col++) {
      std::cout << std::fixed << std::setprecision(4) << vector[col];

      if (col != cols - 1) std::cout << ", ";
    }

    std::cout << " }" << std::endl;
  }

}

