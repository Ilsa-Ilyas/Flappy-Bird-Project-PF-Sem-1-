#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>

/*
    Flappy Bird Game in C
    ---------------------
    Description:
    - The player controls a bird navigating through gaps in pipes.
    - Avoid collisions with pipes and the ground.
    - Use the menu to start the game, view instructions, or select bird characters.
*/

// Game Configuration
#define SCREEN_WIDTH 90      // Width of the game screen
#define SCREEN_HEIGHT 26     // Height of the game screen
#define WIN_WIDTH 70         // Width of the playable window
#define MENU_WIDTH 20        // Width of the menu area
#define GAP_SIZE 7           // Gap size between the upper and lower pipes
#define PIPE_DIF 45          // Difference between consecutive pipes

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

// Game Variables
int pipePos[3];               // Positions of the pipes
int gapPos[3];                // Positions of the gaps in the pipes
int pipeFlag[3];              // Flags indicating if pipes are active
char bird1[2][6] = {          // Design for Bird 1
    {'/', '-', '-', 'o', '\\', ' '},
    {'|', '_', '_', '_', ' ', '>'}
};
char bird2[2][6] = {          // Design for Bird 2
    {'/', '-', 'o', '-', '\\', ' '},
    {'|', '^', '_', '^', ' ', '>'}
};
char bird[2][6];              // Current bird design
int birdPos = 6;              // Bird's vertical position
int score = 0;                // Current score

// Function to position the cursor
void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

// Function to set cursor visibility
void setcursor(int visible, DWORD size) {
    if (size == 0)
        size = 20;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

// Functions related to the game border
void drawBorder() {
    // Draw the top and bottom borders
    for (int i = 0; i < SCREEN_WIDTH; i++) {
        gotoxy(i, 0);
        printf("±");
        gotoxy(i, SCREEN_HEIGHT);
        printf("±");
    }
    // Draw the left and right borders
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(0, i);
        printf("±");
        gotoxy(SCREEN_WIDTH, i);
        printf("±");
    }
    // Draw the separator between playable window and menu
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        gotoxy(WIN_WIDTH, i);
        printf("±");
    }
}

// Functions related to pipes
void genPipe(int ind) {
    // Randomly generate the position of the gap for the pipe
    if (ind >= 0 && ind < 3) {
        gapPos[ind] = 3 + rand() % 14;
    }
}

void validatePipeGap(int ind) {
    if (gapPos[ind] < 3 || gapPos[ind] > (SCREEN_HEIGHT - GAP_SIZE - 3)) {
        gapPos[ind] = 3 + rand() % 14; // Regenerate valid gap size
    }
}

void drawPipe(int ind) {
    // Draw the pipe if it is active
    if (pipeFlag[ind] == 1) {
        validatePipeGap(ind); // Ensure pipe gap is valid
        for (int i = 0; i < gapPos[ind]; i++) {
            gotoxy(WIN_WIDTH - pipePos[ind], i + 1);
            printf("***");
        }
        for (int i = gapPos[ind] + GAP_SIZE; i < SCREEN_HEIGHT - 1; i++) {
            gotoxy(WIN_WIDTH - pipePos[ind], i + 1);
            printf("***");
        }
    }
}

void erasePipe(int ind) {
    // Erase the pipe to allow smooth animation
    if (pipeFlag[ind] == 1) {
        for (int i = 0; i < gapPos[ind]; i++) {
            gotoxy(WIN_WIDTH - pipePos[ind], i + 1);
            printf("   ");
        }
        for (int i = gapPos[ind] + GAP_SIZE; i < SCREEN_HEIGHT - 1; i++) {
            gotoxy(WIN_WIDTH - pipePos[ind], i + 1);
            printf("   ");
        }
    }
}

// Functions related to the bird
void drawBird() {
    // Draw the bird at its current position
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            gotoxy(j + 2, i + birdPos);
            printf("%c", bird[i][j]);
        }
    }
}

void eraseBird() {
    // Erase the bird to allow smooth animation
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 6; j++) {
            gotoxy(j + 2, i + birdPos);
            printf(" ");
        }
    }
}

// Collision detection
int collision() {
    // Check if the bird collides with the first pipe
    if (pipePos[0] >= 61) {
        if (birdPos < gapPos[0] || birdPos > gapPos[0] + GAP_SIZE) {
            return 1; // Collision detected
        }
    }
    return 0; // No collision
}

// Game over screen
void gameover() {
    system("cls");
    printf("\n");
    printf("\t\t--------------------------\n");
    printf("\t\t-------- Game Over -------\n");
    printf("\t\t--------------------------\n\n");
    printf("\t\tYour Final Score: %d\n", score);
    printf("\t\tPress any key to go back to menu.");
    getch();
}

