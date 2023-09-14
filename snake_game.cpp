#include "snake_game.h"
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <conio.h>

SnakeGame::SnakeGame(int maxX, int maxY) {
	max_x = maxX + 2;	// additional + 2 to store BORDER on edges
	max_y = maxY + 2;
}

void SnakeGame::clearBoard() {
	int maxVal = max_x * max_y + 1;
	board.resize(maxVal);
	for (int i = 1; i <= maxVal; i++) {
		if (i % max_x == 0 || i % max_x == 1 || i < max_x || i > max_x * (max_y-1)) board[i] = BOARDER;
		else board[i] = BLANK;
	}
}
void gotoxy(int x, int y) 
{ 
   COORD coord; 
   coord.X = x; 
   coord.Y = y; 
   SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord); 
} 
void SnakeGame::play(NeuralNetwork &net, bool showGame, bool userPlay) {
	std::vector<double> inputVals;
	std::vector<double> outputs;
	Face direction = EAST;
	initializeGame();
	// Force 2 moves to right
	move(direction), move(direction);
	inputVals.resize(28);
	if(showGame) system("cls");
	if (!userPlay)
	while (true) {
		if (showGame) {
			gotoxy(0, 0);
			// system("cls");
			drawGame();
			Sleep(100);
		}
		prepareInputs(inputVals);
		net.feedForward(inputVals);
		net.getOutputs(outputs);
		direction = determineNetMove(outputs);
		lastDir = direction;
		if(!move(direction)) {
			break;
		}
	}

	if (userPlay)
	while (true) {
		gotoxy(0, 0);
		drawGame();
		char input;
		Face direction;
		while (true) if (kbhit()) break;
		input = _getch();
		std::cin.clear();
		// std::cin >> input;
		switch (input) {
		case 'w':
			direction = NORTH;
			break;
		case 'a':
			direction = WEST;
			break;
		case 's':
			direction = SOUTH;
			break;
		case 'd':
			direction = EAST;
			break;
		default:
			std::cout << "Nieprawidłowy klawisz!\n";
			continue;
			break;
		}
		if (!move(direction)) {
			break;
		}
	}
}

void SnakeGame::initializeGame() {
	clearBoard();
	summonSnake();
	generateFood();
	score = 0;
	movesToEnd = (max_x-2) * (max_y-2);
	movesTaken = 0;
}

Face SnakeGame::determineNetMove(std::vector<double> &outputs) {
	Face direction = NORTH;
	double maxOutput = outputs[0];
	int dirCode;
	for (int i = 1; i < outputs.size(); i++) {
		if (outputs[i] > maxOutput) {
			maxOutput = outputs[i];
			dirCode = i + 1;
		}
	}
	switch (dirCode) {
        case 1:
            direction = NORTH;
			break;
        case 2:
            direction = EAST;
			break;
        case 3:
            direction = SOUTH;
			break;
        case 4:
            direction = WEST;
			break;
    }
	return direction;
}

void SnakeGame::prepareInputs(std::vector<double> &inputVals) {
	for (int i = 0; i < inputVals.size(); i++) inputVals[i] = 0.0;
	int headPos = snake.back();
	prepareInputHelper(headPos, -max_x,     inputVals[0],  inputVals[1],  inputVals[2]);
	prepareInputHelper(headPos, -max_x + 1, inputVals[3],  inputVals[4],  inputVals[5]);
	prepareInputHelper(headPos, 1,          inputVals[6],  inputVals[7],  inputVals[8]);
	prepareInputHelper(headPos, max_x + 1,  inputVals[9],  inputVals[10], inputVals[11]);
	prepareInputHelper(headPos, max_x,      inputVals[12], inputVals[13], inputVals[14]);
	prepareInputHelper(headPos, max_x - 1,  inputVals[15], inputVals[16], inputVals[17]);
	prepareInputHelper(headPos, -1,         inputVals[18], inputVals[19], inputVals[20]);
	prepareInputHelper(headPos, -max_x - 1, inputVals[21], inputVals[22], inputVals[23]);
	switch (face) {
	case NORTH:
		inputVals[24] = 1.0;
		break;
	case EAST:
		inputVals[25] = 1.0;
		break;
	case SOUTH:
		inputVals[26] = 1.0;
		break;
	case WEST:
		inputVals[27] = 1.0;
		break;
	}
}

