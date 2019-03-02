#include "terminal.h"
#include <cstdio>
#include <cstdlib>

Terminal::Terminal() {
    initscr();
    start_color();
    cbreak();
    noecho();

    curs_set(0);

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);

    main = subwin(stdscr, LINES - 1, 0, 0, 0);
    info = subwin(stdscr, 1, 0, LINES - 1, 0);
    wbkgd(main, COLOR_PAIR(2));
    wbkgd(info, COLOR_PAIR(1));

    keypad(main, true);
    keypad(info, true);
}

// Close down Terminal
Terminal::~Terminal() {
    delwin(main);
    delwin(info);
    endwin();
}

void Terminal::render() {
    // fill buffer with raytrace
    // convert buffer to unicode

    // refresh terminal
    wrefresh(main);
    // wrefresh(info);
}

void Terminal::handle_resize() {
    getmaxyx(stdscr, width, height);
    printf("Resized to %dx%d (%d COLS, %d LINES)", width, height, COLS, LINES);
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

// auto-refreshes info window
void Terminal::set_info_string(const char* str) {
    wclear(info);
    wmove(info, 0, 0);
    add_str(info, str, A_BOLD);
    wrefresh(info);
}

void add_str(WINDOW* window, const char* str, chtype attr) {
    for (int i = 0; str[i] != '\0'; i++) {
        waddch(window, str[i] | attr);
    }
}
