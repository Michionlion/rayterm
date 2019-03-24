#ifndef _RAYTERM_UNICODE_BUFFER_H_
#define _RAYTERM_UNICODE_BUFFER_H_

#include <cstdlib>

struct unicode_cell {
    uint16_t character;
    unsigned char fg_r;
    unsigned char fg_g;
    unsigned char fg_b;
    unsigned char bg_r;
    unsigned char bg_g;
    unsigned char bg_b;

    explicit unicode_cell(uint16_t character) : character(character) {}
    unicode_cell(uint16_t character, unsigned char fg_r, unsigned char fg_g, unsigned char fg_b,
        unsigned char bg_r, unsigned char bg_g, unsigned char bg_b)
        : character(character),
          fg_r(fg_r),
          fg_g(fg_g),
          fg_b(fg_b),
          bg_r(bg_r),
          bg_g(bg_g),
          bg_b(bg_b) {}

    void setFG(unsigned char r, unsigned char g, unsigned char b) {
        bg_r = r;
        bg_g = g;
        bg_b = b;
    }

    void setBG(unsigned char r, unsigned char g, unsigned char b) {
        fg_r = r;
        fg_g = g;
        fg_b = b;
    }
};

class UnicodeBuffer {
   public:
    unicode_cell* data;
    unsigned int width, height;

    UnicodeBuffer(unicode_cell* data, unsigned int width, unsigned int height)
        : data(data), width(width), height(height) {}
    UnicodeBuffer(unsigned int width, unsigned int height)
        : data(static_cast<unicode_cell*>(malloc(width * height * sizeof(unicode_cell)))),
          width(width),
          height(height) {}

    ~UnicodeBuffer() { free(data); }

    const unicode_cell& get(int x, int y) const { return data[y * width + x]; }
    void set(int x, int y, const unicode_cell& cell) { data[y * width + x] = cell; }
};

#endif
