#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "../include/bmp.h"
#include "../include/steganography.h"
#include "../include/tui.h"

static void print_usage(const char *prog) {
    printf("Usage:\n");
    printf("  Interactive TUI: %s\n", prog);
    printf("  Encode CLI:      %s --encode <image.bmp> <\"message\"> [threads]\n", prog);
    printf("  Decode CLI:      %s --decode <image.bmp>\n", prog);
}

int main(int argc, char *argv[]) {
     
    if (argc == 1) {
        tui_run();
        return 0;
    }

 
    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    // CLI: Decode
    if (strcmp(argv[1], "--decode") == 0) {
        if (argc < 3) {
            printf("Error: Missing image file path.\n");
            print_usage(argv[0]);
            return 1;
        }

        int fd = open(argv[2], O_RDONLY);
        if (fd < 0) {
            perror("Error opening image");
            return 1;
        }

        struct stat sb;
        if (fstat(fd, &sb) < 0) {
            perror("Error getting file size");
            close(fd);
            return 1;
        }

        unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
        if (file_data == MAP_FAILED) {
            perror("mmap failed");
            close(fd);
            return 1;
        }

        struct bmp_header *header = (struct bmp_header *)file_data;
        unsigned char *pixels = file_data + header->pixel_offset;

        char extracted_msg[4096] = {0};
        decode_string(pixels, extracted_msg);

        printf("Extracted Message: %s\n", extracted_msg);

        munmap(file_data, sb.st_size);
        close(fd);
        return 0;
    }

    // CLI: Encode
    if (strcmp(argv[1], "--encode") == 0) {
        if (argc < 4) {
            printf("Error: Missing image file or message.\n");
            print_usage(argv[0]);
            return 1;
        }

        const char *filename = argv[2];
        const char *secret_msg = argv[3];
        int threads = (argc >= 5) ? atoi(argv[4]) : 4;
        if (threads <= 0) threads = 1;

        int fd = open(filename, O_RDWR);
        if (fd < 0) {
            perror("Error opening image");
            return 1;
        }

        struct stat sb;
        if (fstat(fd, &sb) < 0) {
            perror("Error getting file size");
            close(fd);
            return 1;
        }

        unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (file_data == MAP_FAILED) {
            perror("mmap failed");
            close(fd);
            return 1;
        }

        struct bmp_header *header = (struct bmp_header *)file_data;
        size_t available_bytes = sb.st_size - header->pixel_offset;
        size_t required_bytes = (strlen(secret_msg) + 1) * 8;

        if (required_bytes > available_bytes) {
            printf("Error: Image is too small to hold the message!\n");
            munmap(file_data, sb.st_size);
            close(fd);
            return 1;
        }

        unsigned char *pixels = file_data + header->pixel_offset;
        encode_string_multithreaded(pixels, secret_msg, threads);
        printf("Successfully hidden message in %s using %d threads!\n", filename, threads);

        munmap(file_data, sb.st_size);
        close(fd);
        return 0;
    }

    printf("Unknown option: %s\n", argv[1]);
    print_usage(argv[0]);
    return 1;
}
