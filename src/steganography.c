#include "../include/steganography.h"
#include <string.h>


void encode_char(unsigned char *pixel_array, char secret) {
    for (int i = 7; i >= 0; i--) {
        
        int bit_index = 7 - i;
        unsigned char secret_bit = (secret >> bit_index) & 1;

        // clearing lsb
        pixel_array[i] = pixel_array[i] & 0xFE;

        // setting lsb
        pixel_array[i] = pixel_array[i] | secret_bit;
    }
}

void encode_string(unsigned char *pixel_array, const char *message) {
    
    int len = strlen(message);
    for (int i = 0; i < len; i++) {
        encode_char(pixel_array + i * 8, message[i]);
    }

    // encode null terminator too '\0'
    encode_char(pixel_array + (len) * 8, '\0');
}

char decode_char(unsigned char *pixel_array) {

    char secret = 0;
    for (int i = 7; i >= 0; i--) {

        int bit_index = 7 - i;
        unsigned char secret_bit = pixel_array[i] & 1;
        secret = secret | (secret_bit << bit_index);
    }

    return secret;
}

void decode_string(unsigned char *pixel_array, char *message) {
    int i = 0;

    while (1) {
        char decoded_char = decode_char(pixel_array + i * 8);
        message[i] = decoded_char;

        if (decoded_char == '\0') {
            break;
        }
        i++;
    }
}