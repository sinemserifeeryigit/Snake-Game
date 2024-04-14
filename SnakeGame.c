#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BOARD_SIZE 3

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
    int emptyRow;
    int emptyCol;
} PuzzleBoard;

typedef struct {
    PuzzleBoard board;
    int moveCount;
    int heuristic;
    struct Node* parent;
} Node;

void initializeBoard(PuzzleBoard* puzzleBoard) {
    int num = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            puzzleBoard->board[i][j] = num++;
        }
    }
    puzzleBoard->emptyRow = BOARD_SIZE - 1;
    puzzleBoard->emptyCol = BOARD_SIZE - 1;
    puzzleBoard->board[BOARD_SIZE - 1][BOARD_SIZE - 1] = 0;
}

void shuffleBoard(PuzzleBoard* puzzleBoard) {
    srand(time(NULL));
    for (int i = 0; i < 1000; i++) {
        int direction = rand() % 4;
        int newRow = puzzleBoard->emptyRow;
        int newCol = puzzleBoard->emptyCol;

        if (direction == 0 && newRow > 0) {
            newRow--;
        } else if (direction == 1 && newRow < BOARD_SIZE - 1) {
            newRow++;
        } else if (direction == 2 && newCol > 0) {
            newCol--;
        } else if (direction == 3 && newCol < BOARD_SIZE - 1) {
            newCol++;
        }

        puzzleBoard->board[puzzleBoard->emptyRow][puzzleBoard->emptyCol] = puzzleBoard->board[newRow][newCol];
        puzzleBoard->board[newRow][newCol] = 0;
        puzzleBoard->emptyRow = newRow;
        puzzleBoard->emptyCol = newCol;
    }
}

void saveBoardToFile(const PuzzleBoard* puzzleBoard, const char* filename) {
    FILE* file = fopen(filename, "a");
    if (file != NULL) {
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                fprintf(file, "%d ", puzzleBoard->board[i][j]);
            }
            fprintf(file, "\n");
        }
        fprintf(file, "---------\n");
        fclose(file);
    } else {
        printf("Unable to save the board to file.\n");
    }
}


void printBoard(const PuzzleBoard* puzzleBoard) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (puzzleBoard->board[i][j] == 0) {
                printf("  ");
            } else {
                printf("%d ", puzzleBoard->board[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int isMoveValid(const PuzzleBoard* puzzleBoard, int newRow, int newCol) {
    if (newRow < 0 || newRow >= BOARD_SIZE || newCol < 0 || newCol >= BOARD_SIZE) {
        return 0; // Invalid move, out of bounds
    }

    int emptyRow = puzzleBoard->emptyRow;
    int emptyCol = puzzleBoard->emptyCol;

    if ((newRow == emptyRow && (newCol == emptyCol - 1 || newCol == emptyCol + 1)) ||
        (newCol == emptyCol && (newRow == emptyRow - 1 || newRow == emptyRow + 1))) {
        return 1; // Valid move
    }

    return 0; // Invalid move, not adjacent to the empty space
}


int makeMove(PuzzleBoard* puzzleBoard, int number, int newRow, int newCol) {
    // Check if the move is valid
    if (!isMoveValid(puzzleBoard, newRow, newCol)) {
        return 0; // Invalid move
    }

    // Find the number in the board
    int currentRow = -1;
    int currentCol = -1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (puzzleBoard->board[i][j] == number) {
                currentRow = i;
                currentCol = j;
                break;
            }
        }
        if (currentRow != -1) {
            break;
        }
    }

    // Move the number
    puzzleBoard->board[puzzleBoard->emptyRow][puzzleBoard->emptyCol] = puzzleBoard->board[currentRow][currentCol];
    puzzleBoard->board[currentRow][currentCol] = 0;
    puzzleBoard->emptyRow = currentRow;
    puzzleBoard->emptyCol = currentCol;

    return 1; // Move successful
}

int isPuzzleSolved(const PuzzleBoard* puzzleBoard) {
    int num = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (puzzleBoard->board[i][j] != num && !(i == BOARD_SIZE - 1 && j == BOARD_SIZE - 1 && puzzleBoard->board[i][j] == 0)) {
                return 0; // Puzzle is not solved
            }
            num++;
        }
    }
    return 1; // Puzzle is solved
}

