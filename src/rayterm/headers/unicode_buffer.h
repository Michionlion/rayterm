#ifndef _RAYTERM_UNICODE_BUFFER_H_
#define _RAYTERM_UNICODE_BUFFER_H_

// U+2580 (▀)
#define UPPER_HALF_PIXEL_CHAR '\u2580'

struct unicode_cell {
    unsigned char character;
    unsigned char bg_r;
    unsigned char bg_g;
    unsigned char bg_b;
    unsigned char fg_r;
    unsigned char fg_g;
    unsigned char fg_b;

    unicode_cell(unsigned char character) : character(character) {}

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

    UnicodeBuffer(unicode_cell* data, unsigned int width, unsigned int height) {
        this->data   = data;
        this->width  = width;
        this->height = height;
    }

    ~UnicodeBuffer() { free(data); }

    const unicode_cell& get(int x, int y) const { return data[y * width + x]; }
    // void set(int x, int y, unicode_cell& cell) { data[y * width + x] = cell; }
};

#endif