void SnakeGame::prepareInputHelper(int headPos, int posChange, double &boarderInput, double &snakeInput, double &appleInput) {
	boarderInput = 0.0, snakeInput = 0.0, appleInput = 0.0;
	for (double i = 0.0; i <= 10.0; i++) {
		headPos += posChange;
		switch (board[headPos]) {
		case BOARDER:
			if (i <= 4.0) boarderInput = 1.0 - i / 4.0;
			return;	// when encountered boarder, return
		case SNAKE:
			// snakeInput = 1.0;
			if (i == 0.0) snakeInput = 1.0; // 1 if tail is next to head, else 0
			// snakeInput = max(snakeInput, 1 - i / 5);
			break;
		case APPLE:	//	if apple is behind snake, than snake can't see an apple
			if (snakeInput > 0.0) break;
			appleInput = 1.0;
			// appleInput = max(appleInput, 1 - i / 5);
			break;
		}
	}
}

bool SnakeGame::move(Face direction) {
	if (movesToEnd == 0) return false;
	movesTaken++;
	movesToEnd--;
	int startPos = snake.back();
	int nextPos = snake.back();
	switch (direction) {
	case NORTH:
		nextPos -= max_x;
		break;
	case EAST:
		nextPos++;
		break;
	case SOUTH:
		nextPos += max_x;
		break;
	case WEST:
		nextPos--;
		break;
	}
	if (board[nextPos] == BOARDER || board[nextPos] == SNAKE) return false;
	else {
		board[startPos] = SNAKE;
		if (board[nextPos] == APPLE) {
			score++;
			// movesToEnd += ((max_x-2) + (max_y-2)) / 2;
			movesToEnd = (max_x - 2) * (max_y - 2);
			if (!generateFood()) {
				return false;
			}
		} else {
			board[snake.front()] = BLANK;
			snake.pop();
		}
		board[nextPos] = HEAD;
		snake.push(nextPos);
		return true;
	}
}

bool SnakeGame::generateFood() {
	if (snake.size() > min(max_x, max_y)) {
		std::vector<int> vec;
		for (int i = 1; i < board.size(); i++) {
			if (board[i] == BLANK) {
				vec.emplace_back(i);
			}
		}
		if (vec.size() == 0) return false;
		board[vec[my_rand() % vec.size()]] = APPLE;
		return true;
	}
	
	int counter = 0;
	// if there is no space left return
	if (snake.size() >= (max_x - 2) * (max_y - 2) - 1) return false;
	int appleX = my_rand() % (max_x - 2) + 2;
	int appleY = my_rand() % (max_y - 2) + 1;
	int appleVal = appleY * max_x + appleX;
	while (board[appleVal] != BLANK) {
		counter++;
		appleX = 2 + (int)((max_x - 2) * 1.0 * my_rand() / (UINT_MAX + 1.0));
		appleY = 1 + (int)((max_y - 2) * 1.0 * my_rand() / (UINT_MAX + 1.0));
		// appleX = my_rand() % (max_x - 2) + 2;
		// appleY = my_rand() % (max_y - 2) + 1;
		appleVal = appleY * max_x + appleX;
		if (counter % 10000 == 0) {
			std::cout << "_" << counter / 10000 << ", ";
			
		}
	}
	board[appleVal] = APPLE;
	return true;
}

void SnakeGame::summonSnake() {
	std::queue<short> empty;
	std::swap(snake, empty);
	int yPos = (max_y - 1) / 2;
	int centerCell = max_x * yPos + max_x / 2;
	if (max_x % 2 == 1) centerCell++;
	snake.push(centerCell - 1);
	board[centerCell - 1] = SNAKE;
	snake.push(centerCell);
	board[centerCell] = SNAKE;
	snake.push(centerCell + 1);
	board[centerCell + 1] = HEAD;
	face = EAST;
}

void SnakeGame::drawGame() {
	for (int i = 1; i < max_x * 2; i++) std::cout << "#";
	std::cout << "\n";
	for (int i = max_x + 1; i < board.size() - max_x; i++) {
		switch (board[i]) {
		case BLANK:
			std::cout << "-";
			break;
		case SNAKE:
			std::cout << "x";
			break;
		case HEAD:
			std::cout << "O";
			break;
		case APPLE:
			std::cout << "@";
			break;
		case BOARDER:
			std::cout << "#";
			break;
		}
		std::cout << " ";
		if (i % max_x == 0) std::cout << "\n";
	}
	for (int i = 1; i < max_x * 2; i++) std::cout << "#";
	std::cout << "\n";
	std::cout << "Score: " << score << "\n";
	std::cout << "Moves left: " << movesToEnd << "   \n";
}

int SnakeGame::getScore() {
	return score;
}

int SnakeGame::getMovesMade() {
	return movesTaken;
}

void SnakeGame::setSeed(unsigned newSeed) {
	seed = newSeed;
}

unsigned SnakeGame::my_rand() {
	seed *= 1103515245; // parameter from rand()
	seed += 12345;
	seed %= 2147483648; // 2^31
	return seed;
}