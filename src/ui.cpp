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

using namespace std;


// --------- //
// Functions //
// --------- //

/* Choices class
Based on the string vector passed in the constructor, each string's drawing
position is calculated and stored. The currently selected choice can be updated
using the various 'select' methods, and drawn using drawSelect. All of the
choices can also be drawn at once using drawAll.
*/
class choices {
private:
    /* Calculate Position function
    Based on the strings lengths and the total number of strings, this function
    calculates the x drawing position such that each string is centered over an
    equal division of the terminal's width. Results are stored in the pos
    member, with their index matching that of the string. */
    void calcPos() {
        for (int i = 0; i < size; i++) {
            pos.push_back(((COLS * (i + 1)) / (size + 1))
                          - (str[i].size() / 2));
        }
    }

    /* Positive Modulo function
    Source: https://stackoverflow.com/a/14997413
    Returns abs(a % b). */
    int posMod(int a, int b) { return (a % b + b) % b; }

    int size; // Number of choices
    vector<string> str; // Vector of choice strings
    vector<int> pos; // Vector of draw positions
    int oldSelection = 0; // Previously selected choice
    int selection = 0; // Currently selected choice

public:
    // Constructor
    choices(vector<string> strings) : size(strings.size()), str(strings)  {
        calcPos();
    }

    // Sets the selection equal to 'i', with wrap-around.
    void select(int i) {
        oldSelection = selection;
        selection = posMod(i, size);
    }

    // Increments the selection by 'i', with wrap-around.
    void selectUp(int i = 1) {
        oldSelection = selection;
        selection = posMod(selection + i, size);
    }

    // Decrements the selection by 'i', with wrap-around.
    void selectDown(int i = 1) {
        oldSelection = selection;
        selection = posMod(selection - i, size);
    }

    int getSelect() { return selection; }

    /* Draws All Choices function
    Draws the choices in the passed window based on the calculated pos values.
    Assumes the window is the width of the terminal.
    */
    void drawAll(WINDOW* win) {
        for (int i = 0; i < size; i++) {
            mvwprintw(win, 0, pos[i], str[i].c_str());
        }
        wrefresh(win);
    }

    /* Draws Selected Choice function
    Draws the selected choice with highlighting to the passed window. First
    redraws the previously selected choice to remove the highlighting and then
    redraws the current choice with highlighting.
    */
    void drawSelect(WINDOW* win) {
        mvwprintw(win, 0, pos[oldSelection], str[oldSelection].c_str());
        wattron(win, A_STANDOUT);
        mvwprintw(win, 0, pos[selection], str[selection].c_str());
        wattroff(win, A_STANDOUT);
        wrefresh(win);
    }
};


/* Process Menu function
Given the appropriate window and a choices object, handles all the user input
for selecting an option. Supports left and right arrow keys or 'a' and 'd' for
lateral movement, or numbers 1-9 for quick selection, with 'Enter' for
confirmation. Returns a index value corresponding to the chosen option.
*/
int processMenu(WINDOW* cWin, choices c) {
    // Set default selection
    c.select(1);
    c.drawSelect(cWin);

    // Process input
    int key = 0;
    while (key != 10) { // Exit if enter is pressed
        key = wgetch(cWin);
        switch (key) {
            case KEY_RIGHT:
            case 'd':
                c.selectUp();
                break;
            case KEY_LEFT:
            case 'a':
                c.selectDown();
                break;
            case '1' ... '9':
                c.select(key - '0' - 1);
                break;
            default:
                continue;
        }
        c.drawSelect(cWin);
    }
    return c.getSelect();
}


/* Create Prompt function
Creates and draws a centered prompt window at the top of the terminal, complete
with border. Looks best if the prompt string is the same parity as the
terminal's width (odd or even). Returns the created window for deletion later.
*/
WINDOW* createPrompt(string p) {
    // Create a window for the prompt
    WINDOW* pWin = newwin(3, p.size() + 4, 1, (COLS - p.size() - 4) / 2);

    // Draw the prompt
    wborder(pWin, '|', '|', '-', '-', '+', '+', '+', '+');
    mvwprintw(pWin, 1, 2, p.c_str());

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
    choices c(choiceStrs);
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
    WINDOW * logoWin = newwin(8, 50, 0, (COLS - 50) / 2);
    for (int i = 0; i < 5; i++) mvwprintw(logoWin, i, 0, logo[i].c_str());
    for (int i = 5; i < 8; i++) mvwprintw(logoWin, i, 5, logo[i].c_str());

    // Draw Insert Coin
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