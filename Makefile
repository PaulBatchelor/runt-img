default: img.so

lodepng.o: lodepng.c
	gcc $< -fPIC -c -o $@

img.so: img.c lodepng.o
	gcc -Wall -ansi -pedantic -shared -fPIC img.c -o $@ lodepng.o -lrunt

clean:
	rm -rf *.o img.so

install: img.so
	mkdir -p /usr/local/share/runt
	install img.so /usr/local/share/runt/
