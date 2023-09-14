#pragma once

#include <vector>
#include <string>

class Neuron;

typedef std::vector<Neuron> Layer;

class Neuron {
public:
	Neuron(int numOfInputs);
	void setOutputVal(double output);
	double getOutputVal() const;
	void feedForward(const std::vector<Neuron>& prevLayer);
	void mutate(double chance, double scale);
	std::vector<double> getWeights();
	void setWeights(std::vector<double> &newWeights);

private:
	static double activationFunction(double val);
	static double randomWeight();
	double outputVal;
	std::vector<double> myWeights;
};

class NeuralNetwork {
public:
	NeuralNetwork(const std::vector<int>& topology);
	void feedForward(const std::vector<double>& inputVals);
	void getOutputs(std::vector<double>& outputs) const;
	void setTrainingSpeed(double newSpeed);
	void setNeuronMutationChance(double newChance);
	void mutate();
	void saveWeightsToFile(const std::string &fileName);
	void loadWeightsFromFile(const std::string &fileName);

private:
	static double trainingSpeed;
	static double neuronMutationChance;
	std::vector<int> myTopology;
	std::vector<std::vector<Neuron>> layers;
	void setInputs(const std::vector<double>& inputVals);
};