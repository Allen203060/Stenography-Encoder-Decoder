#include <stdio.h>
#include <stdlib.h>
#include "../include/bmp.h"

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


    unsigned char secret = 'A'; // 'A' is 01000001 in binary
    unsigned char pixel[8] = {
        0b11111111, 0b11111111, 0b11111111, 0b11111111,
        0b11111111, 0b11111111, 0b11111111, 0b11111111
    };

    for (int i = 7; i >= 0; i--) {
        int bit_index = 7 - i;

        // Extracting ith bit from secret char
        unsigned char secret_bit = (secret >> bit_index) & 1;

        // clearing the least significant bit of the pixel
        pixel[i] = pixel[i] & 0xFE;

        // setting the cleared bit with the secret bit
        pixel[i] = pixel[i] | secret_bit;

        
    }

    printf("\nTesting LSB Hiding:\n");
    for (int i = 0; i < 8; i++) {
        // We print the last bit of each pixel. 
        // It should spell out 'A' (01000001)
        printf("%d", pixel[i] & 1); 
    }
    printf("\n");





    return 0;
}
    