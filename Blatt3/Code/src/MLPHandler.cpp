// -*- C++ -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#include "MLPHandler.h"


MLPHandler::MLPHandler(std::vector<size_t>& topology,
                       std::vector<std::string>& activations,
                       size_t nTrainingSamples,
                       size_t nTestingSamples,
                       size_t nEpochs,
                       size_t batchSize) :
    nEpochs_(nEpochs),
    batchSize_(batchSize),
    topology_{topology},
    activations_{activations},
    labelsTraining_(nTrainingSamples),
    labelsTesting_(nTestingSamples)
{
  nInpFeatures_ = topology_.front();
  nOutFeatures_ = topology_.back();
  depth_ = topology_.size();
  outDeltas_ = std::vector<float>(nOutFeatures_);
  inpFeaturesTraining_ = std::vector(nTrainingSamples, std::vector<float>(nInpFeatures_));
  inpFeaturesTesting_ = std::vector(nTestingSamples, std::vector<float>(nInpFeatures_));

  currentLossTraining_.reserve(nTrainingSamples);
  currentLossTesting_.reserve(nTestingSamples);

  epochLossTraining_.reserve(nEpochs_);
  epochLossTesting_.reserve(nEpochs_);

  // Special treatment of input layer as it doesn't have connections to any layers behind it
  // Also we set "initialize" to false to prevent weights, biases and other components from created
  MLPLayer inLayer = MLPLayer(0, topology_[0], false);
  layers_.push_back(inLayer);

  for (std::size_t i = 1; i < depth_; i++) {
    MLPLayer layer = MLPLayer(topology_[i - 1], topology_[i], true);
    layers_.push_back(layer);
  }
}


float MLPHandler::BinaryCrossEntropyLoss()
{
  MLPLayer& outputLayer = layers_.back();
  std::vector<float>& outValues = outputLayer.GetFeatures();

  float loss = -logf(outValues[size_t(currentLabel_)]);

  if (std::isinf(loss) || std::isnan(loss)) loss = 100.f;

  return loss;
}


void MLPHandler::CalculateOutputDeltas(MLPLayer& outputLayer)
{
  const std::vector<float>& outValues = outputLayer.GetFeatures();
  std::vector<float>& outDeltas = outputLayer.GetDeltas();

  for (size_t idx = 0; idx < nOutFeatures_; idx++) {
    outDeltas[idx] = outValues[idx] - (idx == currentLabel_ ? 1.f : 0.f);
  }
}


