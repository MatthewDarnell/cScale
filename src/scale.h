/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/

#ifndef _SCALE_H
#define _SCALE_H

#include <stdint.h>
#include "util/hex.h"

bool _is_big_endian(void *value);
uint16_t swap_uint16( uint16_t val );
//! Byte swap short
int16_t swap_int16( int16_t val );
//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val );

//! Byte swap int
int32_t swap_int32( int32_t val );



/*
  *
  *   Fixed Width Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#fixed-width-integers
  *
*/

//uint16 00000001  00000001
typedef struct _scale_fixed_int {
   bool is_signed;
   int8_t byte_width;
   uint8_t data[FIXED_INT_MAX_BYTES]; // 00000000  00000000  00000000  00000000
} _scale_fixed_int;

//Output an encoded _scale_fixed_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, _scale_fixed_int *fixed_int_elem);

//These functions encode decimal values into a _scale_fixed_int struct
void _encode_fixed_int8_to_scale(_scale_fixed_int *fixed_int_elem, int8_t data);
void _encode_fixed_uint8_to_scale(_scale_fixed_int *fixed_int_elem, uint8_t data);
void _encode_fixed_int16_to_scale(_scale_fixed_int *fixed_int_elem, int16_t data);
void _encode_fixed_uint16_to_scale(_scale_fixed_int *fixed_int_elem, uint16_t data);
void _encode_fixed_int32_to_scale(_scale_fixed_int *fixed_int_elem, int32_t data);
void _encode_fixed_uint32_to_scale(_scale_fixed_int *fixed_int_elem, uint32_t data);
void _encode_fixed_int64_to_scale(_scale_fixed_int *fixed_int_elem, int64_t data);
void _encode_fixed_uint64_to_scale(_scale_fixed_int *fixed_int_elem, uint64_t data);
//Tries to convert a hex string "0xFFFFFF00" to a _scale_fixed_int
//Returns 0 if successful, -1 otherwise
int8_t _encode_fixed_hex_to_scale(_scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex);

//get a malloc'd (remember to free) hex string of the encoded _scale_fixed_int value
//  return NULL if invalid
char *decode_scale_fixed_to_hex(_scale_fixed_int *fixed_int_elem);

//Decode a fixed int scale element into an integer. User is responsible for passing a valid int type.
int8_t decode_scale_fixed_int(void *output, _scale_fixed_int *fixed_int_elem);




/*
  *
  *   Compact/General Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#compactgeneral-integers
  *
*/


enum _scale_compact_int_mode {
  SINGLE_BYTE = 0, TWO_BYTE = 1, FOUR_BYTE = 2, BIGNUM = 3    // [   last byte   ]
};
                          //  [ ---     data     ---] [upperbits] [mode]
typedef struct _scale_compact_int { // 00000000 00000000 ...    000000      00
  enum _scale_compact_int_mode mode;
  uint8_t mode_upper_bits;
  uint8_t *data;
} _scale_compact_int;

void _cleanup_scale_compact_int(_scale_compact_int *compact);

//Output an encoded _scale_compact_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_compact_int(uint8_t *serialized, uint64_t *serialized_len, _scale_compact_int *compact_int_elem);

//These functions encode decimal values into a _scale_compact_int struct
int8_t _encode_compact_8(_scale_compact_int *compact_int_elem, uint8_t data);
int8_t _encode_compact_16(_scale_compact_int *compact_int_elem, uint16_t data);
int8_t _encode_compact_32(_scale_compact_int *compact_int_elem, uint32_t data);
int8_t _encode_compact_64(_scale_compact_int *compact_int_elem, uint64_t data);
int8_t _encode_compact_128_from_hex(_scale_compact_int *compact_int_elem, char *hex);

int8_t _encode_compact_hex_to_scale(_scale_compact_int *compact_int_elem, const char *hex);
//outputs a hex value, remember to free
char* _decode_compact_to_hex(_scale_compact_int *compact_int_elem);
#endif
