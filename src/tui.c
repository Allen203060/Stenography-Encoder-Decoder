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


static void read_input(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

 
static void pause_for_user(void) {
    printf(C_GRAY "\n  Press [Enter] to continue..." C_RESET);
    char buf[16];
    fgets(buf, sizeof(buf), stdin);
}

 
static void print_banner(void) {
    printf(C_CYAN C_BOLD);
    printf("\n");
    printf("  ╔══════════════════════════════════════════════════════════════════════╗\n");
    printf("  ║       🕵️   MULTITHREADED STEGANOGRAPHY ENGINE v1.0                 ║\n");
    printf("  ║       Virtual Memory (mmap) + POSIX Threads (pthreads)               ║\n");
    printf("  ╚══════════════════════════════════════════════════════════════════════╝\n");
    printf(C_RESET);
}

 
static void do_encode(void) {
    char filename[256];
    char message[4096];
    char thread_str[16];

    printf(C_BOLD C_GREEN "\n  [ ENCODE MODE ]\n" C_RESET);
    read_input(C_CYAN "  Enter BMP image path [default: test.bmp]: " C_RESET, filename, sizeof(filename));
    if (strlen(filename) == 0) {
        strcpy(filename, "test.bmp");
    }

    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        perror(C_RED "  [!] Error opening image" C_RESET);
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror(C_RED "  [!] Error getting file size" C_RESET);
        close(fd);
        return;
    }

    unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror(C_RED "  [!] mmap failed" C_RESET);
        close(fd);
        return;
    }

    struct bmp_header *header = (struct bmp_header *)file_data;
    if (header->signature != 0x4D42) {  
        printf(C_RED "  [!] Error: File is not a valid BMP image!\n" C_RESET);
        munmap(file_data, sb.st_size);
        close(fd);
        return;
    }

    size_t available_bytes = sb.st_size - header->pixel_offset;
    size_t max_capacity = available_bytes / 8;  
    if (max_capacity > 0) max_capacity--;     

    printf(C_GRAY "  Image Size: %ld bytes | Max Capacity: %zu characters\n" C_RESET, sb.st_size, max_capacity);

    read_input(C_CYAN "  Enter secret message to hide: " C_RESET, message, sizeof(message));
    if (strlen(message) == 0) {
        printf(C_YELLOW "  [!] Aborted: Message was empty.\n" C_RESET);
        munmap(file_data, sb.st_size);
        close(fd);
        return;
    }

    if (strlen(message) > max_capacity) {
        printf(C_RED "  [!] Error: Message is too large for this image (max %zu characters)!\n" C_RESET, max_capacity);
        munmap(file_data, sb.st_size);
        close(fd);
        return;
    }

    read_input(C_CYAN "  Number of worker threads [default: 4]: " C_RESET, thread_str, sizeof(thread_str));
    int threads = (strlen(thread_str) > 0) ? atoi(thread_str) : 4;
    if (threads <= 0) threads = 1;

    printf(C_YELLOW "  [*] Encoding %zu characters using %d threads...\n" C_RESET, strlen(message), threads);
    unsigned char *pixels = file_data + header->pixel_offset;
    encode_string_multithreaded(pixels, message, threads);

    munmap(file_data, sb.st_size);
    close(fd);

    printf(C_GREEN C_BOLD "\n  [✓] SUCCESS: Secret message hidden directly into '%s'!\n" C_RESET, filename);
}

 
static void do_decode(void) {
    char filename[256];

    printf(C_BOLD C_YELLOW "\n  [ DECODE MODE ]\n" C_RESET);
    read_input(C_CYAN "  Enter BMP image path [default: test.bmp]: " C_RESET, filename, sizeof(filename));
    if (strlen(filename) == 0) {
        strcpy(filename, "test.bmp");
    }

 
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(C_RED "  [!] Error opening image" C_RESET);
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror(C_RED "  [!] Error getting file size" C_RESET);
        close(fd);
        return;
    }

    unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror(C_RED "  [!] mmap failed" C_RESET);
        close(fd);
        return;
    }

    struct bmp_header *header = (struct bmp_header *)file_data;
    if (header->signature != 0x4D42) {
        printf(C_RED "  [!] Error: File is not a valid BMP image!\n" C_RESET);
        munmap(file_data, sb.st_size);
        close(fd);
        return;
    }

    unsigned char *pixels = file_data + header->pixel_offset;
    char extracted_msg[4096] = {0};
    decode_string(pixels, extracted_msg);

    munmap(file_data, sb.st_size);
    close(fd);

    printf("\n  ┌─────────────────────────────────────────────────────────────┐\n");
    printf("  │ " C_GREEN C_BOLD "EXTRACTED SECRET MESSAGE:" C_RESET "                                 │\n");
    printf("  ├─────────────────────────────────────────────────────────────┤\n");
    printf("  │ %s%-59s%s │\n", C_WHITE C_BOLD, extracted_msg, C_RESET);
    printf("  └─────────────────────────────────────────────────────────────┘\n");
}


