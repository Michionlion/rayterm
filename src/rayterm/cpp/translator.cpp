#include "translator.h"
#include <stdlib.h>
#include "terminal.h"

POINT* window_to_pixel(float x, float y, VECTOR* sizes) {
    POINT* n = (POINT*)malloc(sizeof(POINT));
    n->x     = x * sizes->x;
    n->y     = y * sizes->y;

    return n;
}

void render(TERMINAL* term) {
    VECTOR* sizes = get_size(term->main);
    for (float x = 0; x < 1; x += 1.0 / sizes->x) {
        for (float y = 0; y < 1; y += 0.5 / sizes->y) {
            POINT* pos = window_to_pixel(x, y, sizes);
            // fprintf(stderr, "x:%d, y:%d\n", pos.x, pos.y);
        }
    }
}
