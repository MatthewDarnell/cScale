/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#ifndef _HEX_UTIL_H
#define _HEX_UTIL_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


char* cscale_fixed_byte_array_to_hex(uint8_t* data, int8_t byte_width);

bool cscale_hex_digit_to_bin(const char hex, char *out);
bool cscale_is_valid_hex(const char *hex);

size_t cscale_hex_to_data(const char *hex, uint8_t **out);
void cscale_print_hash(uint8_t *s, size_t len);

#endif
