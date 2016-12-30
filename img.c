#include <stdio.h>
#include <runt.h>
#include "lodepng.h"

#define WIDTH 256
#define HEIGHT 256

static unsigned char data[WIDTH * HEIGHT * 4];

static unsigned char current_color[4];

void set_color_rgba(
        unsigned char r, 
        unsigned char g, 
        unsigned char b, 
        unsigned char a) 
{
    current_color[0] = r;
    current_color[1] = g;
    current_color[2] = b;
    current_color[3] = a;
}

void set_color_rgb(
        unsigned char r, 
        unsigned char g, 
        unsigned char b) 
{
    set_color_rgba(r, g, b, 255);
}

void img_fill()
{
    int x, y;
    int pos;
    int i;
    for(y = 0; y < HEIGHT; y++)  {
        for(x = 0; x < WIDTH; x++) {
            pos = y * WIDTH * 4 + x * 4;
            data[pos] = current_color[0];
            data[pos + 1] = current_color[1];
            data[pos + 2] = current_color[2];
            data[pos + 3] = current_color[3];
        }
    }
}

void img_point(unsigned int x, unsigned int y)
{
    unsigned int pos = 4 * y * WIDTH + x * 4;
    data[pos] = current_color[0];
    data[pos + 1] = current_color[1];
    data[pos + 2] = current_color[2];
    data[pos + 3] = current_color[3];
}

static void img_rect(unsigned int x_pos, 
    unsigned int y_pos,
    unsigned int width,
    unsigned int height) {

    unsigned int x;
    unsigned int y;

    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            img_point(x + x_pos, y + y_pos);
        }
    }

}

void img_write(const char *filename)
{
    lodepng_encode32_file(filename, data, WIDTH, HEIGHT);
}

static void img_row(unsigned char x, unsigned char y, unsigned char col, unsigned char s)
{
    unsigned int c;
    unsigned int x_pos = x * 8; 
    unsigned int y_pos = y * 8 + col;
    for(c = 0; c < 8; c++) {
        if(s & (1 << c)) {
            img_point(x_pos + (7 - c), y_pos);
        }
    }
}

static void img_col(unsigned char x, unsigned char y, unsigned char row, unsigned char s)
{
    unsigned int c;
    unsigned int x_pos = x * 8 + row; 
    unsigned int y_pos = y * 8;
    for(c = 0; c < 8; c++) {
        if(s & (1 << c)) {
            img_point(x_pos, y_pos + (7 - c));
        }
    }
}

static runt_int rproc_set_color_rgb(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    unsigned char r, g, b;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    b = s->f;
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    g = s->f;
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    r = s->f;

    set_color_rgb(r, g, b);
    return RUNT_OK;
}

static runt_int rproc_fill(runt_vm *vm, runt_ptr p)
{
    img_fill();
    return RUNT_OK;
}

static runt_int rproc_point(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    runt_uint x, y;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;
    img_point(x, y);
    return RUNT_OK;
}

static runt_int rproc_write(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    const char *filename;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    filename = runt_to_string(s->p);

    img_write(filename);

    return RUNT_OK;
}

static runt_int rproc_rect(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s; 
    runt_int rc;
    runt_uint x, y, w, h;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    h = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    w = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;

    img_rect(x, y, w, h);

    return RUNT_OK;
}

static runt_int rproc_col(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s; 
    runt_int rc;
    unsigned char x, y, row, state;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    state = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    row = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;

    img_col(x, y, row, state);
    return RUNT_OK;
}

static runt_int rproc_row(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s; 
    runt_int rc;
    unsigned char x, y, row, state;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    state = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    row = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;

    img_row(x, y, row, state);
    return RUNT_OK;
}

static runt_int rproc_bin(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    runt_int i;
    runt_uint val = 0;
    for(i = 0; i < 8; i++) {
        rc = runt_ppop(vm, &s);
        RUNT_ERROR_CHECK(rc);
        if(s->f != 0) {
            val += 1 << i;
        }
    }
    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->f = val;
    return RUNT_OK;
}

void runt_plugin_init(runt_vm *vm)
{
    runt_word_define(vm, "img_color", 9, rproc_set_color_rgb);
    runt_word_define(vm, "img_fill", 8, rproc_fill);
    runt_word_define(vm, "img_write", 9, rproc_write);
    runt_word_define(vm, "img_point", 9, rproc_point);
    runt_word_define(vm, "img_rect", 8, rproc_rect);
    runt_word_define(vm, "img_col", 7, rproc_col);
    runt_word_define(vm, "img_row", 7, rproc_row);
    runt_word_define(vm, "img_bin", 7, rproc_bin);
}
