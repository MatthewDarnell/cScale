/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/

#ifndef _SCALE_H
#define _SCALE_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#define FIXED_INT_MAX_BYTES 16

bool is_big_endian(void *value);
uint16_t swap_uint16( uint16_t val );
//! Byte swap short
int16_t swap_int16( int16_t val );
//! Byte swap unsigned int
uint32_t swap_uint32( uint32_t val );
//! Byte swap int
int32_t swap_int32( int32_t val );



typedef enum scale_type { FIXED_INT, COMPACT_INT, BOOLEAN, OPTION, ENUM, VECTOR, STRING, STRUCT  } scale_type;

/*
  *
  *   Fixed Width Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#fixed-width-integers
  *
*/

typedef struct {
   bool is_signed;
   int8_t byte_width;
   uint8_t data[FIXED_INT_MAX_BYTES]; // 00000000  00000000  00000000  00000000
} scale_fixed_int;

//Output an encoded scale_fixed_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, scale_fixed_int *fixed_int_elem);

//These functions encode decimal values into a scale_fixed_int struct
void encode_fixed_int8_to_scale(scale_fixed_int *fixed_int_elem, int8_t data);
void encode_fixed_uint8_to_scale(scale_fixed_int *fixed_int_elem, uint8_t data);
void encode_fixed_int16_to_scale(scale_fixed_int *fixed_int_elem, int16_t data);
void encode_fixed_uint16_to_scale(scale_fixed_int *fixed_int_elem, uint16_t data);
void encode_fixed_int32_to_scale(scale_fixed_int *fixed_int_elem, int32_t data);
void encode_fixed_uint32_to_scale(scale_fixed_int *fixed_int_elem, uint32_t data);
void encode_fixed_int64_to_scale(scale_fixed_int *fixed_int_elem, int64_t data);
void encode_fixed_uint64_to_scale(scale_fixed_int *fixed_int_elem, uint64_t data);
int8_t encode_fixed_u128_to_scale(scale_fixed_int *fixed_int_elem, char *hex);
#define encode_fixed_int_to_scale(elem, value) \
  _Generic( \
    (value), \
    int8_t:  encode_fixed_int8_to_scale, \
    uint8_t:  encode_fixed_uint8_to_scale, \
    int16_t:  encode_fixed_int16_to_scale, \
    uint16_t:  encode_fixed_uint16_to_scale, \
    int32_t:  encode_fixed_int32_to_scale, \
    uint32_t:  encode_fixed_uint32_to_scale, \
    int64_t:  encode_fixed_int64_to_scale, \
    uint64_t:  encode_fixed_uint64_to_scale \
  ) (elem, value)

//Swaps Little Endian -encoded U128 hex into Big endian, pass this to encode_fixed_u128_to_scale
//Returns 0 if ok, -1 if got 00000 le string
int8_t swap_u128_le_to_be(char *be_out, char *le);

//Tries to convert a hex string "0xFFFFFF00" to a scale_fixed_int
//Returns 0 if successful, -1 otherwise
int8_t encode_fixed_hex_to_scale(scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex);

//get a malloc'd (remember to free) hex string of the encoded scale_fixed_int value
//  return NULL if invalid
char *decode_scale_fixed_to_hex(scale_fixed_int *fixed_int_elem);

//Decode a fixed int scale element into an integer. User is responsible for passing a valid int type.
int8_t decode_scale_fixed_int(void *output, scale_fixed_int *fixed_int_elem);

void deserialize_fixed_int(void *output, uint8_t *bytes, size_t len, bool is_signed);



/*
  *
  *   Compact/General Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#compactgeneral-integers
  *
*/


enum scale_compact_int_mode {
  SINGLE_BYTE = 0, TWO_BYTE = 1, FOUR_BYTE = 2, BIGNUM = 3    // [   last byte   ]
};
                          //  [ ---     data     ---] [upperbits] [mode]
typedef struct { // 00000000 00000000 ...    000000      00
  enum scale_compact_int_mode mode;
  uint8_t mode_upper_bits;
  uint8_t *data;
} scale_compact_int;

void cleanup_scale_compact_int(scale_compact_int *compact);

//Output an encoded scale_compact_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_compact_int(uint8_t *serialized, uint64_t *serialized_len, scale_compact_int *compact_int_elem);

//These functions encode decimal values into a scale_compact_int struct
int8_t encode_compact_8(scale_compact_int *compact_int_elem, uint8_t data);
int8_t encode_compact_16(scale_compact_int *compact_int_elem, uint16_t data);
int8_t encode_compact_32(scale_compact_int *compact_int_elem, uint32_t data);
int8_t encode_compact_64(scale_compact_int *compact_int_elem, uint64_t data);
int8_t encode_compact_128_from_hex(scale_compact_int *compact_int_elem, char *hex);

#define encode_compact(elem, value) \
  _Generic( \
    (value), \
    uint8_t:  encode_compact_8, \
    uint16_t:  encode_compact_16, \
    uint32_t:  encode_compact_32, \
    uint64_t:  encode_compact_64 \
  ) (elem, value)


int8_t encode_compact_hex_to_scale(scale_compact_int *compact_int_elem, const char *hex);
//outputs a hex value, remember to free
char* decode_compact_to_hex(scale_compact_int *compact_int_elem);


