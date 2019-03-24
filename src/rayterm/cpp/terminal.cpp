#include "terminal.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "tickit.h"

#define COLOR_ERROR 1
#define RECT(t, l, li, co) \
    (TickitRect) { .top = (t), .left = (l), .lines = (li), .cols = (co) }

static int render(TickitWindow* win, TickitEventFlags flags, void* _info, void* data);
static int resize(TickitWindow* win, TickitEventFlags flags, void* _info, void* data);

inline TickitPenRGB8 make_color(uint8_t r, uint8_t g, uint8_t b) {
    return (TickitPenRGB8){.r = r, .g = g, .b = b};
}

Terminal::Terminal() {
    root = tickit_new_stdio();
    term = tickit_get_term(root);

    tickit_setctl_int(root, TICKIT_CTL_USE_ALTSCREEN, true);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_CURSORVIS, false);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_KEYPAD_APP, true);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_MOUSE, TICKIT_TERM_MOUSEMODE_MOVE);

    main = tickit_get_rootwin(root);
    if (!main) {
        fprintf(stderr, "Cannot create TickitTerm - %s\n", strerror(errno));
        free(root);
        exit(1);
    }

    tickit_term_get_size(term, &height, &width);

    tickit_window_bind_event(main, TICKIT_WINDOW_ON_GEOMCHANGE, TICKIT_BIND_UNBIND, resize, this);
    tickit_window_bind_event(main, TICKIT_WINDOW_ON_EXPOSE, TICKIT_BIND_UNBIND, render, this);

    //
    // initscr();
    // start_color();
    // cbreak();
    // noecho();
    //
    // curs_set(0);
    //
    // init_pair(1, COLOR_WHITE, COLOR_BLUE);
    // init_pair(2, COLOR_WHITE, COLOR_GREEN);
    //
    // main = subwin(stdscr, LINES - 1, 0, 0, 0);
    // info = subwin(stdscr, 1, 0, LINES - 1, 0);
    // wbkgd(main, COLOR_PAIR(2));
    // wbkgd(info, COLOR_PAIR(1));
    //
    // keypad(main, true);
    // keypad(info, true);
}

// Close down Terminal
Terminal::~Terminal() {
    tickit_window_close(main);

    tickit_term_unref(term);
    tickit_unref(root);

    delete renderer;
    delete buffer;
    delete pixelbuffer;
}

void Terminal::renderFrame() {
    // issue raytrace

    // convert buffer

    // expose
    tickit_window_expose(this->main, nullptr);
}

void Terminal::resize(unsigned int width, unsigned int height) {
    this->width  = width;
    this->height = height;
    delete pixelbuffer;
    renderer->resize(width, height * 2);
    pixelbuffer = renderer->buffer();
}

void Terminal::set_info_string(std::string info) {
    // move to ensure performance if possible
    this->info = std::move(info);
}

static int render(TickitWindow* win, TickitEventFlags flags, void* _info, void* data) {
    auto tm                = static_cast<Terminal*>(data);
    auto info              = static_cast<TickitExposeEventInfo*>(_info);
    TickitRenderBuffer* rb = info->rb;

    // int cols       = tickit_window_cols(win);
    TickitPen* pen = tickit_pen_new();

    tickit_renderbuffer_eraserect(rb, &info->rect);

    // tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, COLOR_ERROR);
    // tickit_renderbuffer_setpen(rb, pen);
    // tickit_renderbuffer_text_at(rb, 1, 0, "Color:");
    //
    // tickit_renderbuffer_goto(rb, 2, 0);
    // for (int x = 0; x < cols; x++) {
    //     tickit_pen_set_colour_attr(pen, TICKIT_PEN_BG, x > cols / 2 ? COLOR_ERROR : 0);
    //     tickit_pen_set_colour_attr_rgb8(pen, TICKIT_PEN_BG, make_color(255 * x / (cols - 1), 0,
    //     0));
    //
    //     tickit_renderbuffer_setpen(rb, pen);
    //     tickit_renderbuffer_text(rb, " ");
    // }
    // tickit_pen_clear_attr(pen, TICKIT_PEN_BG);

    tickit_renderbuffer_goto(rb, 0, 0);
    tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, COLOR_ERROR);
    tickit_pen_set_colour_attr_rgb8(pen, TICKIT_PEN_FG, make_color(255, 255, 255));
    tickit_renderbuffer_setpen(rb, pen);
    tickit_renderbuffer_text(rb, tm->info.c_str());
    tickit_pen_clear_attr(pen, TICKIT_PEN_FG);

    tickit_renderbuffer_flush_to_term(rb, tm->term);

    tickit_debug_logf("Ue", "Rendered '%s'", tm->info.c_str());

    return 1;
}

static int resize(TickitWindow* win, TickitEventFlags flags, void* _info, void* data) {
    auto tm = static_cast<Terminal*>(data);
    tickit_term_refresh_size(tm->term);
    tickit_term_get_size(tm->term, &(tm->height), &(tm->width));
    // re-expose the entire window if it changes shape
    tickit_window_expose(win, nullptr);
    return 1;
}

// auto-refreshes info window
// void Terminal::set_info_string(const char* str) {
//     wclear(info);
//     attron(A_BOLD);
//     mvwaddstr(info, 0, 0, str);
//     attroff(A_BOLD);
//     wrefresh(info);
// }
