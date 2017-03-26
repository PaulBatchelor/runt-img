# runt-img

Runt-img is a very simple raster graphics library for 
[runt](https://www.github.com/paulbatchelor/runt.git)
designed to create procedurally generated pixel art with a 256x256 resolution.
It is written in ANSI C. 

Under the hood. runt-img uses 
[lodepng](http://www.lodev.org/lodepng) (included with
runt-img) to write PNG files. 

## Compilation

You will need to have [runt](installed)

To compile, simply run:

    make

This will make the library *librunt_img.a*, and a runt interpretor called
*rntimg*. The interpretor can be used to try out some of the examples. For 
instance, to run the lines example, run:

    ./rntimg examples/lines.rnt

Which will write a file lines.png

