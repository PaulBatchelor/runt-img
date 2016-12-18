default: img.so

lodepng.o: lodepng.c
	gcc $< -fPIC -c -o $@

img.so: img.c lodepng.o
	gcc -shared -fPIC img.c -o $@ lodepng.o -lrunt

clean:
	rm -rf *.o img.so
