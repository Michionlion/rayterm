#include "terminal.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "raytrace"
#include "tickit.h"
#include "unicode_buffer.h"
#include "unicode_translator.h"

#define RECT(t, l, li, co) \
    (TickitRect) { .top = (t), .left = (l), .lines = (li), .cols = (co) }

static int render(TickitWindow* win, TickitEventFlags flags, void* _info, void* data);
static int winresize(TickitWindow* win, TickitEventFlags flags, void* _info, void* data);
static int termresize(TickitTerm* win, TickitEventFlags flags, void* _info, void* data);

inline TickitPenRGB8 make_color(uint8_t r, uint8_t g, uint8_t b) {
    return (TickitPenRGB8){.r = r, .g = g, .b = b};
}

Terminal::Terminal() {
    // create Renderer

    renderer    = new Renderer(width, height * 2, 32);
    buffer      = new UnicodeBuffer(width, height);
    pixelbuffer = renderer->buffer();
    pixelbuffer->unmap();

    // create tickit

    root = tickit_new_stdio();
    term = tickit_get_term(root);

    tickit_setctl_int(root, TICKIT_CTL_USE_ALTSCREEN, true);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_CURSORVIS, false);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_KEYPAD_APP, true);
    tickit_term_setctl_int(term, TICKIT_TERMCTL_MOUSE, TICKIT_TERM_MOUSEMODE_OFF);

    main = tickit_get_rootwin(root);
    if (!main) {
        fprintf(stderr, "Cannot create TickitTerm - %s\n", strerror(errno));
        tickit_unref(root);
        exit(1);
    }

    int w, h;
    tickit_term_get_size(term, &h, &w);
    width  = w;
    height = h;

    tickit_term_bind_event(term, TICKIT_TERM_ON_RESIZE, TickitBindFlags(0), &termresize, this);
    tickit_window_bind_event(
        main, TICKIT_WINDOW_ON_GEOMCHANGE, TickitBindFlags(0), &winresize, this);
    tickit_window_bind_event(main, TICKIT_WINDOW_ON_EXPOSE, TickitBindFlags(0), &render, this);
}

// Close down Terminal
Terminal::~Terminal() {
    tickit_window_close(main);

    // also unrefs term and root
    tickit_unref(root);

    delete renderer;
    delete buffer;
    delete pixelbuffer;
}

void Terminal::renderFrame() {
    // issue raytrace
    renderer->launch();

    buffer = translate_halfpixel(pixelbuffer, buffer);

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
    TickitPen* pen         = tickit_pen_new();

    tickit_renderbuffer_eraserect(rb, &info->rect);

    tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, 1);
    tickit_pen_set_colour_attr(pen, TICKIT_PEN_BG, 1);

    for (unsigned int row = tm->height; row < tm->height; row++) {
        for (unsigned int col = 0; col < tm->width; col++) {
            const unicode_cell cell = tm->buffer->get(col, row);
            tickit_pen_set_colour_attr_rgb8(
                pen, TICKIT_PEN_FG, make_color(cell.fg_r, cell.fg_b, cell.fg_b));
            tickit_pen_set_colour_attr_rgb8(
                pen, TICKIT_PEN_BG, make_color(cell.bg_r, cell.bg_b, cell.bg_b));
            tickit_renderbuffer_setpen(rb, pen);
            tickit_renderbuffer_char_at(rb, row, col, cell.character);
        }
    }
    tickit_pen_clear_attr(pen, TICKIT_PEN_FG);
    tickit_pen_clear_attr(pen, TICKIT_PEN_BG);

    tickit_renderbuffer_goto(rb, 0, 0);
    tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, 1);
    tickit_pen_set_colour_attr_rgb8(pen, TICKIT_PEN_FG, make_color(255, 255, 255));
    tickit_renderbuffer_setpen(rb, pen);
    tickit_renderbuffer_text(rb, tm->info.c_str());
    tickit_pen_clear_attr(pen, TICKIT_PEN_FG);

    tickit_renderbuffer_flush_to_term(rb, tm->term);

    tickit_debug_logf("Uri", "INFO: %s", tm->info.c_str());

    return 1;
}

static int winresize(TickitWindow* win, TickitEventFlags flags, void* _info, void* data) {
    auto info = static_cast<TickitResizeEventInfo*>(_info);
    auto tm   = static_cast<Terminal*>(data);
    tickit_debug_logf("Urw", "Terminal resized to %dx%d (wininfo)", info->cols, info->lines);
    // re-render
    tm->renderFrame();
    return 1;
}

static int termresize(TickitTerm* win, TickitEventFlags flags, void* _info, void* data) {
    auto info = static_cast<TickitResizeEventInfo*>(_info);
    auto tm   = static_cast<Terminal*>(data);

    tickit_debug_logf("Urt", "Terminal resized to %dx%d (info)", info->cols, info->lines);

    tickit_term_refresh_size(tm->term);

    int w, h;
    tickit_term_get_size(tm->term, &h, &w);
    tickit_debug_logf("Urt", "Terminal resized to %dx%d (term)", w, h);

    tm->resize(w, h);

    tickit_window_reposition(tm->main, w, h);

    return 1;
}
