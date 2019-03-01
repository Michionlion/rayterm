#include "terminal.h"
#include <stdio.h>
#include <stdlib.h>

TERMINAL* setup() {
    initscr();
    start_color();
    cbreak();
    noecho();

    curs_set(0);


    init_pair(1,COLOR_WHITE, COLOR_BLUE);
    init_pair(2,COLOR_WHITE, COLOR_GREEN);

    TERMINAL* term = (TERMINAL*) malloc(sizeof(TERMINAL));
    term->main = subwin(stdscr, LINES-1, 0, 0, 0);
    term->info = subwin(stdscr, 1, 0, LINES-1, 0);
    wbkgd(term->main, COLOR_PAIR(2));
    wbkgd(term->info, COLOR_PAIR(1));

    keypad(term->main, true);
    keypad(term->info, true);

    return term;
}

void repaint(TERMINAL* term) {

}

void handle_resize(TERMINAL* term) {
    /*wresize(stdscr, LINES, COLS);
    wclear(stdscr);
    wresize(WINDOWS.info, 1, COLS);
    wresize(WINDOWS.main, LINES - 1, COLS);
    mvwin(WINDOWS.info, LINES - 1, 0);
    mvwin(WINDOWS.main, 0, 0);

    // wbkgd(WINDOWS.main, COLOR_PAIR(2));
    // wbkgd(WINDOWS.info, COLOR_PAIR(1));
    wclear(WINDOWS.main);
    wclear(WINDOWS.info);
    repaint();
    */
}


VECTOR* get_size(WINDOW* win) {
    VECTOR* sizes = (VECTOR*) malloc(sizeof(VECTOR));
    getmaxyx(win, sizes->y, sizes->x);

    return sizes;
}

void add_str(WINDOW* window, const char* str, chtype attr) {
    for(int i = 0; str[i] != '\0'; i++) {
        waddch(window, str[i] | attr);
    }
}

// auto-refreshes info window
void set_info_string(TERMINAL* term, const char* str) {
    wclear(term->info);
    wmove(term->info, 0, 0);
    add_str(term->info, str, A_BOLD);
    wrefresh(term->info);
}

// must call refresh after
void set_character_pixel(TERMINAL* term, POINT* pos, const char ch, const Color background, const Color foreground) {
    wmove(term->main, pos->x, pos->y);
    chtype pixel = ch;
    // TODO: add background and foreground to pixel
    waddch(term->main, pixel);
}

// Close down Terminal
void cleanup(TERMINAL* term) {
    delwin(term->main);
    delwin(term->info);
    endwin();
    free(term);
    printf("\n");
}