void playGameAsUser() {
    PuzzleBoard puzzleBoard;
    initializeBoard(&puzzleBoard);
    shuffleBoard(&puzzleBoard);
    saveBoardToFile(&puzzleBoard, "board.txt");
    int direction;

    int moves = 0;
    int number, row, col;
    while (!isPuzzleSolved(&puzzleBoard)) {
        printf("Current board state:\n");
        printBoard(&puzzleBoard);

        printf("Enter the number you want to move and the direction (e.g., 2-R): ");
        scanf("%d-%c", &number, &direction);

        int newRow = puzzleBoard.emptyRow;
        int newCol = puzzleBoard.emptyCol;

        if (direction == 'D') {
            newRow--;
        } else if (direction == 'U') {
            newRow++;
        } else if (direction == 'R') {
            newCol--;
        } else if (direction == 'L') {
            newCol++;
        }

        if (!makeMove(&puzzleBoard, number, newRow, newCol)) {
            printf("Invalid move. Try again.\n");
            continue;
        }

        moves++;
        saveBoardToFile(&puzzleBoard, "board.txt");
    }

    int score = 1000 - 10 * moves;
    printf("Congratulations! You solved the puzzle.\n");
    printf("Total number of moves: %d\n", moves);
    printf("Your score: %d\n", score);

    // Save best score to file if it's the best
    int bestScore;
    FILE* bestScoreFile = fopen("best_score.txt", "r");
    if (bestScoreFile != NULL) {
        fscanf(bestScoreFile, "%d", &bestScore);
        fclose(bestScoreFile);
    } else {
        bestScore = score;
    }

    if (score > bestScore) {
        FILE* bestScoreFile = fopen("best_score.txt", "w");
        if (bestScoreFile != NULL) {
            fprintf(bestScoreFile, "%d", score);
            fclose(bestScoreFile);
            printf("Congratulations! You achieved the best score!\n");
        } else {
            printf("Unable to save the best score.\n");
        }
    }

    printf("\n");
}

void auto_finish_helper(PuzzleBoard* puzzleBoard, int moves) {
    printf("Current board state:\n");
    printBoard(puzzleBoard);

    if (isPuzzleSolved(puzzleBoard)) {
        printf("Puzzle solved by the PC!\n");
        printf("Total number of moves: %d\n", moves);
        saveBoardToFile(puzzleBoard, "board.txt");
        return;
    }

    int number, row, col;
    int direction;
    int validMove = 0;

    while (!validMove) {
        number = rand() % 8 + 1;
        direction = rand() % 4;

        int newRow = puzzleBoard->emptyRow;
        int newCol = puzzleBoard->emptyCol;

        if (direction == 0) {
            newRow--;
        } else if (direction == 1) {
            newRow++;
        } else if (direction == 2) {
            newCol--;
        } else if (direction == 3) {
            newCol++;
        }

        validMove = makeMove(puzzleBoard, number, newRow, newCol);
    }

    moves++;
    saveBoardToFile(puzzleBoard, "board.txt");

    auto_finish_helper(puzzleBoard, moves);
}

void auto_finish() {
    PuzzleBoard puzzleBoard;
    initializeBoard(&puzzleBoard);
    shuffleBoard(&puzzleBoard);
    saveBoardToFile(&puzzleBoard, "board.txt");
    int moves = 0;

    auto_finish_helper(&puzzleBoard, moves);
}


void showBestScore() {
    int bestScore;
    FILE* bestScoreFile = fopen("best_score.txt", "r");
    if (bestScoreFile != NULL) {
        fscanf(bestScoreFile, "%d", &bestScore);
        fclose(bestScoreFile);
        printf("The best score is: %d\n", bestScore);
    } else {
        printf("Unable to read the best score.\n");
    }
}


void mainMenu() {
    int choice;
    do {
        printf("Puzzle Game\n");
        printf("1. Finish the game with user\n");
        printf("2. Finish the game with PC\n");
        printf("3. Show the best score\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                playGameAsUser();
                break;
            case 2:
                auto_finish();
                break;
            case 3:
                showBestScore();
                break;
            case 4:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);
}

int main() {
    mainMenu();
    return 0;
}


