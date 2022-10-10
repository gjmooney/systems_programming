Loads a BMP file and applies a box blur or 'swiss cheese' filter to the image using multiple threads to increase performance.

compile with
gcc Filters.c PixelProcessor.c BmpProcessor.c -lpthread -lm -o Filters

Run With
./Filters -i in.bmp -o out.bmp -f b

Where "in.bmp" represents the desired input file, and "out.bmp" represents the desired output file name
-f flag sets the filter type. "c" is for the swiss cheese filter and "b" is for the blur filter.

Options:
- -i Name of input file
- -o Name of output file
- -f Selects type of filter to use, 'c' for swiss cheese filter, and 'b' for blur filter