// Update and display the score
void updateScore() {
    gotoxy(WIN_WIDTH + 7, 5);
    printf("Score: %d", score);
}

// Instructions screen
void instructions() {
    system("cls");
    printf("Instructions\n");
    printf("----------------\n");
    printf(" Press spacebar to make the bird fly.\n");
    printf(" Avoid hitting the pipes or the ground.\n");
    printf("\nPress any key to go back to menu.");
    getch();
}

// Bird character selection
void selectCharacter() {
    system("cls");
    printf("Select Character\n");
    printf("----------------\n");
    printf("1. Bird 1\n");
    printf("2. Bird 2\n");
    printf("Select option: ");
    char choice = getch();

    if (choice == '1') {
        memcpy(bird, bird1, sizeof(bird1));
    } else if (choice == '2') {
        memcpy(bird, bird2, sizeof(bird2));
    } else {
        printf("\nInvalid option. Default Bird selected.");
        memcpy(bird, bird1, sizeof(bird1));
    }
}

// Game logic
void play() {
    birdPos = 6;          // Reset bird position
    score = 0;            // Reset score
    pipeFlag[0] = 1;      // Activate first pipe
    pipeFlag[1] = 0;      // Deactivate second pipe initially
    pipePos[0] = pipePos[1] = 4; // Initialize pipe positions

    system("cls");
    drawBorder();         // Draw the game border
    genPipe(0);           // Generate the first pipe
    updateScore();        // Display the score

    // Display game instructions
    gotoxy(WIN_WIDTH + 5, 2);
    printf("FLAPPY BIRD");
    gotoxy(WIN_WIDTH + 6, 4);
    printf("----------");
    gotoxy(WIN_WIDTH + 2, 14);
    printf(" Spacebar = Jump");

    gotoxy(10, 5);
    printf("Press any key to start");
    getch();  // Wait for key press to start the game

    // Clear the "Press any key to start" message
    gotoxy(10, 5);
    printf("                      ");

    // Game loop
    while (1) {
        // Handle user input
        if (kbhit()) {
            char ch = getch();
            if (ch == 32 && birdPos > 3) { // Spacebar to jump
                birdPos -= 3;
            }
            if (ch == 27) { // Escape to exit
                break;
            }
        }

        drawBird();    // Draw the bird
        drawPipe(0);   // Draw the first pipe
        drawPipe(1);   // Draw the second pipe

        // Check for collisions
        if (collision() == 1) {
            gameover();
            return;
        }

        Sleep(100);    // Control game speed
        eraseBird();   // Erase the bird for smooth animation
        erasePipe(0);  // Erase the first pipe
        erasePipe(1);  // Erase the second pipe
        birdPos += 1;  // Bird falls due to gravity

        // Check for ground collision
        if (birdPos > SCREEN_HEIGHT - 2) {
            gameover();
            return;
        }

        // Move pipes
        if (pipeFlag[0] == 1) pipePos[0] += 2;
        if (pipeFlag[1] == 1) pipePos[1] += 2;

        // Activate the second pipe
        if (pipePos[0] >= 40 && pipePos[0] < 42) {
            pipeFlag[1] = 1;
            pipePos[1] = 4;
            genPipe(1);
        }

        // Reset pipes when out of bounds
        if (pipePos[0] > 68) {
            score++;
            updateScore();
            pipeFlag[1] = 0;
            pipePos[0] = pipePos[1];
            gapPos[0] = gapPos[1];
        }
    }
}

// Main menu and game loop
int main() {
    setcursor(0, 0);
    srand((unsigned)time(NULL)); // Seed random number generator
    memcpy(bird, bird1, sizeof(bird1)); // Set default bird

    do {
        system("cls");
        gotoxy(10, 5);
        printf(" -------------------------- ");
        gotoxy(10, 6);
        printf(" |      Flappy Bird       | ");
        gotoxy(10, 7);
        printf(" --------------------------");
        gotoxy(10, 9);
        printf("1. Start Game");
        gotoxy(10, 10);
        printf("2. Instructions");
        gotoxy(10, 11);
        printf("3. Quit");
        gotoxy(10, 12);
        printf("4. Select Character");
        gotoxy(10, 14);
        printf("Select option: ");
        char op = getch();

        // Handle menu options
       switch (op) {
    case '1':
        play();
        break;
    case '2':
        instructions();
        break;
    case '3':
        exit(0);
        break;
    case '4':
        selectCharacter();
        break;
    default:
        gotoxy(10, 16);
        printf("Invalid option! Please try again.");
        Sleep(1000); // Pause to show error message
        break;
}
    } while (1);

    return 0;
}
