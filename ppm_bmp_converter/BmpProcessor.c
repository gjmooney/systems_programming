/**
* BMP Image processor. read, write, and create BMP image headers and pixel arrays.
* Modify pixel values of image.
*
* Completion time: 26 Hours
*
* @author Greg Mooney
* @version 1.0
*/
#include "BmpProcessor.h"
#include "PixelProcessor.h"

/**
 * read BMP header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination BMP header
 */
void readBMPHeader(FILE* file, struct BMP_Header* header) {
    fread(&header->signature, sizeof(char)*2, 1, file);
    fread(&header->file_size, sizeof(int), 1, file);
    fread(&header->reserved1, sizeof(short), 1, file);
    fread(&header->reserved2, sizeof(short), 1, file);
    fread(&header->offset_pixel_array, sizeof(int), 1, file);
}

/**
 * write BMP header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeBMPHeader function
 */
void writeBMPHeader(FILE* file, struct BMP_Header* header) {
    fwrite(&header->signature, sizeof(char)*2, 1, file);
    fwrite(&header->file_size, sizeof(int), 1, file);
    fwrite(&header->reserved1, sizeof(short), 1, file);
    fwrite(&header->reserved2, sizeof(short), 1, file);
    fwrite(&header->offset_pixel_array, sizeof(int), 1, file);
}

/**
 * read DIB header from a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination DIB header
 */
void readDIBHeader(FILE* file, struct DIB_Header* header) {
    fread(&header->DIB_header_size, sizeof(unsigned int), 1, file);
    fread(&header->width, sizeof(int), 1, file);
    fread(&header->height, sizeof(int), 1, file);
    fread(&header->planes, sizeof(short), 1, file);
    fread(&header->bits_per_pixel, sizeof(short ), 1, file);
    fread(&header->compression, sizeof(unsigned int), 1, file);
    fread(&header->image_size, sizeof(unsigned int), 1, file);
    fread(&header->x_pixels_per_meter, sizeof(int), 1, file);
    fread(&header->y_pixels_per_meter, sizeof(int), 1, file);
    fread(&header->colors_in_color_table, sizeof(unsigned int), 1, file);
    fread(&header->important_color_count, sizeof(unsigned int), 1, file);
}

/**
 * write DIB header of a file. Useful for converting files from PPM to BMP.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makeDIBHeader function
 */
void writeDIBHeader(FILE* file, struct DIB_Header* header) {
    fwrite(&header->DIB_header_size, sizeof(unsigned int), 1, file);
    fwrite(&header->width, sizeof(int), 1, file);
    fwrite(&header->height, sizeof(int), 1, file);
    fwrite(&header->planes, sizeof(short), 1, file);
    fwrite(&header->bits_per_pixel, sizeof(short ), 1, file);
    fwrite(&header->compression, sizeof(unsigned int), 1, file);
    fwrite(&header->image_size, sizeof(unsigned int), 1, file);
    fwrite(&header->x_pixels_per_meter, sizeof(int), 1, file);
    fwrite(&header->y_pixels_per_meter, sizeof(int), 1, file);
    fwrite(&header->colors_in_color_table, sizeof(unsigned int), 1, file);
    fwrite(&header->important_color_count, sizeof(unsigned int), 1, file);
}

/**
 * make BMP header based on width and height.
 * The purpose of this is to create a new BMPHeader struct using the information
 * from a PPMHeader when converting from PPM to BMP.
 *
 * @param  header: Pointer to the destination DIB header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makeBMPHeader(struct BMP_Header* header, int width, int height) {
    header->signature[0] = 'B';
    header->signature[1] = 'M';
    header->reserved1 = 0;
    header->reserved2 = 0;
    // ceilVal = (a+b-1) / b
    // Row Size = Ceiling((24 * width) / 32) * 4
    header->file_size = ((((24 * width) + 32 - 1) / 32) * 4) * height + 54;
    header->offset_pixel_array = 54;
}

/**
* Makes new DIB header based on width and height. Useful for converting files from PPM to BMP.
*
* @param  header: Pointer to the destination DIB header
* @param  width: Width of the image that this header is for
* @param  height: Height of the image that this header is for
*/
void makeDIBHeader(struct DIB_Header* header, int width, int height) {
    header->DIB_header_size = 40;
    header->width = width;
    header->height = height;
    header->planes = 1;
    header->bits_per_pixel = 24;
    header->compression = 0;
    header->image_size = ((((24 * width) + 32 - 1) / 32) * 4) * height;
    header->x_pixels_per_meter = 3780;
    header->y_pixels_per_meter = 3780;
    header->colors_in_color_table = 0;
    header->important_color_count = 0;

}

/**
 * read Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void readPixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    // allocated 3 bytes for every pixel
    for (int i = height-1; i >= 0; --i) {
        for (int j = 0; j < width; ++j) {
            fread(&pArr[i][j].b, sizeof(unsigned char), 1, file);
            fread(&pArr[i][j].g, sizeof(unsigned char), 1, file);
            fread(&pArr[i][j].r, sizeof(unsigned char), 1, file);
        }
        if (height % 4 != 0) { fseek(file, sizeof(unsigned char) * 2, SEEK_CUR); }
    }
}

/**
 * write Pixels from BMP file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsBMP(FILE* file, struct Pixel** pArr, int width, int height) {
    short padding = 0;
    for (int i = height-1; i >= 0; --i) {
        for (int j = 0; j < width; ++j) {
            fwrite(&pArr[i][j].b, sizeof(unsigned char), 1, file);
            fwrite(&pArr[i][j].g, sizeof(unsigned char), 1, file);
            fwrite(&pArr[i][j].r, sizeof(unsigned char), 1, file);
        }
        if(height % 4 != 0) {
            fwrite(&padding, sizeof(short), 1, file);
        }
    }
}
