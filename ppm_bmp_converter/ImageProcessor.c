/**
* Image processor. Change pixel values of image and convert to ppm or bmp
*
* Completion time: 26 Hours
*
* @author Greg Mooney
* @version 1.0
*/

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "PixelProcessor.h"
#include "BmpProcessor.h"
#include "PpmProcessor.h"

// Globals
BMP_Header bmp_header;
DIB_Header dib_header;
PPM_Header ppm_header;
Pixel** pixel_array;

// Function Declarations
void read_file(char* input_file, int file_length);
void write_file(int r, int g, int b, char* output_file, int input_type, int output_type);
void bmp_to_bmp(FILE* file, int r, int g, int b);
void bmp_to_ppm(FILE* file, int r, int g, int b);
void ppm_to_bmp(FILE* file, int r, int g, int b);
void ppm_to_ppm(FILE* file, int r, int g, int b);
void free_memory(int width);
void print_usage();

// Implementation
void read_file(char* input_file, int file_length) {
    FILE* file = fopen(input_file, "rb");
    if ((strcmp(&input_file[file_length-4], ".bmp") == 0)) {
        readBMPHeader(file, &bmp_header);
        readDIBHeader(file, &dib_header);
        pixel_array = malloc(dib_header.height * sizeof(Pixel*));
        for (int i = 0; i < dib_header.width; ++i) {
            pixel_array[i] = malloc(dib_header.width * sizeof(Pixel));
        }
        readPixelsBMP(file, pixel_array, dib_header.width, dib_header.height);
    } else {
        readPPMHeader(file, &ppm_header);
        pixel_array = malloc(ppm_header.height * sizeof(Pixel*));
        for (int i = 0; i < ppm_header.width; ++i) {
            pixel_array[i] = malloc(ppm_header.width * sizeof(Pixel));
        }
        readPixelsPPM(file, pixel_array, ppm_header.width, ppm_header.height);
    }
    fclose(file);
}

void write_file(int r, int g, int b, char* output_file, int input_type, int output_type) {
    FILE* output = fopen(output_file, "wb");

    switch (input_type) {
        case 0:
            if (output_type == 0) {
                bmp_to_bmp(output, r, g, b);
                break;
            } else {
                bmp_to_ppm(output, r, g, b);
                break;
            }
        case 1:
            if (output_type == 0) {
                ppm_to_bmp(output, r, g, b);
            } else {
                ppm_to_ppm(output, r, g, b);
            }
    }
    fclose(output);
}

void bmp_to_bmp(FILE* file, int r, int g, int b) {
    writeBMPHeader(file, &bmp_header);
    writeDIBHeader(file, &dib_header);
    colorShiftPixels(pixel_array, dib_header.width, dib_header.height, r, g, b);
    writePixelsBMP(file, pixel_array, dib_header.width, dib_header.height);
    free_memory(dib_header.width);
}

void bmp_to_ppm(FILE* file, int r, int g, int b) {
    PPM_Header new_ppm_header;
    makePPMHeader(&new_ppm_header, dib_header.width, dib_header.height);
    writePPMHeader(file, &new_ppm_header);
    colorShiftPixels(pixel_array, new_ppm_header.width, new_ppm_header.height, r, g, b);
    writePixelsPPM(file, pixel_array, new_ppm_header.width, new_ppm_header.height);
    free_memory(new_ppm_header.width);
}

void ppm_to_bmp(FILE* file, int r, int g, int b) {
    BMP_Header new_bmp_header;
    DIB_Header new_dib_header;
    makeBMPHeader(&new_bmp_header, ppm_header.width, ppm_header.height);
    makeDIBHeader(&new_dib_header, ppm_header.width, ppm_header.height);
    writeBMPHeader(file, &new_bmp_header);
    writeDIBHeader(file, &new_dib_header);
    colorShiftPixels(pixel_array, new_dib_header.width, new_dib_header.height, r, g, b);
    writePixelsBMP(file, pixel_array, new_dib_header.width, new_dib_header.height);
    free_memory(new_dib_header.width);
}

void ppm_to_ppm(FILE* file, int r, int g, int b) {
    writePPMHeader(file, &ppm_header);
    colorShiftPixels(pixel_array, ppm_header.width, ppm_header.height, r, g, b);
    writePixelsPPM(file, pixel_array, ppm_header.width, ppm_header.height);
    free_memory(ppm_header.width);
}

