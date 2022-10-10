/**
* Apply blur or swiss cheese filters to an image
*
* Completion time: 20 hours
*
* @author Greg Mooney
* @version 1
*/

////////////////////////////////////////////////////////////////////////////////
//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include "BmpProcessor.h"
#include "PixelProcessor.h"

////////////////////////////////////////////////////////////////////////////////
//MACRO DEFINITIONS

//problem assumptions
#define BMP_HEADER_SIZE 14
#define BMP_DIB_HEADER_SIZE 40
#define MAXIMUM_IMAGE_SIZE 4096
#define NEIGHBOR_HEIGHT 3
#define NEIGHBOR_WIDTH 3
#define THREAD_COUNT 4

////////////////////////////////////////////////////////////////////////////////
//DATA STRUCTURES

typedef struct thread_info {
    int start;
    int end;
    double avg_radius;
    int section_holes;
} thread_info;


////////////////////////////////////////////////////////////////////////////////
//MAIN PROGRAM CODE

// Globals
BMP_Header bmp_header;
DIB_Header dib_header;
Pixel** pixel_array;
thread_info info[THREAD_COUNT];
char* filter_flag;
pthread_t* threads;

// Function Declarations
void read_file(char* input_file, int file_length);
void write_file(int r, int g, int b, char *output_file);
void bmp_to_bmp(FILE* file, int r, int g, int b);
void free_memory(int width);
void blur_filter_calc(int row, int col);
void* cheese_filter(void *param);
void thread_setup();
void* blur_function(void* param);
void print_usage();
void draw_circle(int x, int y, double r);
double rand_normal_dist();

// Implementation
void read_file(char* input_file, int file_length) {
    FILE* file = fopen(input_file, "rb");
    if (!file)perror("fopen");

    readBMPHeader(file, &bmp_header);
    readDIBHeader(file, &dib_header);

    pixel_array = malloc(dib_header.height * sizeof(Pixel*));
    for (int i = 0; i < dib_header.width; ++i) {
        pixel_array[i] = malloc(dib_header.width * sizeof(Pixel));
    }
    readPixelsBMP(file, pixel_array, dib_header.width, dib_header.height);
    fclose(file);
}

void write_file(int r, int g, int b, char *output_file) {
    FILE* output = fopen(output_file, "wb");
    bmp_to_bmp(output, r, g, b);
    fclose(output);
}

// this needs to happen to each pixel
void blur_filter_calc(int row, int col) {
    int avg_r, avg_g, avg_b;
    int sum_r = 0, sum_g = 0, sum_b = 0;
    int touched = 0;
    int r = -1, c = -1;

    // Top row
    if (row == 0) {
        // Top left corner
        if (col == 0) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    sum_r += pixel_array[row + i][col + j].r;
                    sum_g += pixel_array[row + i][col + j].g;
                    sum_b += pixel_array[row + i][col + j].b;
                    touched++;
                }
            }
        }
        //Top right corner
        else if (col == dib_header.width - 1) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    sum_r += pixel_array[row + i][col + c].r;
                    sum_g += pixel_array[row + i][col + c].g;
                    sum_b += pixel_array[row + i][col + c].b;
                    touched++;
                    c++;
                }
                c = -1;
            }
        }
        // Top row not corners
        else {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < NEIGHBOR_WIDTH; ++j) {
                    sum_r += pixel_array[row + i][col + c].r;
                    sum_g += pixel_array[row + i][col + c].g;
                    sum_b += pixel_array[row + i][col + c].b;
                    touched++;
                    c++;
                }
                c = -1;
            }
        }
    }
    // Bottom row
    else if (row == dib_header.height - 1) {
        // Bottom left corner
        if (col == 0) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    sum_r += pixel_array[row + r][col + j].r;
                    sum_g += pixel_array[row + r][col + j].g;
                    sum_b += pixel_array[row + r][col + j].b;
                    touched++;
                }
                r++;
            }
        // Bottom right corner
        } else if (col == dib_header.width - 1) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    sum_r += pixel_array[row + r][col + c].r;
                    sum_g += pixel_array[row + r][col + c].g;
                    sum_b += pixel_array[row + r][col + c].b;
                    touched++;
                    c++;
                }
                r++;
            }
        }
        // Bottom row not corners
        else {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < NEIGHBOR_WIDTH; ++j) {
                    sum_r += pixel_array[row - i][col + c].r;
                    sum_g += pixel_array[row - i][col + c].g;
                    sum_b += pixel_array[row - i][col + c].b;
                    touched++;
                    c++;
                }
                c = -1;
            }
        }
    }
    // Left side, not top or bottom edge
    else if (col == 0) {
        for (int i = 0; i < NEIGHBOR_HEIGHT; ++i) {
            for (int j = 0; j < 2; ++j) {
                sum_r += pixel_array[row + r][col + j].r;
                sum_g += pixel_array[row + r][col + j].g;
                sum_b += pixel_array[row + r][col + j].b;
                touched++;
            }
            r++;
        }
    }
    // Right edge, not top or bottom row
    else if (col == dib_header.width - 1) {
        for (int i = 0; i < NEIGHBOR_HEIGHT; ++i) {
            for (int j = 0; j < 2; ++j) {
                sum_r += pixel_array[row + r][col - j].r;
                sum_g += pixel_array[row + r][col - j].g;
                sum_b += pixel_array[row + r][col - j].b;
                touched++;
            }
            r++;
        }
    }
    // Everything else
    else {
        for (int i = 0; i < NEIGHBOR_HEIGHT; ++i) {
            for (int j = 0; j < NEIGHBOR_WIDTH; ++j) {
                sum_r += pixel_array[row + r][col + c].r;
                sum_g += pixel_array[row + r][col + c].g;
                sum_b += pixel_array[row + r][col + c].b;
                c++;
                touched++;
            }
            c = -1;
            r++;
        }
    }

    avg_r = sum_r / touched;
    avg_g = sum_g / touched;
    avg_b = sum_b / touched;

    pixel_array[row][col].r = avg_r;
    pixel_array[row][col].g = avg_g;
    pixel_array[row][col].b = avg_b;
}

