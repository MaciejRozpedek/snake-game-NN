#include "neural_network.h"
#include "snake_game.h"
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <conio.h>
#include "snake_training_manager.h"

#define NUM_OF_SNAKES 128
#define games_to_rate 10
// defines how often the seed for board generation is changed (in number of generations)
#define SEED_CHANGE_RATE 2
#define training_speed 0.1
#define mutation_chance 0.05

int main() {
	std::srand(std::time(0));
	std::vector<int> topology;
	topology.push_back(28);
	topology.push_back(20);
	topology.push_back(12);
	topology.push_back(4);
	std::cout << "Provide board dimensions\n";
	int x, y;
	std::cin >> x >> y;
	SnakeTrainingManager manager(x, y, NUM_OF_SNAKES, games_to_rate, SEED_CHANGE_RATE, training_speed, mutation_chance, 32, topology);
	manager.looper();
	return 0;
}