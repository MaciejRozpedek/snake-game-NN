#include "snake_training_manager.h"
#include <iostream>
#include <conio.h>
#include <chrono>
#include <iomanip>
#include <sstream>

SnakeTrainingManager::SnakeTrainingManager(int X, int Y, int numOfSnakes, int newGamesToRate, int newSeedChangeRate,
		double trainingSpeed, double mutationChance, int numOfThreads, std::vector<int> topology) : currentBestSnake(topology) {
	for (int i = 0; i < numOfSnakes; i++) {
		snakes.emplace_back(Snake(0.0, NeuralNetwork(topology)));
	}
	snakes.back().net.setTrainingSpeed(trainingSpeed);
	snakes.back().net.setNeuronMutationChance(mutationChance);
	currentBestSnake = NeuralNetwork(topology);
	allTimeBest = 0.0;
	generation = 0;
	my_numOfThreads = numOfThreads;
	currentBestFitness = 0.0;
	gamesToRate = newGamesToRate;
	seedChangeRate = newSeedChangeRate;
	SEED = time(NULL);
	for (int i = 0; i < numOfThreads + 1; i++) {
		games.emplace_back(SnakeGame(X, Y));
	}
	threads.resize(numOfThreads);
	loadSnakes();
}

void SnakeTrainingManager::looper() {
	while (true) {
		launchScoringThreads();
		threadsFinished.store(0);
		if (generation % 1 == 0) showData();
		while (threadsFinished.load() != threads.size()) userAction();
		joinScoringThreads();
		nextGeneration();
		if (generation % seedChangeRate == 0) SEED = rand();
		currentBestSnake = snakes.back().net;
		currentBestFitness = 0.0;
		for (auto &&snake : snakes) {
			currentBestFitness = std::max(currentBestFitness, snake.fitness);
		}
		
		if (currentBestFitness > allTimeBest) allTimeBest = currentBestFitness;
	}
}

void SnakeTrainingManager::loadSnakes() {
	char a;
	std::string fileName;
	std::cout << "Do you want to load snakes from file?\nY/N\n";
	std::cin >> a;
	if (a == 'y' || a == 'Y') {
		std::cout << "Insert filename\n";
		std::cin >> fileName;
		snakes[0].net.loadWeightsFromFile(fileName);
	} else {
		return;
	}
}

void SnakeTrainingManager::launchScoringThreads() {
	int snakesPerThread = snakes.size() / my_numOfThreads;
	int remaining = snakes.size() % my_numOfThreads;
	int start;
	int end = -1;
	// threads.clear();
	for (int i = 0; i < my_numOfThreads; i++) {
		start = end + 1;
		end = start + snakesPerThread - 1;
		if (remaining > 0) {
			remaining--;
			end++;
		}
		threads[i] = std::thread([this, i, start, end]{this->threadPlay(i, start, end);});
	}
}

void SnakeTrainingManager::joinScoringThreads() {
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
}

void SnakeTrainingManager::threadPlay(int index, int start, int end) {
	for (int snake = start; snake <= end; snake++) {
		unsigned my_seed = SEED;
		double fitness = 0.0;
		for (int j = 0; j < gamesToRate; j++) {
			games[index].setSeed(my_seed * (j + 1) % UINT_MAX);
			games[index].play(snakes[snake].net, false);
			double applesAte = games[index].getScore();
			double steps = games[index].getMovesMade();
			fitness += steps+(std::pow(2,applesAte)+std::pow(applesAte,2.1)*500)-(std::pow(applesAte,1.2)*std::pow(0.25*steps, 1.3));
		}
		snakes[snake].fitness = fitness;
		snakes[snake].fitness /= gamesToRate;
	}
	threadsFinished.fetch_add(1);
}

