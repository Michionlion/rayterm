#include "stdio.h"
#include "string.h"
#include "terminal.h"
#include "translator.h"

int main (int argc, char *argv[]) {

    // initalize ncurses
    TERMINAL* term = setup();


    // do fancy hello stuff
    wborder(term->main, 0, 0, 0, 0, 0, 0, 0, 0);
    wmove(term->main, 1, 1);
    add_str(term->main, "Hello World from the main screen", A_BOLD);
    set_info_string(term, "INFO WINDOW");

    // show changes
    repaint(term);

    int key = 0;
    int frames = 0;
    while((key = getch()) != 0) {
        render(term);
        repaint(term);
        if(key == '\n') {
            printf("Got ENTER, exiting\n");
            break;
        } else if (key == KEY_RESIZE) {
            handle_resize(term);
        }
        frames++;
    }

    // do cleanup/exit
    cleanup(term);
}
