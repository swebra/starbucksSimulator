/*
Author: Eric Claerhout, Henry Lin
Student ID: 1532360, 1580649
CMPUT 275 Winter 2019

Final Project: Starbucks Simulator
##################################
*/

// ------------------------ //
// Includes and Definitions //
// ------------------------ //
#include <iostream>
#include <ncurses.h>
#include <ctime>
#include <unistd.h> // For sleep

#include "ui.h"
#include "uiHelper.h" // For Timer class
#include "cpu.h"

using namespace std;


// --------- //
// Functions //
// --------- //

void round(string prompt, Cpu& game, int& score) {
    // Initialize variables
    Timer t(30, time(NULL));
    int roundScore = 0;

    // Create and draw the prompt
    WINDOW* pWin = createPrompt(prompt);

    // Draw the rest of the gameplay screen and get the user input
    while (t.verify()) {
        string input = gameplayScreen(t, roundScore + score);
        if (input.empty()) { continue; }

        // Verify equal phonetic index
        game.processInput(input);

        // Calculate awarded points
        int inputScore = game.getScore();
        if (inputScore > roundScore) { roundScore = inputScore; }
    }

    // Update game score with the final round score
    score += roundScore;

    // Cleanup
    delwin(pWin);
    clear();
    refresh();
}


// Main function
int main() {
    cout << "Starbucks Simulator Starting..." << endl;
    system("resize -s 12 55"); // Ghetto, not a good way to do this
    sleep(1);

    // Ncurses initialization
    initscr();
    cbreak();
    noecho();

    curs_set(0);
    titleScreen();

    int character = selectScreen("Choose your fighter",
                                 {"Names 0", "Names 1", "Names 2"});
    int opponent = selectScreen("Choose your rival",
                                {"Character 0", "Character 1", "Character 2"});
    curs_set(1);

    Cpu game(character, opponent);
    int score = 0;
    round("Round 1: " + game.displayName(), game, score);
    game.nextName();
    round("Round 2: " + game.displayName(), game, score);
    game.nextName();
    round("Round 3: " + game.displayName(), game, score);

    curs_set(0);
    resultsScreen(score);

    // Cleanup
    endwin();
    return 0;
}
