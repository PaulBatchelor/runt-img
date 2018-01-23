#ifndef RUNTIMG_H
#define RUNTIMG_H
runt_int runt_load_img(runt_vm *vm);
void img_fill();
void img_point(unsigned int x, unsigned int y);
void img_write(const char *filename);
void img_row(unsigned char x, unsigned char y, unsigned char col, unsigned char s);
void set_color_rgb(
        unsigned char r, 
        unsigned char g, 
        unsigned char b) ;
void set_color_rgba(
        unsigned char r, 
        unsigned char g, 
        unsigned char b, 
        unsigned char a);
void img_rect(unsigned int x_pos, 
    unsigned int y_pos,
    unsigned int width,
    unsigned int height);
void img_col(unsigned char x, unsigned char y, unsigned char row, unsigned char s);
void img_line(int x0, int y0, int x1, int y1);
void img_circ(int x0, int y0, int radius);
void img_ocirc(int x0, int y0, int radius);
void img_get(int x, int y, unsigned char **col);
unsigned char *img_get_current_color();
unsigned char *img_get_data();
void img_set_point_function(void (*f)(unsigned int, unsigned int));
void img_setsize(unsigned int w, unsigned int h);
#endif
