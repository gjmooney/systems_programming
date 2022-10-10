/**
* Pixel processor. Shift values of pixels by given amount.
*
* Completion time: 26 Hours
*
* @author Greg Mooney
* @version 1.0
*/
#include "PixelProcessor.h"

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
void colorShiftPixels(struct Pixel** pArr, int width, int height, int rShift, int gShift, int bShift) {
    int tmpB, tmpG, tmpR;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (bShift != 0) {
                tmpB = (int) pArr[i][j].b;
                tmpB += bShift;
                pArr[i][j].b = (char) tmpB;
                if (tmpB < 0) {
                    pArr[i][j].b = 0;
                }
                else if (tmpB > 255) {
                    pArr[i][j].b = 255;
                }
            }

            if (gShift != 0) {
                tmpG = (int) pArr[i][j].g;
                tmpG += gShift;
                pArr[i][j].g = (char) tmpG;
                if (tmpG < 0) {
                    pArr[i][j].g = 0;
                }
                else if (tmpG > 255) {
                    pArr[i][j].g = 255;
                }
            }

            if(rShift != 0) {
                tmpR = (int) pArr[i][j].r;
                tmpR += rShift;
                pArr[i][j].r = (char) tmpR;
                if (tmpR < 0) {
                    pArr[i][j].r = 0;
                }
                else if (tmpR > 255) {
                    pArr[i][j].r = 255;
                }
            }
        }
    }
}