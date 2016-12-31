# runt-img

Runt-img is a very simple raster graphics library for 
[runt](https://www.github.com/paulbatchelor/runt.git)
designed to create procedurally generated pixel art with a 256x256 resolution.
It is written in ANSI C. 

Under the hood. runt-img uses 
[lodepng](http://www.lodev.org/lodepng) (included with
runt-img) to write PNG files. 

To compile, simply run "make" and "sudo make install". You will need to have
Runt installed first (it *is* a library for the runt language). Be sure to set
RUNT_PLUGIN_PATH to "/usr/local/include/runt". 

Some small examples can be found in the "examples" folder. To run an example,
run:

    irunt -b lines.rnt

Whhich will write a png called "lines.png". 
