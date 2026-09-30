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
    return 0;
}
    