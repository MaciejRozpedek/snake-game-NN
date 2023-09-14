#include "neural_network.h"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>
#include <fstream>

std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<double> randomWeightsGenerator(0.0, 0.5);

Neuron::Neuron(int numOfInputs) {
	for (int i = 0; i < numOfInputs; i++) {
		myWeights.push_back(randomWeight());
	}
}

void Neuron::setOutputVal(double output) {
	outputVal = output;
}

double Neuron::getOutputVal() const {
	return outputVal;
}

void Neuron::feedForward(const std::vector<Neuron>& prevLayer) {
	double sum = 0.0;

	for (int i = 0; i < prevLayer.size(); i++) {
		sum += prevLayer[i].getOutputVal() * myWeights[i];
	}

	outputVal = activationFunction(sum);
}

void Neuron::mutate(double chance, double scale) {
	for (int n = 0; n < myWeights.size(); n++) {
		if (std::rand() / double(RAND_MAX) < chance) {
			myWeights[n] += scale * ((2 * std::rand() / double(RAND_MAX)) - 1);
		}
		myWeights[n] = std::max(myWeights[n], -1.0);
		myWeights[n] = std::min(myWeights[n], 1.0);
	}
}

double Neuron::activationFunction(double x) {
	if (x < 0.0) return 0.0;
	return x;
	// return 1.0 / (1.0 + std::exp(-x));
	// return std::tanh(x);
}

double Neuron::randomWeight() {
	//// generates weights such as values further from 0 occurs more often. Mean of abs(weight) is 0.73
	double val = randomWeightsGenerator(gen);
	val = std::max(val, -1.0);
	val = std::min(val, 1.0);
	// if (val <= 0) val ++;
	// else val--;
	return val;
	// return 2 * std::rand() / double(RAND_MAX) - 1;
}

void Neuron::setWeights(std::vector<double> &newWeights) {
	myWeights.clear();
	myWeights = newWeights;
}

std::vector<double> Neuron::getWeights() {
	return myWeights;
}

double NeuralNetwork::trainingSpeed = 1.0; // values should be in  range [0.0, ... , 1.0]
double NeuralNetwork::neuronMutationChance = 0.1;

void NeuralNetwork::mutate() {
	for (int l = 0; l < layers.size(); l++) {
		for (int n = 0; n < layers[l].size(); n++) {
			layers[l][n].mutate(neuronMutationChance, trainingSpeed);
		}
	}
}

NeuralNetwork::NeuralNetwork(const std::vector<int> &topology) {
	myTopology = topology;
	int numOfLayers = topology.size();

	for (int layerNum = 0; layerNum < numOfLayers; layerNum++) {
		layers.push_back(Layer());	// create layer

		int numOfInputs = (layerNum == 0) ? 0 : topology[layerNum - 1] + 1; // first layer has no inputs, +1 is for BIAS

		for (int neuNum = 0; neuNum < topology[layerNum]; neuNum++) {
			// in first layer neurons have 0 inputs, in others inputs = prevLayer.size()
			layers.back().push_back(Neuron(numOfInputs));	// add normal neurons
		}
		if (layerNum != numOfLayers - 1) { // add in all, skipping last
			layers.back().push_back(Neuron(0));	// add BIAS
			layers.back().back().setOutputVal(1.0);	// output of BIAS is always 1.0, and weights from it to next layer are random
		}
	}
}

void NeuralNetwork::feedForward(const std::vector<double> &inputVals) {
	setInputs(inputVals);
	for (int layerNum = 1; layerNum < layers.size(); layerNum++) {
		Layer &prevLayer = layers[layerNum - 1];
		for (int n = 0; n < layers[layerNum].size() - 1; n++) {
			layers[layerNum][n].feedForward(prevLayer);
		}
	}
}

void NeuralNetwork::getOutputs(std::vector<double> &outputs) const {
	outputs.clear();

	for (int n = 0; n < layers.back().size(); n++) {
		outputs.push_back(layers.back()[n].getOutputVal());
	}
}

void NeuralNetwork::setInputs(const std::vector<double> &inputVals) {
	if (layers[0].size() - 1 != inputVals.size()) {
		std::cerr << "Error in file " << __FILE__ << " at line " << __LINE__ << ": wrong network topology or inputs\n";
		exit(1);
	}
	for (int i = 0; i < layers[0].size(); i++) {
		layers[0][i].setOutputVal(inputVals[i]);
	}
}

void NeuralNetwork::setTrainingSpeed(double newSpeed) {
	trainingSpeed = newSpeed;
}

void NeuralNetwork::setNeuronMutationChance(double newChance) {
	neuronMutationChance = newChance;
}

void NeuralNetwork::saveWeightsToFile(const std::string &fileName) {
	std::fstream file;
	file.open(fileName, std::fstream::out | std::fstream::trunc);
	file.close();
	file.open(fileName);
	std::vector<double> weights;
	for (int l = 1; l < layers.size(); l++) {
		for (int n = 0; n < layers[l].size(); n++) {
			weights = layers[l][n].getWeights();
			for (int i = 0; i < weights.size(); i++) {
				file << weights[i] << " ";
			}
			file << "\n";
		}
	}
	file.close();
}
void NeuralNetwork::loadWeightsFromFile(const std::string &fileName) {
	std::fstream file(fileName);
	std::vector<double> weights;
	double val;
	for (int l = 1; l < layers.size(); l++) {
		for (int n = 0; n < layers[l].size() - 1; n++) { // -1, because last neuron is BIAS.
			weights.clear();
			for (int i = 0; i < layers[l - 1].size(); i++) {
				file >> val;
				weights.push_back(val);
			}
			layers[l][n].setWeights(weights);
		}
	}
	std::cout <<"loaded weights\n";
	file.close();
}