/*
  *
  *   Boolean: https://substrate.dev/docs/en/knowledgebase/advanced/codec#boolean
  *
*/
typedef struct {
  bool value;
} scale_boolean;
//Output 0x01 or 0x00 SCALE value for this bool
void serialize_boolean(uint8_t *seralized, scale_boolean *boolean_elem);
void deserialize_boolean(scale_boolean *boolean_elem, uint8_t* serialized);
//Encode a bool value into a boolean element
void encode_boolean(scale_boolean *boolean_elem, bool value);
//Return a bool value contained in the boolean element
bool decode_boolean(scale_boolean *boolean_elem);
//Consume a scale-encoded hex string and populate a scale_boolean struct
int8_t encode_boolean_from_hex(scale_boolean *boolean_elem, char *boolean_hex_value);
//outputs a hex scale string, remember to free
char *decode_boolean_to_hex(scale_boolean *boolean_elem);


/*
  *
  *   Options: https://substrate.dev/docs/en/knowledgebase/advanced/codec#options
  *
*/

typedef struct {
  enum scale_type type;
  union {
    scale_fixed_int _fixed_int;
    scale_compact_int _compact_int;
    scale_boolean _boolean;
  };
} option_value;
typedef struct {
  scale_boolean option;
  option_value value;
} scale_option;

//Encodes a fixed_int scale struct into an option. Pass NULL as fixed int to make option None
int8_t encode_option_fixed_int(scale_option *option, scale_fixed_int *fixed_int);

//Encodes a compact_int scale struct into an option. Pass NULL as fixed int to make option None
//This operation moves compact_int->data array, so remember to release this option and no longer reference
//the compact_int->data
int8_t encode_option_compact_int(scale_option *option, scale_compact_int *compact_int);
//Encodes a boolean scale struct into an option. Pass NULL as fixed int to make option None
int8_t encode_option_boolean(scale_option *option, scale_boolean *boolean);

//Get hex String of this option. Remember to free
char *decode_option_to_hex(scale_option *option);



/*
  *
  *   Vectors https://substrate.dev/docs/en/knowledgebase/advanced/codec#vectors-lists-series-sets
  *     Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
typedef struct {
  uint8_t *data;
  uint64_t data_len;
  scale_compact_int prefix_num_elements;
} scale_vector;

void serialize_vector(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
void deserialize_vector(scale_vector *vec, uint8_t *serialized, size_t *serialized_len);
void push_vector(scale_vector *vec, uint8_t *bytes, size_t len);
void cleanup_vector(scale_vector *vec);

/*
  *
  *   Strings: https://substrate.dev/docs/en/knowledgebase/advanced/codec#strings
  *
*/
void inline create_string(scale_vector *vec, unsigned char *string, size_t len) {
  size_t i;
  for(i=0; i < len; i++) {
    push_vector(vec, &string[i], 1);
  }
}
void inline serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec) {
  serialize_vector(serialized, serialized_len, vec);
}
void inline deserialize_string(scale_vector *vec, uint8_t *serialized, size_t *serialized_len) {
  deserialize_vector(vec, serialized, serialized_len);
}
void inline cleanup_string(scale_vector *vec) {
  cleanup_vector(vec);
}
/*
  *
  *   Data Structures: https://substrate.dev/docs/en/knowledgebase/advanced/codec#data-structures
  *
*/
typedef struct {
  void (*serialize)(uint8_t* seralized, size_t *bytes, void *structure);  //Function pointer that consumes a user defined struct, data, and outputs a custom fixed scale
  void (*deserialize)(void *structure_out, uint8_t *bytes, size_t len);  //Consumes scale-serialized bytes and outputs a struct
} scale_structure;


/*
  *
  *   Enumerations: https://substrate.dev/docs/en/knowledgebase/advanced/codec#enumerations-tagged-unions
  *
*/
typedef struct {
  size_t num_elements;
  char *keys[256];
  char *values[256];
} scale_enum_type;
int8_t encode_scale_enum_type(scale_enum_type *enum_type, size_t num_elements, char *keys[], char *values[]);
void print_scale_enum_type(scale_enum_type *enum_type);
void cleanup_scale_enum_type(scale_enum_type *enum_type);

typedef struct {
  option_value value;
  scale_enum_type enum_types;
  char *key;
} scale_enumeration;

char* serialize_enumeration_to_hex(scale_enumeration *enumeration);
int8_t serialize_enumeration(uint8_t *serialized, size_t *serialized_len, scale_enumeration *enumeration);

void decode_enumeration(uint8_t *bytes, uint16_t *enum_type_index, scale_enum_type *enum_types, uint8_t *serialized, size_t *serialized_len);
void encode_enumeration(uint8_t *bytes, scale_enum_type *enum_types, const char *key, uint8_t *serialized, size_t *serialized_len);





/*
  *
  *   Tuples: https://substrate.dev/docs/en/knowledgebase/advanced/codec#tuples
  *
*/
int8_t serialize_as_tuple(uint8_t *bytes, size_t *len, scale_type a, void *data_a, scale_type b, void *data_b);
//to deserialize, simply check values of a and b and call appropriate deserialization function
#endif
