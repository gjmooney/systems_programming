This program takes an input file, modifies the pixel data by applying a color shifting iffset, and saves the modified image in PPM or BMP format.

To Run:
gcc ImageProcessor.c PixelProcessor.c BmpProcessor.c PpmProcessor.c -o ImageProcessor
./ImageProcessor originalFile.bmp -o outputFile.ppm
./ImageProcessor originalFile.bmp -r 89 -o outputFile.ppm

First argument is file name to modify. Required.

Options:
- -t to set output type, valid arguments are bmp or ppm. Optional, defaults to bmp.
- -r -g -b to set value to shift red, green, or blue pixels by. Optional.
- -o set output file. Optional.
