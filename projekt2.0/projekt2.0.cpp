#include <iostream>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <conio.h>
#include <chrono>
#include <thread>

enum ShotResult { MISS, HIT, REFLECT };

class BlackBoxGame {
public:
    BlackBoxGame(int size);

    void moveCursor(char direction);
    void shootRay();
    void markAtom();
    void handleSpecialKeys(char key);
    void playGame();
    void displayBoard(bool showAtoms = false, bool showGuessedAtoms = false);
    int getCorrectGuesses() const;
    int getRemainingAtoms() const;
    void displaySolution();
    void help();
    void shoot();

private:
    int board_size;
    int num_atoms = static_cast<int>(num_atoms);
    std::vector<std::pair<int, int>> atoms;
    std::set<std::pair<int, int>> hidden_atoms;
    std::set<std::pair<int, int>> guessed_atoms;
    int correctGuesses;
    std::stack<std::set<std::pair<int, int>>> movesHistory;
    std::pair<int, int> cursorPosition;
    char currentDirection;
    void generateRandomNumAtoms();
    void generateAtoms();
    void placeHiddenAtoms();
    ShotResult shootRay(int row, int col);
    void guessAtoms();
    void evaluateGuess();
    void displayEmptyBoard();
    void displayHelp();
    void revealAtoms();
};

BlackBoxGame::BlackBoxGame(int size) : board_size(size), correctGuesses(0) {
    srand(static_cast<unsigned int>(time(0)));
    generateRandomNumAtoms();
    generateAtoms();
    placeHiddenAtoms();
}

void BlackBoxGame::generateRandomNumAtoms() {
    if (board_size == 5) {
        num_atoms = 3;
    }
    else if (board_size == 8) {
        num_atoms = rand() % 3 + 3;
    }
    else if (board_size == 10) {
        num_atoms = rand() % 4 + 5;
    }
}
void BlackBoxGame::revealAtoms() {
    std::cout << "Displaying the positions of hidden atoms:" << std::endl;

    for (const auto& atom : atoms) {
        std::cout << "Atom at position (" << atom.first << ", " << atom.second << ")." << std::endl;
    }
}


void BlackBoxGame::moveCursor(char direction) {
    int newRow, newCol;

    switch (direction) {
    case 'w':
        newRow = cursorPosition.first - 1;
        newCol = cursorPosition.second;
        break;
    case 's':
        newRow = cursorPosition.first + 1;
        newCol = cursorPosition.second;
        break;
    case 'a':
        newRow = cursorPosition.first;
        newCol = cursorPosition.second - 1;
        break;
    case 'd':
        newRow = cursorPosition.first;
        newCol = cursorPosition.second + 1;
        break;
    default:
        std::cout << "Invalid direction. Use 'w', 's', 'a', or 'd'." << std::endl;
        return;
    }

    // Sprawdź, czy nowa pozycja kursora jest w zakresie planszy
    if (newRow >= 0 && newRow < board_size && newCol >= 0 && newCol < board_size) {
        cursorPosition = { newRow, newCol };
        std::cout << "Cursor moved to position (" << newRow << ", " << newCol << ")." << std::endl;
    }
    else {
        std::cout << "Invalid move. Cursor cannot go outside the board." << std::endl;
    }
}



ShotResult BlackBoxGame::shootRay(int row, int col) {
    if (hidden_atoms.count({ row, col }) > 0) {
        hidden_atoms.erase({ row, col });
        return HIT;
    }
    else if (row < 0 || row >= board_size || col < 0 || col >= board_size) {
        return MISS;
    }
    else {
        return REFLECT;
    }
}

void BlackBoxGame::shootRay() {
    int newRow = cursorPosition.first, newCol = cursorPosition.second;

    ShotResult result = shootRay(newRow, newCol);

    if (result == HIT) {
        std::cout << "Hit! Atom detected (H)." << std::endl;
    }
    else if (result == REFLECT) {
        std::cout << "Reflection! Ray reflected (R)." << std::endl;
    }
    else {
        std::cout << "Miss! No atom detected." << std::endl;
    }

    displayBoard(true, true);  // Wyświetl planszę po zakończeniu ruchu
}


void BlackBoxGame::generateAtoms() {
    atoms.clear();
    for (int i = 0; i < num_atoms; ++i) {
        atoms.push_back({ rand() % board_size, rand() % board_size });
    }
}

void BlackBoxGame::placeHiddenAtoms() {
    hidden_atoms.clear();
    for (const auto& atom : atoms) {
        hidden_atoms.insert(atom);
    }
}




