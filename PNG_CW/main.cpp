#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <png.h>
#include <cstring>
#include <zlib.h>
#define OPEN_FILE 100
#define PNG_DEBUG 3

using namespace std;


//str 32
struct Png{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;

    png_structp png_ptr;
    png_infop info_ptr;
    int number_of_passes;
    png_bytep *row_pointers;
};

void read_png_file(char *file_name, struct Png *image);
void write_png_file(char *file_name, struct Png *image);
void process_file(struct Png *image);

int main(int argc, char **argv) {
    if (argc != 3){
        fprintf(stderr,"Usage: program_name <file_in> <file_out>\n");
        return 0;
    }

    struct Png image={};
    //char sorce[]="/home/kot/etu_v2.0/cpp/rick.png";
    //char dest[]="/home/kot/etu_v2.0/cpp/PNG_CW/test.png";
    read_png_file(argv[1], &image);
    //process_file(&image);
    write_png_file(argv[2], &image);
    printf("ok\n");

    return 0;
}



void read_png_file(char *file_name, struct Png *image) {
    int x,y;
    char header[8];    // 8 is the maximum size that can be checked
    //png_bytep header= nullptr;

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp){
        cout << "Can't open file to read. Bye\n";
        exit(OPEN_FILE);
    }
    //png_bytep ptr_hdr= nullptr;

    fread(header, 1, 8, fp);
    /*if (!png_sig_cmp(header, 0, 8)) {
        // Some error handling: file is not recognized as a PNG
    }*/

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!image->png_ptr){
        // Some error handling: png_create_read_struct failed
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        // Some error handling: png_create_info_struct failed
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during init_io
    }

    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

    image->number_of_passes = png_set_interlace_handling(image->png_ptr);
    png_read_update_info(image->png_ptr, image->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        cout << "can't read image\n";
        exit(1);
    }

    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    for (y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);
}


void write_png_file(char *file_name, struct Png *image) {
    int x,y;
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp){
        cout << "Can't open file to write. Bye\n";
        exit(OPEN_FILE);
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!image->png_ptr){
        // Some error handling: png_create_write_struct failed
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr){
        // Some error handling: png_create_info_struct failed
    }

    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during init_io
    }

    png_init_io(image->png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during writing header
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during writing bytes
    }

    png_write_image(image->png_ptr, image->row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))){
        // Some error handling: error during end of write
    }

    png_write_end(image->png_ptr, nullptr);

    /* cleanup heap allocation */
    for (y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

    fclose(fp);
}


void process_file(struct Png *image) {
    int x,y;
    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB){
        // Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA){
        // Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    for (y = 0; y < image->height; y++) {
        png_byte *row = image->row_pointers[y];
        for (x = 0; x < image->width; x++) {
            png_byte *ptr = &(row[x * 4]);
            printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
                   x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

            /* set red value to 0 and green value to the blue one */
            ptr[0] = 0;
            ptr[1] = ptr[2];
        }
    }
}
