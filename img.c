#include <stdio.h>
#include <stdlib.h>
#include <runt.h>
#ifdef RUNT_IMG_GL
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <ctype.h>
#include "lodepng.h"
#include "img.h"

#define WIDTH 256
#define HEIGHT 256

#ifndef min
#define min(A, B) ((A < B) ? A : B)
#endif

#ifndef max
#define max(A, B) ((A > B) ? A : B)
#endif

#ifndef clamp
#define clamp(X, A, B) (max(A, min(B, X)))
#endif

typedef struct {
    unsigned int width, height;
    unsigned char *data;
} img_image;

static unsigned char data[WIDTH * HEIGHT * 4];

static unsigned char current_color[4];

struct {
    int width;
    int height;
    void (*point)(unsigned int, unsigned int);
} G;

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

unsigned char *img_get_current_color()
{
    return current_color;
}

void img_set_point_function(void (*f)(unsigned int, unsigned int))
{
    G.point = f;
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
    for(y = 0; y < G.height ; y++)  {
        for(x = 0; x < G.width; x++) {
            pos = y * G.width * 4 + x * 4;
            data[pos] = current_color[0];
            data[pos + 1] = current_color[1];
            data[pos + 2] = current_color[2];
            data[pos + 3] = current_color[3];
        }
    }
}

void img_point(unsigned int x, unsigned int y)
{
    unsigned int pos; 
    x = clamp(x, 0, G.width);
    y = clamp(y, 0, G.height);
    pos = 4 * y * G.width + x * 4;
    data[pos] = current_color[0];
    data[pos + 1] = current_color[1];
    data[pos + 2] = current_color[2];
    data[pos + 3] = current_color[3];
}

void img_rect(unsigned int x_pos, 
    unsigned int y_pos,
    unsigned int width,
    unsigned int height) {

    unsigned int x;
    unsigned int y;

    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            G.point(x + x_pos, y + y_pos);
        }
    }

}

void img_write(const char *filename)
{
    lodepng_encode32_file(filename, data, G.width, G.height);
}

void img_row(unsigned char x, unsigned char y, unsigned char col, unsigned char s)
{
    unsigned int c;
    unsigned int x_pos = x * 8; 
    unsigned int y_pos = y * 8 + col;
    for(c = 0; c < 8; c++) {
        if(s & (1 << c)) {
            G.point(x_pos + (7 - c), y_pos);
        }
    }
}

void img_col(unsigned char x, unsigned char y, unsigned char row, unsigned char s)
{
    unsigned int c;
    unsigned int x_pos = x * 8 + row; 
    unsigned int y_pos = y * 8;
    for(c = 0; c < 8; c++) {
        if(s & (1 << c)) {
            G.point(x_pos, y_pos + (7 - c));
        }
    }
}

