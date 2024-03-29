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
#include "ui.h"

#include <ncurses.h>
#include <string>
#include <vector>
#include <ctime>

#include "uiHelper.h"

using namespace std;


// --------- //
// Functions //
// --------- //

/* Title Screen function
Draws the title screen and waits for user input (Enter) before returning.
*/
void titleScreen() {
    // Ascii art logo, 8 rows by 50 columns
    string logo[8];
    logo[0] = "   _____ __             __               __";
    logo[1] = "  / ___// /_____ ______/ /_  __  _______/ /_______";
    logo[2] = "  \\__ \\/ __/ __ `/ ___/ __ \\/ / / / ___/ // / ___/";
    logo[3] = " ___/ / /_/ /_/ / /  / /_/ / /_/ / /__/ , <(__  )";
    logo[4] = "/____/\\__/\\__,_/_/  /_.___/\\__,_/\\___/_/|_/____/";
    logo[5] = "____ _ _  _ _  _ _    ____ ___ ____ ____";
    logo[6] = "[__  | |\\/| |  | |    |__|  |  |  | |__/";
    logo[7] = "___] | |  | |__| |___ |  |  |  |__| |  \\";

    // Create window and draw the logo
    WINDOW * logoWin = newwin(8, 53, 0, (COLS - 53) / 2);
    for (int i = 0; i < 5; i++) mvwprintw(logoWin, i, 1, logo[i].c_str());
    for (int i = 5; i < 8; i++) mvwprintw(logoWin, i, 12, logo[i].c_str());

    // Draw Press Enter
    attron(A_STANDOUT);
    mvprintw(10, (COLS - 11) / 2, "Press Enter");
    attroff(A_STANDOUT);

    // Refresh
    refresh();
    wrefresh(logoWin);

    // Wait for enter
    int key = 0;
    while (key != 10) key = getch();

    // Cleanup
    delwin(logoWin);
    clear();
    refresh();
}


/* Create Prompt function
Creates and draws a centered prompt window at the top of the terminal, complete
with border. Looks best if the prompt string is the same parity as the
terminal's width (odd or even). Returns the created window for deletion later.
*/
WINDOW* createPrompt(string prompt) {
    // Create a window for the prompt
    WINDOW* pWin = newwin(3, prompt.size() + 4, 1,
                          (COLS - prompt.size() - 4) / 2);

    // Draw the prompt
    wborder(pWin, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwprintw(pWin, 1, 2, prompt.c_str());

    // Refresh
    refresh();
    wrefresh(pWin);

    return pWin;
}


/* Select Screen function
Draws the select screen and processes user input to allow user selection.
Returns an integer corresponding to the user's choice.

Args:
- prompt: The prompt string displayed at the top of the screen
- choiceStrs: A string vector of the available choices to the user. The strings
              are displayed in order and the returned integer corresponds to an
              index in this vector. Best if 1-6 strings are provided.
*/
int selectScreen(string prompt, vector<string> choiceStrs) {
    // Create and draw the prompt
    WINDOW* pWin = createPrompt(prompt);

    // Create and draw the choice window
    Choices c(choiceStrs);
    WINDOW* cWin = newwin(1, COLS, 7, 0);
    keypad(cWin, TRUE);
    c.drawAll(cWin);

    // Get user choice
    int result = processMenu(cWin, c);

    // Cleanup
    delwin(pWin);
    delwin(cWin);
    clear();
    refresh();
    return result;
}


/* Ready Screen function
Draws the timer until the round start and the current score. Returns when the
timer has depleted.

Args:
- round: The current round number
- score: The score to display
*/
void readyScreen(int round, int score) {
    // Create and draw the timer prompt window
    WINDOW* tpWin = newwin(1, 17, 3, (COLS - 21) / 2);
    string timerPrompt = "Round " + to_string(round) + " starts in";
    mvwprintw(tpWin, 0, 0, timerPrompt.c_str());
    wrefresh(tpWin);

    // Create and draw the timer window
    Timer t(5, time(NULL));
    WINDOW* tWin = newwin(1, 3, 3, (COLS - 21) / 2 + 18);
    t.draw(tWin);

    // Round 1 starts in 005

    // Create and draw the score window
    string scoreStr = to_string(score);
    WINDOW* sWin = newwin(1, 15 + scoreStr.size(), 6,
                          (COLS - (15 + scoreStr.size())) / 2);
    mvwprintw(sWin, 0, 0, ("Current Score: " + scoreStr).c_str());
    wrefresh(sWin);

    // Update timer until end
    while (t.verify()) { t.update(tWin); }

    // Cleanup
    clear();
    delwin(tpWin);
    delwin(tWin);
    delwin(sWin);
}


/* Gameplay Screen function
Draws the timer, score, message and input windows, processes the user input, and
updates the timer accordingly. Returns the string typed by the user when enter
is pressed, or the partial string within the buffer if the timer runs out.

Args:
- t: The timer object to be updated
- score: The score to display
- message: The message to display
*/
string gameplayScreen(Timer& t, int score, const string& message) {
    curs_set(1);

    // Create and draw the time and score window
    WINDOW* tsWin = newwin(1, 9, 5, (COLS - 9) / 2);
    mvwprintw(tsWin, 0, 0, "000 | 000");
    t.draw(tsWin);

    string scoreStr = to_string(score);
    scoreStr.insert(scoreStr.begin(), 3 - scoreStr.size(), '0');
    mvwprintw(tsWin, 0, 6, scoreStr.c_str());
    wrefresh(tsWin);

    // Create and draw the message window
    WINDOW* mWin = newwin(1, message.size(), 9, (COLS - message.size()) / 2);
    mvwprintw(mWin, 0, 0, message.c_str());
    wrefresh(mWin);

    // Create and draw the input window
    WINDOW* iWin = newwin(1, 50, 7, (COLS - 50) / 2);
    keypad(iWin, TRUE);
    wtimeout(iWin, 500);
    wrefresh(iWin);

    // Get input while updating the timer
    Buffer b(50);
    while (true) {
        t.update(tsWin); // Update the timer
        if (!t.verify()) break; // Break if out of time
        b.redrawCursor(iWin);
        if(processKeyboard(iWin, b)) break; // Break if Enter was pressed
    }

    // Cleanup
    delwin(tsWin);
    werase(mWin);
    wrefresh(mWin);
    delwin(mWin);
    delwin(iWin);
    curs_set(0);
    return b.getBuffer();
}


/* Results Screen function
Draws the game score and asks the player if they would like to play again.
Returns true if "Play Again" was chosen, otherwise false.

Args:
- score: The score to be displayed
*/
bool resultsScreen(int& score) {
    // Draw score window
    WINDOW* sWin = createPrompt("Your score was: " + to_string(score));

    // Create and draw the choice window
    Choices c(vector<string> {"Play Again", "Quit"});
    WINDOW* cWin = newwin(1, COLS, 7, 0);
    keypad(cWin, TRUE);
    c.drawAll(cWin);

    // Get user choice
    int result = processMenu(cWin, c);

    // Cleanup
    delwin(sWin);
    delwin(cWin);
    clear();
    refresh();
    if (result == 0) { return true; }
    return false;
}
