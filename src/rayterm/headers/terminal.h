#ifndef _RAYTERM_TERMINAL_H_
#define _RAYTERM_TERMINAL_H_

#include <ncurses.h>
#include <stdint.h>

typedef uint8_t Color;

/*
 * The TERMINAL struct holds all data representing a terminal.
 * Inside the terminal are two windows: info, and main.
 * The main window shows a ray-traced view, while the info shows the FPS and some configurable text.
 */
typedef struct {
    WINDOW* info;
    WINDOW* main;
} TERMINAL;

/*
 * Vec2 is a position or direction in 2D space.
 *
 */
typedef struct {
    int x;
    int y;
} POINT, VECTOR;

// setup() returns a TERMINAL* pointing to a newly constructed terminal.
TERMINAL* setup(void);

// refresh(TERMINAL*) redraws the entire terminal given.
void repaint(TERMINAL* terminal);

// cleanup(TERMINAL*) frees any resources utilized by the terminal given.
void cleanup(TERMINAL* terminal);

// handle_resize(TERMINAL*) resets the terminal sizes to handle a resize.
void handle_resize(TERMINAL* terminal);

// get_size(WINDOW*) returns the size of the window given.
VECTOR* get_size(WINDOW* window);

// add_str(WINDOW*, char*, chtype) draws the given string to the window given with the given attributes.
void add_str(WINDOW* window, const char* str, chtype attr);

// set_info_string(TERMINAL*, char*) sets the configurable string to be shown in the info window of the terminal given.
void set_info_string(TERMINAL* terminal, const char* str);

// set_character_pixel(TERMINAL*, POINT*, char, color, color) sets the character pixel at the given point to the given char with the given background and foreground colors.
void set_character_pixel(TERMINAL* terminal, POINT* position, const char ch, const Color background, const Color foreground);

// set_pixel(TERMINAL*, POINT*, Color) sets the half-character pixel at the logical position given to the color given.
void set_pixel(TERMINAL* terminal, POINT* position, const Color col);

// ray-tracing functions
//
// - set scene
// - change settings?
// - change ray-tracer algorithm?

#endif