static void do_inspect(void) {
    char filename[256];

    printf(C_BOLD C_BLUE "\n  [ INSPECT BMP HEADER & CAPACITY ]\n" C_RESET);
    read_input(C_CYAN "  Enter BMP image path [default: test.bmp]: " C_RESET, filename, sizeof(filename));
    if (strlen(filename) == 0) {
        strcpy(filename, "test.bmp");
    }

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(C_RED "  [!] Error opening image" C_RESET);
        return;
    }

    struct stat sb;
    if (fstat(fd, &sb) < 0) {
        perror(C_RED "  [!] Error getting file size" C_RESET);
        close(fd);
        return;
    }

    unsigned char *file_data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_data == MAP_FAILED) {
        perror(C_RED "  [!] mmap failed" C_RESET);
        close(fd);
        return;
    }

    struct bmp_header *header = (struct bmp_header *)file_data;
    size_t available_bytes = sb.st_size - header->pixel_offset;
    size_t max_chars = available_bytes / 8;
    if (max_chars > 0) max_chars--;

    printf("\n  " C_BOLD "Image Metadata for: " C_WHITE "%s\n" C_RESET, filename);
    printf("  ├─ File Size:         " C_GREEN "%ld bytes (%.2f MB)\n" C_RESET, sb.st_size, (double)sb.st_size / (1024 * 1024));
    printf("  ├─ Dimensions:        " C_GREEN "%u x %u pixels\n" C_RESET, header->width, header->height);
    printf("  ├─ Color Depth:       " C_GREEN "%u bits per pixel\n" C_RESET, header->bpp);
    printf("  ├─ Pixel Data Offset: " C_GREEN "%u bytes\n" C_RESET, header->pixel_offset);
    printf("  └─ Max Text Capacity: " C_YELLOW C_BOLD "%zu characters (~%.2f KB)\n" C_RESET, max_chars, (double)max_chars / 1024);

    munmap(file_data, sb.st_size);
    close(fd);
}

 
void tui_run(void) {
    char choice[16];

    while (1) {
        print_banner();
        printf(C_BOLD "\n  [ MAIN MENU ]\n" C_RESET);
        printf("  " C_GREEN "[1]" C_RESET " 🔒 Encode Secret Message into BMP\n");
        printf("  " C_YELLOW "[2]" C_RESET " 🔓 Decode Secret Message from BMP\n");
        printf("  " C_BLUE "[3]" C_RESET " 📊 Inspect BMP Header & Capacity\n");
        printf("  " C_RED "[4]" C_RESET " 🚪 Exit\n\n");

        read_input(C_CYAN "  Select an option [1-4]: " C_RESET, choice, sizeof(choice));

        if (strcmp(choice, "1") == 0) {
            do_encode();
            pause_for_user();
        } else if (strcmp(choice, "2") == 0) {
            do_decode();
            pause_for_user();
        } else if (strcmp(choice, "3") == 0) {
            do_inspect();
            pause_for_user();
        } else if (strcmp(choice, "4") == 0) {
            printf(C_GREEN "\n  Exiting Steganography Engine. Stay safe!\n\n" C_RESET);
            break;
        } else {
            printf(C_RED "  [!] Invalid selection. Please enter 1, 2, 3, or 4.\n" C_RESET);
            pause_for_user();
        }
    }
}
