default: librunt_img.a rntimg

CFLAGS = -ansi -fPIC -pedantic -Wall -I$(HOME)/.runt/include
LDFLAGS = -L$(HOME)/.runt/lib -lrunt librunt_img.a  -ldl -lGL


OBJ=img.o lodepng.o

%.o: %.c
	$(CC) $< $(CFLAGS) -c -o $@

librunt_img.a: $(OBJ)
	ar rcs $@ $(OBJ)

rntimg: parse.c librunt_img.a
	$(CC) $(CFLAGS) parse.c -o $@ $(LDFLAGS)

install: librunt_img.a
	mkdir -p ~/.runt/lib
	mkdir -p ~/.runt/bin
	mkdir -p ~/.runt/include
	cp librunt_img.a ~/.runt/lib
	cp rntimg ~/.runt/bin/
	cp img.h ~/.runt/include/

clean:
	rm -rf $(OBJ) librunt_img.a rntimg

