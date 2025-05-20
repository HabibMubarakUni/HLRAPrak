// -*- C++ Header -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#ifndef HPCA_PC_MLP_MLPLAYER_H
#define HPCA_PC_MLP_MLPLAYER_H

#include "Utils.h"

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdlib>
#include <algorithm>

class MLPLayer
{

private:
  float learningRate_ = 0.001f; 

  size_t inSize_ = 0;
  size_t layerSize_ = 0;

  std::vector<float> features_ = {};
  std::vector<float> derivatives_ = {};
  std::vector<float> deltas_ = {};

  std::vector<float> biases_ = {};
  std::vector<float> biasGradients_ = {};

  std::vector<std::vector<float>> weights_ = {};
  std::vector<std::vector<float>> weightGradients_ = {};


public:
  /**
   * Default constructor.
   */
  MLPLayer() = default;

  /**
   * Constructor.
   * @param inSize size_t size of input features.
   * @param layerSize size_t size of layer.
   * @param initialize bool flag to initialize weights and biases.
   */
  MLPLayer(size_t inSize, size_t layerSize, bool initialize) :
      inSize_(inSize),
      layerSize_(layerSize),
      features_(layerSize),
      derivatives_(layerSize),
      deltas_(layerSize),
      biases_(layerSize),
      biasGradients_(layerSize)
  {
    if (initialize) {
      weights_ = std::vector(layerSize_, std::vector<float>(inSize_));
      weightGradients_ = std::vector(layerSize_, std::vector<float>(inSize_));

      Utils::Zeros(weightGradients_);

      Utils::FillRandomlyPyTorch(weights_, inSize_);
      Utils::FillRandomlyPyTorch(biases_, inSize_);
    }
  }

  /**
   * Pass of input features into the input layer.
   * @param inFeatures std::vector<float> reference to input features.
   */
  void ForwardPassInput(const std::vector<float>& inFeatures)
  {
    features_ = inFeatures;
  }


  /**
   * Forward pass to current layer with features of previous layer.
   * @param inFeatures std::vector<float> reference with features of previous layer.
   */
  void ForwardPass(std::vector<float>& inFeatures)
  {
    Utils::AffineTransform(weights_, inFeatures, biases_, features_);
  }

  /**
   * Activation of current layer.
   * @param activation std::string reference with name of activation function.
   */
  void Activate(const std::string& activation)
  {
    if (activation == "Softmax") {
      ActivateSoftmax();
    } else if (activation == "TanH") {
      ActivateTanH();
    } else if (activation == "LeakyReLU") {
      ActivateLeakyReLU();
    } else if (activation == "None") {
      ActivateNone();
    } else {
      std::cout << "Error: Bad activation name provided." << std::endl;
    }
  }

  /**
   * No activation of current layer.
   */
  void ActivateNone()
  {
    std::fill(derivatives_.begin(), derivatives_.end(), 1.0f);
  }


  /**
   * Activation of current layer with TanH.
   */
  void ActivateTanH()
  {
    for (size_t i = 0; i < layerSize_; i++) {
      features_[i] = (expf(features_[i]) - expf(-features_[i])) / (expf(features_[i]) + expf(-features_[i]));
      derivatives_[i] = 1.f - features_[i] * features_[i];
    }
  }


  /**
   * Activation of current layer with LeakyReLU.
   */
  void ActivateLeakyReLU()
  {
    for (size_t i = 0; i < layerSize_; i++) {
      features_[i] = features_[i] > 0.f ? features_[i] : 0.01f * features_[i];
      derivatives_[i] = features_[i] > 0.f ? 1.f : 0.01f;
    }
  }


  /**
   * Activation of current layer with Softmax.
   */
  void ActivateSoftmax()
  {
    float max = *std::max_element(features_.begin(), features_.end());
    float sum = 0.f;

    for (size_t i = 0; i < layerSize_; i++) {
      features_[i] = expf(features_[i] - max);
      sum += features_[i];
    }

    for (size_t i = 0; i < layerSize_; i++) {
      features_[i] /= sum;
    }
  }


  /**
   * Calculation of hidden neuron's deltas.
   * @param nextLayerDeltas std::vector<float> reference to deltas of next layer.
   * @param weights std::vector<std::vector<float>> reference to weights of next layer.
   */
  void CalculateHiddenDeltas(const std::vector<float>& nextLayerDeltas,
                             const std::vector<std::vector<float>>& weights)
  {
    // mit der Transpose()- und MatVecMul()-Funktion
    // std::vector<std::vector<float>> weights_transpose;
    // Utils::Transpose(weights, weights_transpose);
    // Utils::MatVecMul(weights_transpose, nextLayerDeltas, deltas_);

    // mit der MatTransposeVecMul()-Funktion
    Utils::MatTransposeVecMul(weights, nextLayerDeltas, deltas_);

    Utils::HadamardProduct(deltas_, derivatives_, deltas_);
  }


  /**
   * Calculation of gradients for given layer
   * @param inFeatures std::vector<float> reference to input features.
   */
  void CalculateGradients(const std::vector<float>& inFeatures)
  {
    Utils::OuterProductAdd(deltas_, inFeatures, weightGradients_);
    Utils::VecAdd(deltas_, biasGradients_, biasGradients_);
  }


  /**
   * Update of weights of current layer.
   */
  void UpdateWeights()
  {
    for (size_t row = 0; row < weights_.size(); row++) {
      for (size_t col = 0; col < weights_[0].size(); col++) {
        weights_[row][col] = weights_[row][col] - (learningRate_ * weightGradients_[row][col]);
      }
    }
  }

  /**
  * Update of biases of current layer.
  */
  void UpdateBias()
  {
    for (size_t i = 0; i < biases_.size(); i++) {
      biases_[i] = biases_[i] - (learningRate_ * biasGradients_[i]);
    }
  }


  /**
   * Clear all (weights, biases) gradients of current layer.
   */
  void ClearGradients()
  {
    for (std::vector<float>& vector: weightGradients_) {
      Utils::Zeros(vector);
    }

    Utils::Zeros(biasGradients_);
  }


  /**
 * Returns the index of the neuron with the highest output value.
 */
  size_t ArgMaxOutputFeatures()
  {
    size_t maxIdx = 0;
    float max = features_[0];

    for (size_t i = 1; i < layerSize_; i++) {
      if (features_[i] > max) {
        max = features_[i];
        maxIdx = i;
      }
    }

    return maxIdx;
  }

  /**
   * Getter for features of current layer.
   * @return std::vector<float> reference to features of current layer.
   */
  std::vector<float>& GetFeatures() { return features_; }


  /**
   * Getter for weights of current layer.
   * @return std::vector<std::vector<float>> reference to weight matrix
   */
  std::vector<std::vector<float>>& GetWeights() { return weights_; }


  /**
   * Getter for derivatives of current layer.
   * @return std::vector<float> reference to derivatives of current layer.
   */
  std::vector<float>& GetDerivatives() { return derivatives_; }


  /**
   * Getter for deltas of current layer.
   * @return std::vector<float> reference to deltas of current layer.
   */
  std::vector<float>& GetDeltas() { return deltas_; }


  /**
   * Getter for gradient matrix of current layer.
   * @return std::vector<std::vector<float>> reference to gradient matrix of current layer.
   */
  std::vector<std::vector<float>>& GetGradientsWeights() { return weightGradients_; }

};


#endif //HPCA_PC_MLP_MLPLAYER_H
