#ifndef BMP_H // if struct is not defined only then define else skip
#define BMP_H

#include <stdint.h>

// When reading the header we need to capture the structure of the header perfectly so that we can get the useful data in byte order, useful info -> height, width, pixel_offset
struct bmp_header {
    uint16_t signature;       // 'BM' (2 bytes)
    uint32_t file_size;       // Total size of the file (4 bytes)
    uint16_t reserved1;       // Unused (2 bytes)
    uint16_t reserved2;       // Unused (2 bytes)
    uint32_t pixel_offset;    // Byte where the pixel data begins (4 bytes)
    
    // Info Header Section
    uint32_t header_size;     // Size of this info header (4 bytes)
    uint32_t width;           // Image width in pixels (4 bytes)
    uint32_t height;          // Image height in pixels (4 bytes)
    uint16_t planes;          // Number of color planes (2 bytes)
    uint16_t bpp;             // Bits per pixel, usually 24 (2 bytes)
    uint32_t compression;     // Compression type (4 bytes)
    uint32_t image_size;      // Image size in bytes (4 bytes)
    uint32_t x_ppm;           // X pixels per meter (4 bytes) for printing
    uint32_t y_ppm;           // Y pixels per meter (4 bytes)
    uint32_t total_colors;    // Total colors (4 bytes)
    uint32_t important_colors;// Important colors (4 bytes)
} __attribute__((packed));

#endif // BMP_H