void BlackBoxGame::markAtom() {
    int row, col;
    std::cout << "Enter the position to mark as a possible atom (row col): ";
    std::cin >> row >> col;

    if (row >= 0 && row < board_size && col >= 0 && col < board_size) {
        guessed_atoms.insert({ row, col });
        std::cout << "Position (" << row << ", " << col << ") marked as a possible atom." << std::endl;
    }
    else {
        std::cout << "Invalid position. Atom must be marked within the board." << std::endl;
    }

    displayBoard(true, true);  // Wyświetl planszę po zakończeniu ruchu
}

void BlackBoxGame::handleSpecialKeys(char key) {
    switch (key) {
    case 'q':
    case 'Q':
        std::cout << "Exiting to the main menu." << std::endl;
        break;
    case 'u':
    case 'U':
        if (!movesHistory.empty()) {
            guessed_atoms = movesHistory.top();
            movesHistory.pop();
            std::cout << "Undo successful. Restoring the previous move." << std::endl;
            displayBoard(true, true);
        }
        else {
            std::cout << "Cannot undo. No moves available in the history." << std::endl;
        }
        break;
    case 'r':
    case 'R':
        if (!guessed_atoms.empty()) {
            movesHistory.push(guessed_atoms);
            std::cout << "Redo successful. Restoring the next move." << std::endl;
            displayBoard(true, true);
        }
        else {
            std::cout << "Cannot redo. No moves available to redo." << std::endl;
        }
        break;
    case '6':
        shootRay();
        break;
    case 'o':
    case 'O':
        markAtom();
        break;
    case 'k':
    case 'K':
        std::cout << "Ending the game and displaying the solution." << std::endl;
        displayBoard(true);
        std::cout << "Hidden atoms and their positions:" << std::endl;
        for (const auto& atom : atoms) {
            std::cout << "Atom at position (" << atom.first << ", " << atom.second << ")." << std::endl;
        }
        break;
    case 'p':
    case 'P':
        std::cout << "Displaying the solution. Pausing the game." << std::endl;
        displayBoard(true);
        break;
    case 'H':
        std::cout << "Displaying the positions of hidden atoms for a moment." << std::endl;
        displayHelp();
        break;
    default:
        std::cout << "Invalid key. Please use a valid special key." << std::endl;
    }
}

void BlackBoxGame::guessAtoms() {
    guessed_atoms.clear();

    std::cout << "Guess the position of atoms (row col):" << std::endl;
    for (int i = 0; i < num_atoms; ++i) {
        int row, col;
        std::cout << "Atom " << i + 1 << ": ";
        std::cin >> row >> col;

        guessed_atoms.insert({ row, col });
    }
}

void BlackBoxGame::evaluateGuess() {
    int currentCorrectGuesses = 0;

    for (const auto& guessedAtom : guessed_atoms) {
        if (hidden_atoms.count(guessedAtom) > 0) {
            std::cout << "Correct guess at position (" << guessedAtom.first << ", " << guessedAtom.second << ")." << std::endl;
            ++currentCorrectGuesses;
        }
        else {
            std::cout << "Incorrect guess at position (" << guessedAtom.first << ", " << guessedAtom.second << ")." << std::endl;
        }
    }

    std::cout << "Number of correctly guessed atoms: " << currentCorrectGuesses << "/" << num_atoms << std::endl;

    correctGuesses += currentCorrectGuesses;
}

