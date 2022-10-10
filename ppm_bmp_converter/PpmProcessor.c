/**
* PPM Image processor. read, write, and create PPM image headers and pixel arrays.
*
* Completion time: 26 Hours
*
* @author Greg Mooney
* @version 1.0
*/
#include <string.h>
#include "PpmProcessor.h"
#include "PixelProcessor.h"

/**
 * read PPM header of a file. Useful for converting files from BMP to PPM.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: Pointer to the destination PPM header
 */
void readPPMHeader(FILE* file, struct PPM_Header* header) {
    fscanf(file, "%s", &header->magic_number);
    fscanf(file, "%d", &header->width);
    fscanf(file, "%d", &header->height);
    fscanf(file, "%d", &header->maximum_color_value);
    fscanf(file, " ");
}

/**
 * write PPM header of a file. Useful for converting files from BMP to PPM.
 *
 * @param  file: A pointer to the file being read or written
 * @param  header: The header made by makePPMHeader function

 */
void writePPMHeader(FILE* file, struct PPM_Header* header) {
    fprintf(file, "%s", &header->magic_number);
    fprintf(file, "\n");
    fprintf(file, "%d", header->width);
    fprintf(file, "\n");
    fprintf(file, "%d", header->height);
    fprintf(file, "\n");
    fprintf(file, "%d", header->maximum_color_value);
    fprintf(file, "\n");
}

/**
 * make PPM header based on width and height. Useful for converting files from BMP to PPM.
 *
 * @param  header: Pointer to the destination PPM header
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void makePPMHeader(struct PPM_Header* header, int width, int height) {
    strcpy(&header->magic_number, "P6");
    header->width = width;
    header->height = height;
    header->maximum_color_value = 255;
}

/**
 * read Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void readPixelsPPM(FILE* file, struct Pixel** pArr, int width, int height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fscanf(file, "%c", &pArr[i][j].r);
            fscanf(file, "%c", &pArr[i][j].g);
            fscanf(file, "%c", &pArr[i][j].b);
        }
    }
}

/**
 * write Pixels from PPM file based on width and height.
 *
 * @param  file: A pointer to the file being read or written
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 */
void writePixelsPPM(FILE* file, struct Pixel** pArr, int width, int height) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(file, "%c", pArr[i][j].r);
            fprintf(file, "%c", pArr[i][j].g);
            fprintf(file, "%c", pArr[i][j].b);
        }
    }
}