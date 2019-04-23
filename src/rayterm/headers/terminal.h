#ifndef _RAYTERM_TERMINAL_H_
#define _RAYTERM_TERMINAL_H_

#include <cstdint>
#include <string>
#include "raytrace"
#include "tickit.h"
#include "unicode_buffer.h"

using color = uint8_t;

/*
 * The TERMINAL struct holds all data representing a terminal.
 * Inside the terminal are two windows: info, and main.
 * The main window shows a ray-traced view, while the info shows the FPS and some configurable text.
 *
 * This class is a singleton (created from stdscr by default), but this is not enforced.
 */
class Terminal {
    PixelBuffer* pixelbuffer;

   public:
    TickitWindow* main;
    Tickit* root;
    TickitTerm* term;

    std::string info;
    unsigned int width, height;
    int samples;

    Renderer* renderer;
    UnicodeBuffer* buffer;

    explicit Terminal(int samples);
    ~Terminal();

    void renderFrame();
    void resize(unsigned int width, unsigned int height);

    // set_info_string(TERMINAL*, char*) sets the configurable string to be shown in the info window
    // of the terminal given.
    void set_info_string(std::string info);
};

#endif
