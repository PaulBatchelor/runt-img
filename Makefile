default: librunt_img.a runt_img

CFLAGS = -ansi -fPIC -pedantic -Wall

OBJ=img.o lodepng.o

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

librunt_img.a: $(OBJ)
	ar rcs $@ $(OBJ)

runt_img: parse.c librunt_img.a
	$(CC) parse.c -o $@ -lrunt librunt_img.a  -ldl -lGL

clean:
	rm -rf $(OBJ) librunt_img.a runt_img

