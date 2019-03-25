#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include "rayterm"
#include "tickit.h"

struct tick_loop_vars {
    Terminal *tm;
    int frame;
    std::string last_key;
    std::string last_mouse;
    bool complete;
};

static int on_key(TickitTerm *term, TickitEventFlags flags, void *_info, void *user) {
    auto info = static_cast<TickitKeyEventInfo *>(_info);
    auto tlv  = static_cast<tick_loop_vars *>(user);

    switch (info->type) {
        case TICKIT_KEYEV_TEXT:
            tlv->last_key = "text ";
            break;
        case TICKIT_KEYEV_KEY:
            tlv->last_key = "key ";
            break;
        default:
            return false;
    }
    tlv->last_key += info->str;

    tickit_debug_logf("Uik", "Key Event: %s", tlv->last_key.c_str());

    if (info->str[0] == 'q') {
        tickit_debug_logf("Ui", "Got EXIT!");
        tickit_stop(tlv->tm->root);
    }

    return true;
}

static int on_mouse(TickitTerm *term, TickitEventFlags flags, void *_info, void *user) {
    auto info = static_cast<TickitMouseEventInfo *>(_info);
    auto tlv  = static_cast<tick_loop_vars *>(user);

    switch (info->type) {
        case TICKIT_MOUSEEV_PRESS:
            tlv->last_mouse = "press ";
            break;
        case TICKIT_MOUSEEV_DRAG:
            tlv->last_mouse = "drag ";
            break;
        case TICKIT_MOUSEEV_RELEASE:
            tlv->last_mouse = "release ";
            break;
        case TICKIT_MOUSEEV_WHEEL:
            tlv->last_mouse = "wheel ";
            break;
        default:
            return false;
    }

    if (info->type == TICKIT_MOUSEEV_WHEEL) {
        tlv->last_mouse += (info->button == TICKIT_MOUSEWHEEL_DOWN) ? "down" : "up";
    } else {
        tlv->last_mouse += "button ";
        tlv->last_mouse += std::to_string(info->button);
    }

    tlv->last_mouse += " at ";
    tlv->last_mouse += std::to_string(info->line);
    tlv->last_mouse += ", ";
    tlv->last_mouse += std::to_string(info->col);

    tickit_debug_logf("Uim", "Mouse Event: %s", tlv->last_mouse.c_str());

    return true;
}

int main(int argc, char *argv[]) {
    tickit_debug_init();
    Terminal *tm;
    try {
        // initalize libtickit
        tm            = new Terminal();
        auto tlv      = new tick_loop_vars();
        tlv->tm       = tm;
        tlv->complete = false;

        tickit_term_bind_event(tm->term, TICKIT_TERM_ON_KEY, TICKIT_BIND_FIRST, on_key, tlv);
        tickit_term_bind_event(tm->term, TICKIT_TERM_ON_MOUSE, TICKIT_BIND_FIRST, on_mouse, tlv);

        while (!tlv->complete) {
            tickit_tick(tlv->tm->root, TICKIT_RUN_ONCE);
            std::stringstream info_str;
            info_str << "Frame: " << tlv->frame << "  Lines: " << tlv->tm->height;
            info_str << "  Columns: " << tlv->tm->width << "  ";
            info_str << "Key: " << tlv->last_key << "  Mouse: " << tlv->last_mouse;
            tlv->tm->set_info_string(info_str.str());
            tlv->tm->renderFrame();
            // usleep(33333);
            // usleep(6060);
            tlv->frame++;
        }

        tickit_stop(tlv->tm->root);
    } catch (const std::exception &ex) {
        tickit_debug_logf("Ue", "Fatal error: %s\n", ex.what());
        delete tm;
        throw std::runtime_error(ex.what());
    }
    delete tm;

    std::cout << "PAUSED -- press key to exit";
    getchar();
    return 0;
}