bool compareSnakes(const Snake &a, const Snake &b) {
	return a.fitness < b.fitness;
}
void SnakeTrainingManager::nextGeneration() {
	generation++;
	std::sort(snakes.begin(), snakes.end(), compareSnakes); // snakes[0] - the worst
	double totalScore = 0.0;
	for (auto &&snake : snakes) {
		totalScore += snake.fitness;
	}
	int iterator = 0;
	for (auto &&snake : snakes) {
		double numCopies = snake.fitness * snakes.size() / totalScore;
		for (double i = 1; i < numCopies; i++) {
			snakes[iterator] = snake;
			snakes[iterator].net.mutate();
			iterator++;
		}
		if (iterator > 0) {
			// std::cout<<" ";
		}
	}
	if(iterator == 1024) {
		// std::cout << "sth\n";
	}
	mutatedSnakes = iterator;
}

void SnakeTrainingManager::userAction() {
	if (_kbhit()) {
		int my_seed = SEED;
		char a;
		std::string fileName;
		std::cout << "What do you want to do?\ns - save\nt - adjust training speed\nm - adjust mutation chance\n"; 
		std::cout << "b - show best snake\n";
		std::cin >> a;
		switch (a) {
		case 's':
			std::cout << "Insert filename\n";
			std::cin >> fileName;
			currentBestSnake.saveWeightsToFile(fileName);
			break;

		case 't':
			std::cout << "Inert new training speed\n";
			double t;
			std::cin >> t;
			snakes[0].net.setTrainingSpeed(t);
			break;

		case 'm':
			std::cout << "Inert new neuron mutation chance\n";
			double m;
			std::cin >> m;
			snakes[0].net.setNeuronMutationChance(m);
			break;

		case 'b':
			std::cout << "Showing best snake:\n";
			_sleep(2000);
			for (int k = 0; k < gamesToRate; k++) {
				if (_kbhit()) {
					std::cin >> a;
					break;
				}
				games.back().setSeed(my_seed * (k + 1) % UINT_MAX);
				games.back().play(currentBestSnake, true);
				std::cout << games.back().lastDir << "\n";

				double applesAte = games.back().getScore();
				double steps = games.back().getMovesMade();
				double score = steps + (std::pow(2, applesAte) + std::pow(applesAte, 2.1) * 500) - (std::pow(applesAte, 1.2) * std::pow(0.25 * steps, 1.3));
				std::cout << "Fitness: " << score << "\n";
				_sleep(1500);
			}
			break;

		default:
			break;
		}
	}
}

std::string timeSinceLastCall() {
	 static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();

    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> timeDiff = currentTime - lastTime;
    double milliseconds = timeDiff.count();

    int minutes = static_cast<int>(milliseconds / 60000);
    milliseconds -= minutes * 60000;
    int seconds = static_cast<int>(milliseconds / 1000);
    milliseconds -= seconds * 1000;
	milliseconds = static_cast<int>(milliseconds);

	std::ostringstream oss;
    oss << "Time since last call: "
              << std::setfill('0') << std::setw(1) << minutes << ":"
              << std::setfill('0') << std::setw(2) << seconds << ":"
              << std::setfill('0') << std::setw(3) << milliseconds << "\n\n";

    lastTime = currentTime;
	return oss.str();
}

void SnakeTrainingManager::showData() {
	std::string out;
	std::stringstream ss;
	ss << "Generation: " << generation << "\n";
	ss <<"Best fitness: " << std::scientific << currentBestFitness << "\n";
	ss << "All time best: " << std::scientific << allTimeBest << "\n";
	ss << "Mutated snakes: " << mutatedSnakes << "\n";
	out += ss.str();
	out += timeSinceLastCall();
	std::cout << "\n\n\n\n\n\n";
	std::cout << "\x1b[6F";
	std::cout << out;
	// std::cout << "Generation: " << generation << "\nBest fitness: " << currentBestFitness << "\n";
	// std::cout << "All time best: " << allTimeBest << "\n";
	// std::cout << "Mutated snakes: " << mutatedSnakes << "\n\n" << std::flush;
}