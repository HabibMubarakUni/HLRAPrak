// -*- C++ Header -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#ifndef HPCA_PC_MLP_MLPHANDLER_H
#define HPCA_PC_MLP_MLPHANDLER_H

#include "MLPLayer.h"

#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <numeric>

class MLPHandler
{
private:
  size_t nEpochs_ = 1;
  size_t batchSize_ = 1;

  size_t nInpFeatures_ = 728;
  size_t nOutFeatures_ = 10;
  size_t currentLabel_ = -1;
  size_t depth_ = -1;
  std::vector<size_t> topology_;
  std::vector<std::string> activations_;

  std::vector<MLPLayer> layers_;

  std::vector<float> outDeltas_;

  std::vector<std::vector<float>> inpFeaturesTraining_;
  std::vector<size_t> labelsTraining_;
  std::vector<std::vector<float>> inpFeaturesTesting_;
  std::vector<size_t> labelsTesting_;

  std::vector<float> accuracyTraining_;
  std::vector<float> accuracyTesting_;

  std::vector<float> currentLossTraining_;
  std::vector<float> currentLossTesting_;
  std::vector<float> epochLossTraining_;
  std::vector<float> epochLossTesting_;

public:
  /**
   * Default constructor.
   */
  MLPHandler() = default;

  /**
   * Constructor.
   * @param topology std::vector<size_t> reference to topology of MLP.
   * @param activations std::vector<std::string> reference to activation order of MLP.
   * @param nTrainingSamples size_t number of training samples.
   * @param nTestingSamples size_t number of testing samples.
   * @param nEpochs size_t number of epochs.
   * @param batchSize size_t number of samples per batch.
   */
  MLPHandler(std::vector<size_t>& topology,
             std::vector<std::string>& activations, size_t nTrainingSamples,
             size_t nTestingSamples, size_t nEpochs, size_t batchSize);

  /**
   * Function to start training of MLP.
   */
  void StartTraining();

  /**
   * Function to start testing of MLP.
   */
  void StartTesting();

  /**
   * Function to calculate Binary Cross-Entropy Loss of MLP.
   * @return float loss.
   */
  float BinaryCrossEntropyLoss();

  /**
      * Calculation of output neuron's deltas.
      * Attention: Simplified math only for (Softmax && Cross-Entropy Loss)!
      * For other loss function / output layer activation, use derivatives of loss and output layer neurons.
      **/
  void CalculateOutputDeltas(MLPLayer& outputLayer);


  /**
   * File reader for MNIST files.
   * @param path td::string reference to path of MNIST files' directory.
   */
  void ReadMNISTFiles(std::string& path);
};

#endif //HPCA_PC_MLP_MLPHANDLER_H
