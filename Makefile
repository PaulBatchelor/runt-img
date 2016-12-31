default: img.so

CFLAGS = -ansi -fPIC -pedantic -Wall

lodepng.o: lodepng.c
	gcc $< $(CFLAGS) -c -o $@

img.so: img.c lodepng.o
	gcc $(CFLAGS) -shared img.c -o $@ lodepng.o -lrunt

clean:
	rm -rf *.o img.so

install: img.so
	mkdir -p /usr/local/share/runt
	install img.so /usr/local/share/runt/