void print_usage(){
    printf("USAGE:\n./MooneyImageProcessor <input_file_name(char*)> -r <value(int)> -g <value(int)> -b <value(int)> -o <output_file_name(char*) -t <output_file_type(char*)\n\n");
    printf("data_file_name refers to the input file and is a required field. Only .bmp and .ppm file are supported.\n");
    printf("-r -g & -b refers to the values used to manipulate the pixel data in the output file. These values must be integers. These are optional fields. \n");
    printf("-o refers to the desired output file name. This field is optional. The default name is output.\n");
    printf("-t refers to the desired output file type. Only .bmp and .ppm file types are supported. This field is optional. The default selection is .bmp\n");
}

void free_memory(int width) {
    for (int i = 0; i < width; ++i) {
        free(pixel_array[i]);
    }
    free(pixel_array);
}

int main(int argc, char *argv[]) {
    char* datafile;
    int opt;
    int r = 0, g = 0, b = 0;
    int o = 0;
    char* t = "bmp";
    char* output_file;
    // 0 for bmp, 1 for ppm
    int input_type, output_type;

    while ((opt = getopt(argc, argv, ":r:g:b:o:t:")) != -1) {
        switch (opt) {
            case 'r':
                if (strcmp(optarg, "0") == 0) {
                    r = atoi(optarg);
                } else {
                    r = atoi(optarg);
                    if (r == 0) {
                        printf("Value must be an integer.\n");
                        print_usage();
                        exit(1);
                    }
                }
                break;
            case 'g':
                if (strcmp(optarg, "0") == 0) {
                    g = atoi(optarg);
                } else {
                    g = atoi(optarg);
                    if (g == 0) {
                        printf("Value must be an integer.\n");
                        print_usage();
                        exit(1);
                    }
                }
                break;
            case 'b':
                if (strcmp(optarg, "0") == 0) {
                    b = atoi(optarg);
                } else {
                    b = atoi(optarg);
                    if (b == 0) {
                        printf("Value must be an integer.\n");
                        print_usage();
                        exit(1);
                    }
                }
                break;
            case 'o':
                o = 1;
                output_file = optarg;
                break;
            case 't':
                if ((strcmp(optarg, "bmp") == 0) || (strcmp(optarg, "ppm") == 0)) {
                    t = optarg;
                } else {
                    printf("Only .bmp and .ppm outputs are supported.\n");
                    print_usage();
                    exit(1);
                }
                break;
            case ':':
                printf("Option needs a value\n");
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
        }
    }

    // Read input file name
    datafile = argv[optind];
    optind++;

    // Extra arguments
    for(; optind < argc; optind++) {
        printf("Extra arguments: %s\n", argv[optind]);
    }

    // Set default output file if -o option is not used
    if (o == 0) {
        strcpy(output_file, "output");
    }

    // Create output files full name
    strcat(output_file, ".");
    strcat(output_file, t);

    // set output_type flag based on -t flag
    if (strcmp("bmp", t) == 0) {
        output_type = 0;
    } else {
        output_type = 1;
    }

    int dflen;
    if (datafile != NULL) {
        dflen = strlen(datafile);
        if (dflen >= 5 && (strcmp(&datafile[dflen-4], ".bmp") == 0) && (access(datafile, F_OK) != -1)) {
            input_type = 0;
            printf("Reading data from %s\n\n", datafile);
            read_file(datafile, dflen);
            printf("Writing data to %s\n", output_file);
            write_file(r, g, b, output_file, input_type, output_type);
        } else if (dflen >= 5 && (strcmp(&datafile[dflen-4], ".ppm") == 0) && (access(datafile, F_OK) != -1)) {
            input_type = 1;
            printf("Reading data from %s\n\n", datafile);
            read_file(datafile, dflen);
            printf("Writing data to %s\n", output_file);
            write_file(r, g, b, output_file, input_type, output_type);
        } else {
            printf("Input file has an invalid name or it does not exist.\n");
            print_usage();
            exit(1);
        }
    } else {
        printf("No input file name provided. This is required.\n");
        print_usage();
        exit(1);
    }

    return 0;
}