void* cheese_filter(void *param) {
    thread_info* in = (thread_info*) param;

    // Tint the image yellow
    colorShiftPixels(pixel_array, in->start, in->end, dib_header.height,50, 50, -50);

    double radius;

    // Draw hole for this threads section
    for (int i = 0; i <= in->section_holes; ++i) {
        // Try again in case radius ends up 0
        do {
            // Calculate radius using normal distribution.
            // Using avg_radius as mu and 1/3 the avg_radius as the variance
            radius = rand_normal_dist() * (in->avg_radius / 3) + in->avg_radius;
        } while (radius == 0);

        // Draw circle in this section
        draw_circle(rand() % in->end + in->start, rand() % dib_header.height, radius);
    }
    pthread_exit(0);
}

// Box-Muller Transform
double rand_normal_dist() {
    double rand1 = ((double)(rand()) + 1 )/((double)(RAND_MAX) + 1);
    double rand2 = ((double)(rand()) + 1 )/((double)(RAND_MAX) + 1);
    double norm = sqrt(-2.0 * log(rand1)) * cos(2.0 * M_PI * rand2);
    return norm;
}

void draw_circle(int x, int y, double r) {
    double theta;
    double x1, y1;
    int row1, row2, col1, col2;

    for (int i = 0; i <= 90; i++) {
        theta = i;
        x1 = r * cos(theta * M_PI / 180);
        y1 = r * sin(theta * M_PI / 180);

        if (y1 == 0) {y1 = 1;}

        col1 = x - x1;
        col2 = x + x1;
        row1 = y - y1;
        row2 = y + y1;

        if (col1 < 0) { col1 = 0;}
        if (col2 >= dib_header.width) {col2 = dib_header.width-1;}
        if (row1 < 0) { row1 = 0;}
        if (row2 >= dib_header.height) { row2 = dib_header.height-1; }

        for (int j = col1; j <= col2; ++j) {
            for (int k = row1; k <= row2 ; ++k) {
                pixel_array[k][j].b = 0;
                pixel_array[k][j].g = 0;
                pixel_array[k][j].r = 0;
            }
        }
    }
}

void free_memory(int width) {
    for (int i = 0; i < width; ++i) {
        free(pixel_array[i]);
    }
    free(pixel_array);
}

void bmp_to_bmp(FILE* file, int r, int g, int b) {
    writeBMPHeader(file, &bmp_header);
    writeDIBHeader(file, &dib_header);
    colorShiftPixels(pixel_array, dib_header.width, dib_header.width, dib_header.height, r, g, b);
    writePixelsBMP(file, pixel_array, dib_header.width, dib_header.height);
    free_memory(dib_header.width);
}

