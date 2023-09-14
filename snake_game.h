#pragma once

#include <queue>
#include "neural_network.h"

enum Cell {
	BLANK = 0, SNAKE = 1, HEAD = 2, APPLE = 3, BOARDER = 4
};

enum Face {NORTH = 1, EAST = 2, SOUTH = 3, WEST = 4};

class SnakeGame {
public:
	SnakeGame(int maxX, int maxY);
	void play(NeuralNetwork &net, bool showGame, bool userPlay = false);
	void drawGame();
	int getScore();
	int getMovesMade();
	void setSeed(unsigned newSeed);
	Face lastDir;

private:
	void initializeGame();
	bool move(Face direction);
	bool generateFood();
	void clearBoard();
	void summonSnake();
	void prepareInputs(std::vector<double> &inputVals);
	void prepareInputHelper(int headPos, int posChange, double &boarderInput, double &snakeInput, double &appleInput);
	unsigned my_rand();
	Face determineNetMove(std::vector<double> &outputs);
	std::vector<Cell> board; // 0-nothing, 1-snake, 2-head, 3-apple, 4-boarder
	int max_x;
	int max_y;
	int score;
	int movesToEnd;
	int movesTaken;
	unsigned seed;	// for random numbers
	std::queue<short> snake;	// back() is head, front() is tail
	Face face;
};