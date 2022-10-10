/**
* Pixel processor. Shift values of pixels by given amount.
*
* Completion time: 26 Hours
*
* @author Greg Mooney
* @version 1.0
*/
#ifndef PIXEL_PROCESSOR_H
#define PIXEL_PROCESSOR_H

typedef struct Pixel{
    unsigned char r, g, b;
} Pixel;

/**
 * Shift color of Pixel array. The dimension of the array is width * height. The shift value of RGB is
 * rShift, gShift，bShift. Useful for color shift.
 *
 * @param  pArr: Pixel array of the image that this header is for
 * @param  width: Width of the image that this header is for
 * @param  height: Height of the image that this header is for
 * @param  rShift: the shift value of color r shift
 * @param  gShift: the shift value of color g shift
 * @param  bShift: the shift value of color b shift
 */
void colorShiftPixels(struct Pixel** pArr, int width1, int width2, int height, int rShift, int gShift, int bShift);

#endif