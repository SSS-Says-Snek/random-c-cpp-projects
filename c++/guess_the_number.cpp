#include <iostream>
#include <random>

int main() {
    int num_guesses = 3;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10);
    
    std::cout << "Welcome to \"Guess the Number\"!\n";

    do {
        int user_num;

        std::cout << "Pick a number from 1 to 10: ";
        std::cin >> user_num;

        int rand_num = dist(rng);

        if (user_num == rand_num) {
            std::cout << "You got it! The number was " << rand_num << '\n';
            break;
        } else {
            num_guesses--;

            if (num_guesses > 0) {
                std::cout << "Incorrect; you have " << num_guesses << " guesses left.\n";
            } else {
                std::cout << "You lose! The number was " << rand_num << '\n';
            }
        }
    } while (num_guesses > 0);
}