/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#ifndef _HEX_UTIL_H
#define _HEX_UTIL_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#define FIXED_INT_MAX_BYTES 8



char* _fixed_byte_array_to_hex(uint8_t* data, int8_t byte_width);
char* _byte_array_to_hex(uint8_t* data, size_t len);

bool hex_digit_to_bin(const char hex, char *out);
bool is_valid_hex(const char *hex);

size_t hex_to_data(const char *hex, uint8_t **out);
void print_hash(uint8_t *s, size_t len);

#endif
