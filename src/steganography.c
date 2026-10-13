#include "../include/steganography.h"
#include <string.h>
#include <pthread.h>

struct thread_data {
    unsigned char *pixel_chunk;  
    const char *msg_chunk;       
    size_t length;               
    int is_last;                 
};

void *encode_worker(void *arg) {
    // casting the generic void pointer back to our struct pointer
    struct thread_data *data = (struct thread_data *)arg;

    
    for (size_t i = 0; i < data->length; i++) {
        encode_char(data->pixel_chunk + i * 8, data->msg_chunk[i]);
    }

    if (data->is_last) {
        encode_char(data->pixel_chunk + data->length * 8, '\0');
    }

    return NULL;  
}

void encode_string_multithreaded(unsigned char *pixel_array, const char *message, int num_threads) {
    size_t len = strlen(message);
    if (len == 0) {
        encode_char(pixel_array, '\0');
        return;
    }

    // spawn less threads than the number of characters
    if (num_threads > (int)len) {
        num_threads = len;
    }
    if (num_threads <= 0) {
        num_threads = 1;
    }

    pthread_t threads[num_threads];
    struct thread_data args[num_threads];

    size_t chunk_size = len / num_threads;
    size_t remainder = len % num_threads;
    size_t current_msg_offset = 0;
 
    for (int i = 0; i < num_threads; i++) {
        size_t thread_len = chunk_size + (i < (int)remainder ? 1 : 0);

        args[i].msg_chunk = message + current_msg_offset;
        args[i].pixel_chunk = pixel_array + (current_msg_offset * 8);
        args[i].length = thread_len;
        args[i].is_last = (i == num_threads - 1);

        pthread_create(&threads[i], NULL, encode_worker, &args[i]);

        current_msg_offset += thread_len;
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
}



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