// -*- C++ -*-
/*
Created on 10/29/23.
==================================================
Authors: R.Lakos; A.Mithran
Emails: lakos@fias.uni-frankfurt.de; mithran@fias.uni-frankfurt.de
==================================================
*/

#include <iostream>
#include <cmath>

#include "MLPHandler.h"

// Schreibe das in das Terminal: ./HPCA_PC_MLP /home/users8/wioo/s9292109/Desktop/HLRA_Codes/Blatt_3/Exercise-03_HPCA-PC_MLP/mnist_datasets

int main(int argc, char* argv[])
{
  std::string filePath;

  if (argc > 1) {
    filePath = argv[1];
    std::cout << "File path provided: " << filePath << std::endl;

  } else {
    std::cout << "Usage: " << argv[0] << " <FILEPATH>" << std::endl;
    std::cout << "Example: " << argv[0] << " \"/home/username/Downloads\"" << std::endl;
    std::cout << "[Use the directory as path, were training- and test-file are located]" << std::endl;
    exit(1);
  }

  // topology: given as size of each layer (for MNIST, first layer size has to be 784, last layer size has to be 10)
  // activation: given as string, possible values: "None", "TanH", "LeakyReLU", "Softmax"

  // with using a hidden layer
  std::vector<size_t> topology = {784, 800, 10};
  std::vector<std::string> activations = {"None", "LeakyReLU", "Softmax"};

  // without using a hidden layer
  // std::vector<size_t> topology = {784, 10};
  // std::vector<std::string> activations = {"None", "Softmax"};

  // with using a hidden layer and the TanH activation function instead of LeakyReLU
  // std::vector<size_t> topology= {784, 800, 10};
  // std::vector<std::string> activations = {"None", "TanH", "Softmax"};

  MLPHandler mlp(topology,             // topology
                 activations,         // activation order
                 60000,    // nTrainingSamples
                 10000,     // nTestingSamples
                 10,              // nEpochs
                 10);           // batchSize

  mlp.ReadMNISTFiles(filePath);

  mlp.StartTraining();

  return 0;
}