void MLPHandler::StartTraining()
{
  for (std::size_t epoch = 0; epoch < nEpochs_; epoch++) {
    size_t classifiedCorrectly = 0;
    size_t classifiedIncorrectly = 0;

    auto start = std::chrono::high_resolution_clock::now();


    Utils::Shuffle(inpFeaturesTraining_, labelsTraining_);

    size_t nBatches = inpFeaturesTraining_.size() / batchSize_;

    for (size_t batch = 0; batch < nBatches; batch++) {
      // std::cout << "Batch: " << batch << " / " << nBatches << std::endl;
      for (size_t batchElem = 0; batchElem < batchSize_; batchElem++) {
        size_t inpFeatureIdx = batch * batchSize_ + batchElem;
        const std::vector<float>& input_data = inpFeaturesTraining_[inpFeatureIdx];
        currentLabel_ = labelsTraining_[inpFeatureIdx];

        //--------------------------------------------------------------
        // Start FeedForward
        //--------------------------------------------------------------
        // Forward pass from dataset to the input layer
        layers_[0].ForwardPassInput(input_data);

        // Forward pass through all remaining layers including output layer
        for (size_t layerIdx = 1; layerIdx < depth_; layerIdx++) {
          layers_[layerIdx].ForwardPass(layers_[layerIdx - 1].GetFeatures());
          layers_[layerIdx].Activate(activations_[layerIdx]);
        }

        //--------------------------------------------------------------
        // Calculate the loss value (BCELoss)
        //--------------------------------------------------------------
        float loss = BinaryCrossEntropyLoss();
        currentLossTraining_.push_back(loss);

        //--------------------------------------------------------------
        // Evaluate the model's prediction
        //--------------------------------------------------------------
        if (layers_.back().ArgMaxOutputFeatures() == currentLabel_) {
          classifiedCorrectly++;
        } else {
          classifiedIncorrectly++;
        }

        //--------------------------------------------------------------
        // Start BackPropagation
        //--------------------------------------------------------------

        // Calculate gradient w.r.t features for the output layer
        CalculateOutputDeltas(layers_.back());
        // Calculate weights and biases gradient for the output layer
        layers_.back().CalculateGradients(layers_[depth_ - 2].GetFeatures());

        // Calculate gradient w.r.t features, weight gradients and bias gradients for the layers except input
        for (size_t layerIdx = depth_ - 2; layerIdx > 0; layerIdx--) {
          layers_[layerIdx].CalculateHiddenDeltas(layers_[layerIdx + 1].GetDeltas(),
                                                  layers_[layerIdx + 1].GetWeights());
          layers_[layerIdx].CalculateGradients(layers_[layerIdx - 1].GetFeatures());
        }
        // Nothing to do for the input layer

        //--------------------------------------------------------------
        // Update weights / gradients of the parameters
        //--------------------------------------------------------------
        if (batchElem == batchSize_ - 1) {
          // Nothing to do for the input layer as it was not created through forwardpass
          // and doesn't have weights and biases
          for (size_t layerIdx = 1; layerIdx < depth_; layerIdx++) {
            // update weights
            layers_[layerIdx].UpdateWeights();
            // update biases
            layers_[layerIdx].UpdateBias();
            // Clear gradients for the next iteration
            layers_[layerIdx].ClearGradients();
          }
        }
      }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = end - start;
    auto time = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

    std::cout << "\n--------------------------------------------------" << std::endl;
    std::cout << "[INFO] Epoch " << epoch + 1 << std::endl;
    std::cout << "[INFO] Training finished in " << time << " seconds.\n";

    std::cout << "[INFO] Accuracy Training: "
              << std::fixed
              << std::setprecision(2)
              << float(classifiedCorrectly) / float(classifiedCorrectly + classifiedIncorrectly) * 100.f
              << "%"
              << std::endl;

    float lossSum = std::accumulate(currentLossTraining_.begin(), currentLossTraining_.end(), 0.f);
    float lossMean = lossSum / float(currentLossTraining_.size());
    currentLossTraining_.clear();
    epochLossTraining_.push_back(lossMean);

    std::cout << "[INFO] Loss Training: " << lossMean << std::endl;

    StartTesting();
  }

  std::cout << std::endl << std::endl;
}

void MLPHandler::StartTesting()
{
  size_t classifiedCorrectly = 0;
  size_t classifiedIncorrectly = 0;

  auto start = std::chrono::high_resolution_clock::now();

  const std::size_t testSetSize = inpFeaturesTesting_.size();
  for (size_t sampleIdx = 0; sampleIdx < testSetSize; sampleIdx++) {

    const std::vector<float>& inputData = inpFeaturesTesting_[sampleIdx];
    currentLabel_ = labelsTesting_[sampleIdx];

    //--------------------------------------------------------------
    // Start FeedForward
    //--------------------------------------------------------------
    // Forward pass from dataset to the input layer
    layers_[0].ForwardPassInput(inputData);

    // Forward pass through all remaining layers including output layer
    for (size_t layerIdx = 1; layerIdx < depth_; layerIdx++) {
      layers_[layerIdx].ForwardPass(layers_[layerIdx - 1].GetFeatures());
      layers_[layerIdx].Activate(activations_[layerIdx]);
    }

    //--------------------------------------------------------------
    // Calculate the loss value (BCELoss)
    //--------------------------------------------------------------
    float loss = BinaryCrossEntropyLoss();
    currentLossTesting_.push_back(loss);

    //--------------------------------------------------------------
    // Evaluate the model's prediction
    //--------------------------------------------------------------
    if (layers_.back().ArgMaxOutputFeatures() == currentLabel_) {
      classifiedCorrectly++;
    } else {
      classifiedIncorrectly++;
    }

    //--------------------------------------------------------------
    // No BackPropagation for test dataset
    //--------------------------------------------------------------

    //--------------------------------------------------------------
    // No weight update / update of gradients for test dataset
    //--------------------------------------------------------------
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration = end - start;
  auto time = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

  std::cout << "[INFO] Testing finished in " << time << " seconds.\n";

  std::cout << "[INFO] Accuracy Testing: "
            << std::fixed
            << std::setprecision(2)
            << float(classifiedCorrectly) / float(classifiedCorrectly + classifiedIncorrectly) * 100.f
            << "%"
            << std::endl;

  float lossSum = std::accumulate(currentLossTesting_.begin(), currentLossTesting_.end(), 0.f);
  float lossMean = lossSum / float(currentLossTesting_.size());
  currentLossTesting_.clear();
  epochLossTesting_.push_back(lossMean);

  std::cout << "[INFO] Loss Testing: " << lossMean << std::endl;
}

void MLPHandler::ReadMNISTFiles(std::string& path)
{
  std::string fileNameTraining = "/mnist_train.csv";
  std::string fileNameTesting = "/mnist_test.csv";

  std::cout << "[INFO] Reading training file..." << std::endl;
  std::ifstream fileTraining(path + fileNameTraining);
  std::cout << "[INFO] File name: " << path + fileNameTraining << std::endl;

  // read file, line by line - first digit is label, rest is pixel values
  std::string line;
  std::size_t lineIdx = 0;
  while (std::getline(fileTraining, line)) {
    if (line.empty()) continue;
    if (lineIdx >= inpFeaturesTraining_.size()) break;

    std::stringstream ss(line);
    std::string pixelValue;
    std::vector<float> pixelValues;
    pixelValues.reserve(nInpFeatures_);
    bool isLabel = true;

    while (std::getline(ss, pixelValue, ',')) {
      if (pixelValue.empty()) continue;
      if (isLabel) {
        labelsTraining_[lineIdx] = size_t(std::stof(pixelValue));
        isLabel = false;
      }
        // scale pixel values from 0-255 to 0-1
      else {
        pixelValues.push_back(std::stof(pixelValue) / 255.f);
      }
    }

    inpFeaturesTraining_[lineIdx] = pixelValues;
    lineIdx++;
  }

  lineIdx = 0;

  std::cout << "[INFO] Reading test file..." << std::endl;
  std::ifstream fileTesting(path + fileNameTesting);
  std::cout << "[INFO] File name: " << path + fileNameTesting << std::endl;

  while (std::getline(fileTesting, line)) {
    if (lineIdx >= inpFeaturesTesting_.size()) break;

    std::stringstream ss(line);
    std::string pixelValue;
    std::vector<float> pixelValues;
    pixelValues.reserve(nInpFeatures_);
    bool isLabel = true;

    while (std::getline(ss, pixelValue, ',')) {
      if (isLabel) {
        labelsTesting_[lineIdx] = size_t(std::stof(pixelValue));
        isLabel = false;
      }
        // scale pixel values from 0-255 to 0-1
      else {
        pixelValues.push_back(std::stof(pixelValue) / 255.f);
      }
    }

    inpFeaturesTesting_[lineIdx] = pixelValues;
    lineIdx++;
  }
}



