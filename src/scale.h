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


/*
  *
  *   Boolean: https://substrate.dev/docs/en/knowledgebase/advanced/codec#boolean
  *
*/
typedef struct _scale_boolean {
  bool value;
} _scale_boolean;
//Output 0x01 or 0x00 SCALE value for this bool
void serialize_boolean(uint8_t *seralized, _scale_boolean *boolean_elem);
//Encode a bool value into a boolean element
void _encode_boolean(_scale_boolean *boolean_elem, bool value);
//Return a bool value contained in the boolean element
bool _decode_boolean(_scale_boolean *boolean_elem);

//Consume a scale-encoded hex string and populate a _scale_boolean struct
int8_t _encode_boolean_from_hex(_scale_boolean *boolean_elem, char *boolean_hex_value);
//outputs a hex scale string, remember to free
char *_decode_boolean_to_hex(_scale_boolean *boolean_elem);


/*
  *
  *   Options: https://substrate.dev/docs/en/knowledgebase/advanced/codec#options
  *
*/
typedef enum _option_type { FIXED_INT, COMPACT_INT, BOOLEAN, STRUCT_FIXED_INT } _option_type;

typedef struct _scale_struct {
  void *data;
  size_t num_elements;
} _scale_struct;

typedef struct _option_value {
  enum _option_type type;
  union {
    _scale_fixed_int _fixed_int;
    _scale_compact_int _compact_int;
    _scale_boolean _boolean;
    _scale_struct _struct;
  };
} _option_value;
typedef struct _scale_option {
  _scale_boolean option;
  _option_value value;
} _scale_option;

//Encodes a fixed_int scale struct into an option. Pass NULL as fixed int to make option None
int8_t _encode_option_fixed_int(_scale_option *option, _scale_fixed_int *fixed_int);

//Encodes a compact_int scale struct into an option. Pass NULL as fixed int to make option None
//This operation moves compact_int->data array, so remember to release this option and no longer reference
//the compact_int->data
int8_t _encode_option_compact_int(_scale_option *option, _scale_compact_int *compact_int);
//Encodes a boolean scale struct into an option. Pass NULL as fixed int to make option None
int8_t _encode_option_boolean(_scale_option *option, _scale_boolean *boolean);

//Get hex String of this option. Remember to free
char *_decode_option_to_hex(_scale_option *option);

//Get a scale serialized array of bytes with length serialized_len of the option
int8_t serialize_option(uint8_t *serialized, size_t *serialized_len, _scale_option *option);


/*
  *
  *   Enumerations: https://substrate.dev/docs/en/knowledgebase/advanced/codec#enumerations-tagged-unions
  *
*/
typedef struct _scale_enum_type {
  size_t num_elements;
  char *keys[256];
  char *values[256];
} _scale_enum_type;
int8_t _encode_scale_enum_type(_scale_enum_type *enum_type, size_t num_elements, char *keys[], char *values[]);
void _print_scale_enum_type(_scale_enum_type *enum_type);
void _cleanup_scale_enum_type(_scale_enum_type *enum_type);


typedef struct _scale_encoded_struct {
  void* data; //pointer to user-defined struct, compact function must cast
  void (*fixed)(void *compact_int_elem, void *data);  //Function pointer that consumes a user defined struct, data, and outputs a custom fixed scale
} _scale_encoded_struct;


typedef struct _scale_enumeration {
  _option_value value;
  _scale_enum_type enum_types;
  char *key;
} _scale_enumeration;

char* _serialize_enumeration_to_hex(_scale_enumeration *enumeration);
int8_t _serialize_enumeration(uint8_t *serialized, size_t *serialized_len, _scale_enumeration *enumeration);
int8_t _encode_enumeration(_scale_enumeration *enumeration, _scale_enum_type *enum_types, const char *key, void* data);
#endif
