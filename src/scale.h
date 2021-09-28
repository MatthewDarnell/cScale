/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/

#ifndef _SCALE_H
#define _SCALE_H

#include <stdint.h>
#include <wchar.h>
#include "util/hex.h"

bool _is_big_endian(void *value);
uint16_t swap_uint16( uint16_t val );
//! Byte swap short
int16_t swap_int16( int16_t val );
//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val );

//! Byte swap int
int32_t swap_int32( int32_t val );



typedef enum _scale_type { FIXED_INT, COMPACT_INT, BOOLEAN, OPTION, ENUM, VECTOR, STRING, STRUCT  } _scale_type;

/*
  *
  *   Fixed Width Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#fixed-width-integers
  *
*/

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
int8_t _encode_fixed_u128_to_scale(_scale_fixed_int *fixed_int_elem, char *hex);
#define _encode_fixed_int_to_scale(elem, value) \
  _Generic( \
    (value), \
    int8_t:  _encode_fixed_int8_to_scale, \
    uint8_t:  _encode_fixed_uint8_to_scale, \
    int16_t:  _encode_fixed_int16_to_scale, \
    uint16_t:  _encode_fixed_uint16_to_scale, \
    int32_t:  _encode_fixed_int32_to_scale, \
    uint32_t:  _encode_fixed_uint32_to_scale, \
    int64_t:  _encode_fixed_int64_to_scale, \
    uint64_t:  _encode_fixed_uint64_to_scale \
  ) (elem, value)
//Tries to convert a hex string "0xFFFFFF00" to a _scale_fixed_int
//Returns 0 if successful, -1 otherwise
int8_t _encode_fixed_hex_to_scale(_scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex);

//get a malloc'd (remember to free) hex string of the encoded _scale_fixed_int value
//  return NULL if invalid
char *decode_scale_fixed_to_hex(_scale_fixed_int *fixed_int_elem);

//Decode a fixed int scale element into an integer. User is responsible for passing a valid int type.
int8_t decode_scale_fixed_int(void *output, _scale_fixed_int *fixed_int_elem);

void deserialize_fixed_int(void *output, uint8_t *bytes, size_t len, bool is_signed);



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

#define _encode_compact(elem, value) \
  _Generic( \
    (value), \
    uint8_t:  _encode_compact_8, \
    uint16_t:  _encode_compact_16, \
    uint32_t:  _encode_compact_32, \
    uint64_t:  _encode_compact_64 \
  ) (elem, value)


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
void deserialize_boolean(_scale_boolean *boolean_elem, uint8_t* serialized);
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

typedef struct _option_value {
  enum _scale_type type;
  union {
    _scale_fixed_int _fixed_int;
    _scale_compact_int _compact_int;
    _scale_boolean _boolean;
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



/*
  *
  *   Vectors https://substrate.dev/docs/en/knowledgebase/advanced/codec#vectors-lists-series-sets
  *     Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
typedef struct _scale_vector {
  uint8_t *data;
  uint64_t data_len;
  _scale_compact_int prefix_num_elements;
} _scale_vector;

void serialize_vector(uint8_t *serialized, size_t *serialized_len, _scale_vector *vec);
void deserialize_vector(_scale_vector *vec, uint8_t *serialized, size_t *serialized_len);
void push_vector(_scale_vector *vec, uint8_t *bytes, size_t len);
void cleanup_vector(_scale_vector *vec);

/*
  *
  *   Strings: https://substrate.dev/docs/en/knowledgebase/advanced/codec#strings
  *
*/
void inline create_string(_scale_vector *vec, unsigned char *string, size_t len) {
  size_t i;
  for(i=0; i < len; i++) {
    push_vector(vec, &string[i], 1);
  }
}
void inline serialize_string(uint8_t *serialized, size_t *serialized_len, _scale_vector *vec) {
  serialize_vector(serialized, serialized_len, vec);
}
void inline deserialize_string(_scale_vector *vec, uint8_t *serialized, size_t *serialized_len) {
  deserialize_vector(vec, serialized, serialized_len);
}
void inline cleanup_string(_scale_vector *vec) {
  cleanup_vector(vec);
}
/*
  *
  *   Data Structures: https://substrate.dev/docs/en/knowledgebase/advanced/codec#data-structures
  *
*/
typedef struct _scale_structure {
  void (*serialize)(uint8_t* seralized, size_t *bytes, void *structure);  //Function pointer that consumes a user defined struct, data, and outputs a custom fixed scale
  void (*deserialize)(void *structure_out, uint8_t *bytes, size_t len);  //Consumes scale-serialized bytes and outputs a struct
} _scale_structure;


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

typedef struct _scale_enumeration {
  _option_value value;
  _scale_enum_type enum_types;
  char *key;
} _scale_enumeration;

char* _serialize_enumeration_to_hex(_scale_enumeration *enumeration);
int8_t _serialize_enumeration(uint8_t *serialized, size_t *serialized_len, _scale_enumeration *enumeration);

void _decode_enumeration(uint8_t *bytes, uint16_t *enum_type_index, _scale_enum_type *enum_types, uint8_t *serialized, size_t *serialized_len);
void _encode_enumeration(uint8_t *bytes, _scale_enum_type *enum_types, const char *key, uint8_t *serialized, size_t *serialized_len);





/*
  *
  *   Tuples: https://substrate.dev/docs/en/knowledgebase/advanced/codec#tuples
  *
*/
int8_t _serialize_as_tuple(uint8_t *bytes, size_t *len, _scale_type a, void *data_a, _scale_type b, void *data_b);
//to deserialize, simply check values of a and b and call appropriate deserialization function
#endif
