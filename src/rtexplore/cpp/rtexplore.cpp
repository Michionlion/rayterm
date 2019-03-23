#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "rayterm"
#include "tickit.h"

std::string last_key;
std::string last_mouse;

static int on_key(TickitTerm *term, TickitEventFlags flags, void *_info, void *user) {
    auto info = static_cast<TickitKeyEventInfo *>(_info);
    // auto tm   = static_cast<Terminal *>(user);

    switch (info->type) {
        case TICKIT_KEYEV_TEXT:
            last_key = "text ";
            break;
        case TICKIT_KEYEV_KEY:
            last_key = "key ";
            break;
        default:
            return 0;
    }
    last_key += info->str;

    return 1;
}

static int on_mouse(TickitTerm *term, TickitEventFlags flags, void *_info, void *user) {
    auto info = static_cast<TickitMouseEventInfo *>(_info);
    // auto tm   = static_cast<Terminal *>(user);

    switch (info->type) {
        case TICKIT_MOUSEEV_PRESS:
            last_mouse = "press ";
            break;
        case TICKIT_MOUSEEV_DRAG:
            last_mouse = "drag ";
            break;
        case TICKIT_MOUSEEV_RELEASE:
            last_mouse = "release ";
            break;
        case TICKIT_MOUSEEV_WHEEL:
            last_mouse = "wheel ";
            break;
        default:
            return 0;
    }

    if (info->type == TICKIT_MOUSEEV_WHEEL) {
        last_mouse += (info->button == TICKIT_MOUSEWHEEL_DOWN) ? "down" : "up";
    } else {
        last_mouse += "button ";
        last_mouse += std::to_string(info->button);
    }

    last_mouse += " at ";
    last_mouse += std::to_string(info->line);
    last_mouse += ", ";
    last_mouse += std::to_string(info->col);

    return 1;
}

int main(int argc, char *argv[]) {
    tickit_debug_init();

    // initalize libtickit
    auto term = new Terminal();

    tickit_term_bind_event(term->term, TICKIT_TERM_ON_KEY, TICKIT_BIND_UNBIND, on_key, term);
    tickit_term_bind_event(term->term, TICKIT_TERM_ON_MOUSE, TICKIT_BIND_UNBIND, on_mouse, term);

    int frame = 0;
    while (last_key.compare("text q") != 0) {
        tickit_debug_logf("Ut", "frame %d", frame);
        // handle timers and IO that has come up
        tickit_tick(term->root, TICKIT_RUN_NOHANG);
        std::stringstream info;
        info << "Frame: " << frame << "  Lines: " << term->height;
        info << "  Columns: " << term->width << "  ";
        info << "Key: " << last_key << "  Mouse: " << last_mouse;
        term->set_info_string(info.str());
        term->renderFrame();
        usleep(66666);
        frame++;
    }

    delete term;
}
