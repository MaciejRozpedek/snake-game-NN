# SNAKE GAME Neural Network

Train a neural network to play the classic Snake game with customizable fitness functions.

## Features
 - Train a neural network to play Snake.
 - Customize the fitness function e.g. $f(S, A) = S + 2^{A} + 500*A^{2.1} - A^{1.2} - (\frac{1}{4}S)^{1.3}$, where S - steps taken, A - apples ate.
 - User-definable parameters for training.
 - Real-time gameplay visualization.
 - Save and load trained models.

## Usage
1. **Prerequisites**: Ensure you have a C++ compiler (e.g., g++) installed.

2. **Instalation**: Clone the repository and then compile all *.cpp files
```bash
git clone https://github.com/MaciejRozpedek/snake-game-NN.git
```
## Customization
In main.cpp file there are some parameters you can customize:
 - NUM_OF_SNAKES: The number of snakes
 - GAMES_TO_RATE: Each snake is rated after playing X games
 - SEED_CHANGE_RATE: For every X generations, the same seed is used for generating apples
 - TRAINING_SPEED: A value of 1.0 indicates that weights are mutated by a random number within the interval (-1.0, 1.0)
 - MUTATION_CHANCE: The probability that a weight will be mutated. Use a number between 0.0 and 1.0


You can also customize the neural network architecture by specifying the number of layers and the number of neurons in each layer, allowing you to balance between training speed and model complexity to achieve optimal performance for your Snake-playing AI

## License
This project is licensed under the MIT License.