void BlackBoxGame::displayBoard(bool showAtoms, bool showGuessedAtoms) {
    // Wyświetlanie planszy tylko raz po zakończeniu ruchu
    system("cls");  // Czyszczenie ekranu (Windows, dla Linux użyj "clear")

    for (int row = 0; row < board_size; ++row) {
        for (int col = 0; col < board_size; ++col) {
            if (showAtoms && hidden_atoms.count({ row, col }) > 0) {
                std::cout << "X ";
            }
            else if (showGuessedAtoms && guessed_atoms.count({ row, col }) > 0) {
                if (hidden_atoms.count({ row, col }) > 0) {
                    std::cout << "O ";
                }
                else {
                    std::cout << "X ";
                }
            }
            else if (cursorPosition.first == row && cursorPosition.second == col) {
                std::cout << "* "; // Oznaczenie pozycji kursora
            }
            else if (guessed_atoms.count({ row, col }) > 0) {
                std::cout << "+ "; // Oznaczenie miejsca, gdzie oddałeś strzał
            }
            else {
                std::cout << ". ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
void BlackBoxGame::displayEmptyBoard() {
    displayBoard(false, true);
}

void BlackBoxGame::displayHelp() {
    std::cout << "Displaying the positions of hidden atoms for a moment..." << std::endl;

    // Wyświetlenie planszy z ukrytymi atomami na krótko
    displayBoard(true, false);

    // Poczekaj przez chwilę
    std::this_thread::sleep_for(std::chrono::seconds(15));

    // Powrót do normalnego widoku planszy
    displayEmptyBoard();
}

void BlackBoxGame::displaySolution() {
    std::cout << "Displaying the solution. Pausing the game." << std::endl;
    revealAtoms();
    displayEmptyBoard();
}

void BlackBoxGame::help() {
    std::cout << "Displaying the positions of hidden atoms for a moment." << std::endl;
    displayHelp();
}

void BlackBoxGame::shoot() {
    shootRay();
    displayBoard(true, true);
}

void BlackBoxGame::playGame() {
    correctGuesses = 0;
    while (!movesHistory.empty()) {
        movesHistory.pop(); // Wyczyść historię ruchów
    }

    std::cout << "Welcome to Black Box Game (" << board_size << "x" << board_size << ")" << std::endl;

    // Wyświetl liczbę ukrytych atomów
    std::cout << "Number of hidden atoms: " << getRemainingAtoms() << std::endl;

    displayEmptyBoard();

    int shotNumber = 1;

    while (true) {
        char key;
        std::cout << "Enter 'w', 's', 'a', 'd' to move, 'q' to quit, 'u' to undo, 'r' to redo, '6' to shoot, 'o' to mark atom, 'k' to end the game, 'p' to display solution, 'H' for help, or any other key to continue: ";
        std::cin >> key;

        if (key == 'q') {
            std::cout << "Goodbye!" << std::endl;
            break;
        }

        if (key == 'H') {
            displayHelp();
            continue;
        }

        if (key == 'p') {
            std::cout << "Displaying the solution. Pausing the game." << std::endl;
            revealAtoms();
            displayEmptyBoard();
            continue;
        }

        if (key == 'k') {
            std::cout << "Ending the game and displaying the solution." << std::endl;
            revealAtoms();
            displayEmptyBoard();
            break;
        }

        if (key == 'u' || key == 'U' || key == 'r' || key == 'R' || key == ' ') {
            handleSpecialKeys(key);
        }
        else if (key == 'o' || key == 'O') {
            markAtom();
            displayBoard(false, true);
        }
        else {
            moveCursor(key);
            displayBoard(false, true);
        }
    }
}

int BlackBoxGame::getCorrectGuesses() const {
    return correctGuesses;
}

int BlackBoxGame::getRemainingAtoms() const {
    return hidden_atoms.size();
}

int main() {
    srand(time(0)); // Seed for random number generation

    int choice;
    BlackBoxGame* currentGame = nullptr;

    do {
        std::cout << "Black Box Game Menu" << std::endl;
        std::cout << "1. Start a new game" << std::endl;
        if (currentGame != nullptr) {
            std::cout << "2. Resume current game" << std::endl;
            std::cout << "3. Return to the main menu" << std::endl;
        }
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
        case 1: {
            int boardSize;
            std::cout << "Enter the board size (5, 8, or 10): ";
            std::cin >> boardSize;

            if (boardSize == 5 || boardSize == 8 || boardSize == 10) {
                if (currentGame != nullptr) {
                    delete currentGame; // Zwolnij pamięć po poprzedniej grze
                }
                currentGame = new BlackBoxGame(boardSize);

                // Dodaj wywołanie displayBoard, aby pokazać planszę
                currentGame->displayBoard(false, true);
                std::cout << "Number of hidden atoms: " << currentGame->getRemainingAtoms() << std::endl;
                currentGame->playGame();
                std::cout << "Game over. Number of correct guesses: " << currentGame->getCorrectGuesses() << std::endl;
            }
            else {
                std::cout << "Invalid board size. Please enter 5, 8, or 10." << std::endl;
            }
            break;
        }
        case 2:
            if (currentGame != nullptr) {
                currentGame->playGame();
                std::cout << "Game over. Number of correct guesses: " << currentGame->getCorrectGuesses() << std::endl;
            }
            else {
                std::cout << "No current game to resume. Please start a new game." << std::endl;
            }
            break;
        case 3:
            if (currentGame != nullptr) {
                delete currentGame; // Zwolnij pamięć przed powrotem do menu
                currentGame = nullptr;
            }
            std::cout << "Returning to the main menu." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please enter a valid option." << std::endl;
        }
    } while (choice != 0);

    if (currentGame != nullptr) {
        delete currentGame; // Zwolnij pamięć przed zakończeniem programu
    }

    return 0;
}