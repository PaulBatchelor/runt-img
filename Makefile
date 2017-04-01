default: librunt_img.a rntimg

CFLAGS = -ansi -fPIC -pedantic -Wall

OBJ=img.o lodepng.o

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

librunt_img.a: $(OBJ)
	ar rcs $@ $(OBJ)

rntimg: parse.c librunt_img.a
	$(CC) parse.c -o $@ -lrunt librunt_img.a  -ldl -lGL

install: librunt_img.a
	mkdir -p ~/.runt/lib
	mkdir -p ~/.runt/bin
	mkdir -p ~/.runt/include
	cp librunt_img.a ~/.runt/lib
	cp rntimg ~/.runt/bin/
	cp img.h ~/.runt/include/

clean:
	rm -rf $(OBJ) librunt_img.a rntimg