void thread_setup() {
    int section_size = dib_header.width/THREAD_COUNT;

    // Divide image into equal columns
    for (int i = 0; i < THREAD_COUNT; ++i) {
        info[i].start = (section_size * i);
        info[i].end = section_size * (i + 1);
    }

    // Create threads
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    threads = (pthread_t*) malloc(sizeof(pthread_t) * THREAD_COUNT);

    // Blur filter
    if (*filter_flag == 'b') {
        // Add padding to section size
        for (int i = 0; i < THREAD_COUNT; ++i) {
            if (info[i].start == 0 && info[i].end != dib_header.width) {
                info[i].end++;
            }
            if (info[i].end == dib_header.width && info[i].start != 0) {
                info[i].start--;
            }
            if (info[i].start != 0 && info[i].end != dib_header.width) {
                info[i].start--;
                info[i].end++;
            }
        }
        for (int i = 0; i < THREAD_COUNT; ++i) {
            pthread_create(&threads[i], &attr, blur_function, &info[i]);
        }
    }
    // Cheese filter
    else {
        int smallest_side;
        if (dib_header.width < dib_header.height)
            smallest_side = dib_header.width;
        else
            smallest_side = dib_header.height;

        // average radius and number of holes are 10% of the smallest side.
        double avg_radius = smallest_side / 10.0;
        int num_holes = smallest_side / 10;

        // How many holes each thread is responsible for
        int section_holes = num_holes / THREAD_COUNT;

        for (int i = 0; i < THREAD_COUNT; ++i) {
            info[i].avg_radius = avg_radius;
            info[i].section_holes = section_holes;
        }

        for (int i = 0; i < THREAD_COUNT; ++i) {
            pthread_create(&threads[i], &attr, cheese_filter, &info[i]);
        }
    }

}

void* blur_function(void* param) {
    thread_info* in = (thread_info*) param;

    // Call blur filter on each pixel
    for (int i = 0; i < dib_header.height; ++i) {
        for (int j = in->start; j < in->end; ++j) {
            blur_filter_calc(i, j);
        }
    }
    pthread_exit(0);
}

void print_usage(){
    printf("USAGE:\n./Filter -i <input_file_name(char*)> -o <output_file_name(char*) -f <filter_type(char)>\n\n");
    printf("-i refers to the input file and is a required field. Only .bmp files are supported.\n");
    printf("-o refers to the desired output file name. Only .bmp files are supported. This field is required.\n");
    printf("-f refers to the desired image filter. Supported filters are blur using b and swiss cheese using c. "
           "This field is required.\n");
}

int main(int argc, char* argv[]) {
    char* input_file;
    char* output_file;
    int opt;
    int r = 0, g = 0, b = 0;
    srand(time(0));

    while ((opt = getopt(argc, argv, ":i:o:f:")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'f':
                if (!(strcmp(optarg, "b") == 0 || strcmp(optarg, "c") == 0)) {
                    printf("Please use b for blur filter of c for swiss cheese filter.\n");
                    print_usage();
                    exit(1);
                }
                filter_flag = optarg;
                break;
            case ':':
                printf("Option needs a value.\n");
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
        }
    }

    // Extra arguments
    for(; optind < argc; optind++) {
        printf("Extra arguments: %s\n", argv[optind]);
    }

    if (output_file == NULL) {
        printf("Output file name is a required field.\n");
        print_usage();
        exit(1);
    }

    if (filter_flag == NULL) {
        printf("Filter flag is a required field.\n");
        print_usage();
        exit(1);
    }

    int dflen;
    if (input_file != NULL) {
        dflen = strlen(input_file);
        if (dflen >= 5 && (strcmp(&input_file[dflen-4], ".bmp") == 0) && (access(input_file, F_OK) != -1)) {
            printf("Reading data from %s\n", input_file);
            read_file(input_file, dflen);


            // filter stuff happens in thread_setup
            thread_setup();
            printf("Writing data to %s\n", output_file);
            write_file(r, g, b, output_file);
            // Combine threads
            for (int i = 0; i < THREAD_COUNT; ++i) {
                pthread_join(threads[i], NULL);
            }

        }
        else {
            printf("Input file has an invalid name or it does not exist.\n");
            print_usage();
            exit(1);
        }
    }
    else {
        printf("No input file name provided. This is required.\n");
        print_usage();
        exit(1);
    }

    free(threads);
    threads = NULL;

    return  0;
}