static void swap(int *a, int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Bresenham line drawing algorith
 * adopted from the tinyrenderer tutorial by 
 * Dimitry V. Sokolov 
 */

void img_line(int x0, int y0, int x1, int y1) 
{
    int x, y;
    int dx, dy;
    int derror2;
    int error2;
    char steep = 0;

    if(abs(x0 - x1) < abs(y0 - y1)) {
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = 1;
    } 

    if(x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    } 

    dx = x1 - x0;
    dy = y1 - y0;
    derror2 = abs(dy) * 2;
    error2 = 0;
    y = y0;

    for(x = x0; x < x1; x++) {
        if(steep) {
            G.point(y, x);
        } else {
            G.point(x, y);
        }
        error2 += derror2;
        if(error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}


/* midpoint circle algorithm */
void img_circ(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while(x >= y) {

        img_line(x0 - x, y0 + y, x0 + x, y0 + y);
        img_line(x0 - y, y0 + x, x0 + y, y0 + x);
        img_line(x0 + x, y0 - y, x0 - x, y0 - y);
        img_line(x0 + y, y0 - x, x0 - y, y0 - x);
        
        if(err <= 0) {
            y += 1;
            err += 2 * y + 1;
        } 

        if( err > 0 ) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

/* midpoint circle algorithm */
void img_ocirc(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while(x >= y) {

        G.point(x0 - x, y0 + y);
        G.point(x0 + x, y0 + y);
        G.point(x0 - y, y0 + x); 
        G.point(x0 + y, y0 + x);
        G.point(x0 + x, y0 - y); 
        G.point(x0 - x, y0 - y);
        G.point(x0 + y, y0 - x); 
        G.point(x0 - y, y0 - x);
        
        if(err <= 0) {
            y += 1;
            err += 2 * y + 1;
        } 

        if( err > 0 ) {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

int img_load(img_image *img, const char *filename) 
{
    int error; 
    error = lodepng_decode32_file(
           &img->data, 
           &img->width,
           &img->height,
           filename);
    return error;
}

void img_close(img_image *img) {
    free(img->data);
}

int img_copy(img_image *img, 
        unsigned int pos_x, unsigned int pos_y,
        unsigned int ix, unsigned int iy,
        unsigned int w, unsigned int h)
{
    unsigned int x;
    unsigned int y;
    unsigned int pos;
    if((ix + w) > img->width || (iy + h) > img->height) return 1;
 
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            pos = 4 * (iy + y) * img->width + (ix + x) * 4;
            set_color_rgba(img->data[pos], 
                img->data[pos + 1], 
                img->data[pos + 2], 
                img->data[pos + 3]);
            G.point(pos_x + x, pos_y + y);
        }
    }
    return 0;
}

int img_glyph(img_image *img, 
        unsigned int pos_x, unsigned int pos_y,
        unsigned int ix, unsigned int iy,
        unsigned int w, unsigned int h)
{
    unsigned int x;
    unsigned int y;
    unsigned int pos;
    if((ix + w) > img->width || (iy + h) > img->height) {
        fprintf(stderr, "Error: %dx%d\n", img->width, img->height);
        return 1;
    }
 
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            pos = 4 * (iy + y) * img->width + (ix + x) * 4;
            if(img->data[pos] != 255) {
                G.point(pos_x + x, pos_y + y);
            }
        }
    }
    return 0;
}

void img_get(int x, int y, unsigned char **col)
{
    int pos;
    pos = y * G.width * 4 + x * 4;
    *col = &data[pos];
}

/* TODO: implement scale */

void img_text(img_image *img, 
        unsigned int x, unsigned int y, 
        unsigned int w, unsigned int h, 
        unsigned int scale,
        const char *str)
{
    unsigned int i;
    unsigned int len;
    unsigned char pos;
    unsigned int pos_x;
    unsigned int pos_y;
    unsigned int iw;
    unsigned int ix;
    unsigned int iy;

    len = strlen(str);

    pos_x = x;
    pos_y = y;

    iw = img->width / w;

    for(i = 0; i < len; i++) {
        pos = str[i];
        if(pos >= 'a' && pos <= 'z') {
            pos -= 'a';
            pos += 26;
        } else if(pos >= 'A' && pos <= 'Z') { 
            pos -= 'A';
        } else if(pos == ' ') {
            pos_x += w;
            continue;
        }
        ix = pos % iw;
        iy = pos / iw;
        img_glyph(img, pos_x, pos_y, ix * w, iy * h, w, h);
        pos_x += w;
    }
}



/* BEGIN RUNT PROCEDURES */

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
    G.point(x, y);
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

static runt_int rproc_circ(runt_vm *vm, runt_ptr p)
{   
    runt_stacklet *s;
    runt_int rc;
    runt_uint x0;
    runt_uint y0;
    runt_uint r;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    r = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y0 = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x0 = s->f;

    img_circ(x0, y0, r);

    return RUNT_OK;
}

static runt_int rproc_ocirc(runt_vm *vm, runt_ptr p)
{   
    runt_stacklet *s;
    runt_int rc;
    runt_uint x0;
    runt_uint y0;
    runt_uint r;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    r = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y0 = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x0 = s->f;

    img_ocirc(x0, y0, r);

    return RUNT_OK;
}

static runt_int rproc_line(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    runt_uint x0;
    runt_uint y0;
    runt_uint x1;
    runt_uint y1;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y1 = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x1 = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y0 = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x0 = s->f;

    img_line(x0, y0, x1, y1);

    return RUNT_OK;
}

static int rproc_width(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->f = G.width;
    return RUNT_OK;
}

static int rproc_height(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->f = G.height;
    return RUNT_OK;
}

static int rproc_load(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    const char *filename;
    img_image *img;


    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    filename = runt_to_string(s->p);
   

    img = malloc(sizeof(img_image));
    rc = img_load(img, filename);

    if(rc) {
        runt_print(vm, "Error: %u: %s\n", rc, lodepng_error_text(rc));
        return RUNT_NOT_OK;
    }

    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->p = runt_mk_cptr(vm, img);

    return RUNT_OK;
}

static int rproc_close(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    img_image *img;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    img = runt_to_cptr(s->p);
    img_close(img);
    free(img);
    return RUNT_OK;
}

static int rproc_glyph(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    img_image *img;
    runt_int pos_x;
    runt_int pos_y;
    runt_int ix;
    runt_int iy;
    runt_int w;
    runt_int h;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    img = runt_to_cptr(s->p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    h = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    w = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    iy = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    ix = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos_y = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos_x = s->f;

    if(img_glyph(img, pos_x, pos_y, ix, iy, w, h)) return RUNT_NOT_OK;

    return RUNT_OK;
}

static int rproc_copy(runt_vm *vm, runt_ptr p)
{
    runt_stacklet *s;
    runt_int rc;
    img_image *img;
    runt_int pos_x;
    runt_int pos_y;
    runt_int ix;
    runt_int iy;
    runt_int w;
    runt_int h;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    img = runt_to_cptr(s->p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    h = s->f;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    w = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    iy = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    ix = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos_y = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    pos_x = s->f;

    if(img_copy(img, pos_x, pos_y, ix, iy, w, h)) return RUNT_NOT_OK;

    return RUNT_OK;
}

static int rproc_xy(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_uint w;
    runt_uint x;
    runt_uint y;
    runt_uint val;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    w = s->f;
   
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    val = s->f;
    if(w <= 0) {
        runt_print(vm, "xy: width cannot be zero\n");
        return RUNT_NOT_OK; 
    }
   
    y = val / w;
    x = val % w;

    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->f = x;
    
    rc = runt_ppush(vm, &s);
    RUNT_ERROR_CHECK(rc);
    s->f = y;
    return RUNT_OK;
}

#ifdef RUNT_IMG_GL
static runt_int rproc_gl(runt_vm *vm, runt_ptr p)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f (1.0, 1.0, 1.0);
    glPixelZoom(1, -1);
    glRasterPos2i (0, 0);
    glDrawPixels(256, 256, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glFlush();
    return RUNT_OK;
}
#endif

static runt_int rproc_setsize(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    G.height = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    G.width= s->f;

    return RUNT_OK;
}

static runt_int rproc_blk(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_float x, y;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;

    img_rect(x * 8, y * 8, 8, 8);

    return RUNT_OK;
}

static runt_int rproc_writec(runt_vm *vm, runt_ptr p)
{
    int x, y;
    int pos;
    unsigned char *d;
    const char *filename;
    const char *varname;
    char macro[25];
    int len;
    FILE *fp;
    int counter;
    runt_stacklet *s;
    runt_uint rc;

    counter = 0;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    filename = runt_to_string(s->p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    varname = runt_to_string(s->p);

    fp = fopen(filename, "w");
    if(fp == NULL) {
        runt_print(vm, "img_writec: could not open file %s\n", filename);
    }

    len = strlen(varname);

    for(x = 0; x < 25; x++) {
        if(x < len) {
            macro[x] = toupper(varname[x]);
        } else {
            macro[x] = 0;
        }
    }
    fprintf(fp, "#ifndef IMG_%s\n#define IMG_%s\n", macro, macro);
    fprintf(fp, "#define IMG_%s_WIDTH %d\n", macro, G.width);
    fprintf(fp, "#define IMG_%s_HEIGHT %d\n", macro, G.height);
    fprintf(fp, "const unsigned char %s[] = {\n", varname);
    d = data;
    for(y = 0; y < G.height; y++) {
        for(x = 0; x < G.width; x++) {
            pos = y * G.width * 4 + x * 4;
            fprintf(fp, "0x%02x, 0x%02x, 0x%02x, ",
                d[pos],
                d[pos + 1],
                d[pos + 2]);
            counter++;
            if(counter % 4 == 0) {
                fprintf(fp, "\n");
            }
        }
    }

    fprintf(fp,"};\n");
    fprintf(fp,"#endif\n");
    fclose(fp);
    return RUNT_OK;
}

static runt_int rproc_writeb(runt_vm *vm, runt_ptr p)
{
    int x, y;
    int pos;
    unsigned char *d;
    const char *filename;
    const char *varname;
    char macro[25];
    int len;
    FILE *fp;
    int counter;
    runt_stacklet *s;
    runt_uint rc;

    counter = 0;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    filename = runt_to_string(s->p);

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    varname = runt_to_string(s->p);

    fp = fopen(filename, "w");
    if(fp == NULL) {
        runt_print(vm, "img_writec: could not open file %s\n", filename);
    }

    len = strlen(varname);

    for(x = 0; x < 25; x++) {
        if(x < len) {
            macro[x] = toupper(varname[x]);
        } else {
            macro[x] = 0;
        }
    }
    fprintf(fp, "#ifndef IMG_%s\n#define IMG_%s\n", macro, macro);
    fprintf(fp, "#define IMG_%s_WIDTH %d\n", macro, G.width);
    fprintf(fp, "#define IMG_%s_HEIGHT %d\n", macro, G.height);
    fprintf(fp, "const unsigned char %s[] = {\n", varname);
    d = data;
    for(y = 0; y < G.height; y++) {
        for(x = 0; x < G.width; x++) {
            pos = y * G.width * 4 + x * 4;
            if(d[pos] != 255 && d[pos + 3] == 255) {
                fprintf(fp, "1, ");
            } else {
                fprintf(fp, "0, ");
            }
            counter++;
            if(counter % 16 == 0) {
                fprintf(fp, "\n");
            }
        }
    }

    fprintf(fp,"};\n");
    fprintf(fp,"#endif\n");
    fclose(fp);
    return RUNT_OK;
}

static runt_int rproc_text(runt_vm *vm, runt_ptr p)
{
    runt_int rc;
    runt_stacklet *s;
    runt_uint width;
    runt_uint height;
    runt_uint x;
    runt_uint y;
    runt_uint scale;
    img_image *img;
    const char *string;

    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    img = runt_to_cptr(s->p);
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    scale = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    height = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    width = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    y = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    x = s->f;
    
    rc = runt_ppop(vm, &s);
    RUNT_ERROR_CHECK(rc);
    string = runt_to_string(s->p);

    img_text(img, x, y, width, height, scale, string);

    return RUNT_OK;
}

runt_int runt_load_img(runt_vm *vm)
{
    unsigned int i;
    G.width = WIDTH;
    G.height = HEIGHT;
    G.point = img_point;
    for(i = 0; i < WIDTH * HEIGHT * 4; i++) data[i] = 0;
    runt_word_define(vm, "img_color", 9, rproc_set_color_rgb);
    runt_word_define(vm, "img_fill", 8, rproc_fill);
    runt_word_define(vm, "img_write", 9, rproc_write);
    runt_word_define(vm, "img_point", 9, rproc_point);
    runt_word_define(vm, "img_rect", 8, rproc_rect);
    runt_word_define(vm, "img_col", 7, rproc_col);
    runt_word_define(vm, "img_row", 7, rproc_row);
    runt_word_define(vm, "img_bin", 7, rproc_bin);
    runt_word_define(vm, "img_circ", 8, rproc_circ);
    runt_word_define(vm, "img_ocirc", 9, rproc_ocirc);
    runt_word_define(vm, "img_line", 8, rproc_line);
    runt_word_define(vm, "img_height", 10, rproc_height);
    runt_word_define(vm, "img_width", 9, rproc_width);
    runt_word_define(vm, "img_load", 8, rproc_load);
    runt_word_define(vm, "img_close", 9, rproc_close);
    runt_word_define(vm, "img_copy", 8, rproc_copy);
    runt_word_define(vm, "img_glyph", 9, rproc_glyph);
    runt_word_define(vm, "img_xy", 6, rproc_xy);
#ifdef RUNT_IMG_GL
    runt_word_define(vm, "img_gl", 6, rproc_gl);
#endif
    runt_word_define(vm, "img_setsize", 11, rproc_setsize);
    runt_word_define(vm, "img_blk", 7, rproc_blk);
    runt_word_define(vm, "img_writec", 10, rproc_writec);
    runt_word_define(vm, "img_text", 8, rproc_text);
    runt_word_define(vm, "img_writeb", 10, rproc_writeb);
    return RUNT_OK;
}
