#ifndef _RAYTERM_TERMINAL_H_
#define _RAYTERM_TERMINAL_H_

#include <ncurses.h>
#include <cstdint>

using color = uint8_t;

/*
 * The TERMINAL struct holds all data representing a terminal.
 * Inside the terminal are two windows: info, and main.
 * The main window shows a ray-traced view, while the info shows the FPS and some configurable text.
 *
 * This class is a singleton (created from stdscr by default), but this is not enforced.
 */
class Terminal {
   public:
    WINDOW* info;
    WINDOW* main;

    int width, height;

    Terminal();
    ~Terminal();

    void render();

    void handle_resize();

    // set_info_string(TERMINAL*, char*) sets the configurable string to be shown in the info window
    // of the terminal given.
    void set_info_string(const char* str);
};

// add_str(WINDOW*, char*, chtype) draws the given string to the window given with the given
// attributes.
void add_str(WINDOW* window, const char* str, chtype attr);

#endif
