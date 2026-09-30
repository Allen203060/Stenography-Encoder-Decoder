#include <stdio.h>
#include <stdlib.h>
#include "../include/bmp.h"
#include <string.h>
#include "../include/steganography.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Usage: %s <image.bmp>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    struct bmp_header header;
    fread(&header, sizeof(struct bmp_header), 1, file);
    fclose(file);
    printf("Width: %u\n", header.width);
    printf("Height: %u\n", header.height);
    printf("Pixel Offset: %u\n", header.pixel_offset);


    unsigned char fake_img[100] = {0};
    
    const char *secret_msg = "hello world!@#!@#";
    printf("Original Message: %s\n", secret_msg);

    encode_string(fake_img, secret_msg);

    char extracted_msg[100] = {0};
    decode_string(fake_img, extracted_msg);
    printf("Extracted Message: %s\n", extracted_msg);

    return 0;
}
    