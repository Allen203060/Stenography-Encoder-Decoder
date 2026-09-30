#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

void encode_char(unsigned char *pixel_array, char secret);
void encode_string(unsigned char *pixel_array, const char *message);

char decode_char(unsigned char *pixel_array);
void decode_string(unsigned char *pixel_array, char *message);

#endif