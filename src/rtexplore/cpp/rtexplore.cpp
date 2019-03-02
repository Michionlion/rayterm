#include <cstdio>
#include <cstring>
#include "rayterm"

int main(int argc, char* argv[]) {
    // initalize ncurses
    auto term = new Terminal();

    // do fancy hello stuff
    wborder(term->main, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove(term->main, 1, 1);
    add_str(term->main, "Hello World from the main screen", A_BOLD);
    term->set_info_string("INFO WINDOW");

    // show changes
    term->render();

    int key    = 0;
    int frames = 0;
    while ((key = getch()) != 0) {
        term->render();
        if (key == '\n') {
            printf("Got ENTER, exiting\n");
            break;
        } else if (key == KEY_RESIZE) {
            term->handle_resize();
        }
        frames++;
    }

    delete term;
}
