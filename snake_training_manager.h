#pragma once

#include "neural_network.h"
#include "snake_game.h"
#include <thread>
#include <atomic>

struct Snake {
	double fitness = 0.0;
	NeuralNetwork net;

	Snake() = default;
	Snake(double _fitness, NeuralNetwork _net)
		: fitness(_fitness), net(_net) {
	}
};

class SnakeTrainingManager {
public:
	SnakeTrainingManager(int X, int Y, int numOfSnakes, int gamesToRate, int seedChangeRate,
			double trainingSpeed, double mutationChance, int numOfThreads, std::vector<int> topology);
	void looper();
private:
	void loadSnakes();
	void userAction(); // to check if user wants to do sth
	void nextGeneration();
	void launchScoringThreads();
	void joinScoringThreads();
	void threadPlay(int index, int start, int end);
	void showData();
	int generation;
	int my_numOfThreads;
	unsigned SEED;
	int gamesToRate;
	int seedChangeRate;
	int mutatedSnakes;
	double currentBestFitness;
	double allTimeBest;
	NeuralNetwork currentBestSnake;
	std::vector<Snake> snakes;
	std::vector<SnakeGame> games;
	std::vector<std::thread> threads;
	std::atomic<int> threadsFinished;
};