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

typedef enum scale_type { FIXED_INT, COMPACT_INT, BOOLEAN, OPTION, ENUM, VECTOR, STRING, STRUCT  } scale_type;

/*
  *
  *   Fixed Width Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#fixed-width-integers
  *
*/

//Struct Containing A SCALE Fixed Integer Value.
typedef struct {
   bool is_signed;
   int8_t byte_width;
   uint8_t data[FIXED_INT_MAX_BYTES]; // 00000000  00000000  00000000  00000000
} scale_fixed_int;

//These functions encode integer values into a scale_fixed_int struct
void encode_int8_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int8_t data);
void encode_uint8_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint8_t data);
void encode_int16_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int16_t data);
void encode_uint16_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint16_t data);
void encode_int32_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int32_t data);
void encode_uint32_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint32_t data);
void encode_int64_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int64_t data);
void encode_uint64_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint64_t data);

//Encode a valid U128 Hex String To a Fixed Int SCALE structure
//TODO: add support for uint128_t gcc extension (problem is literal int values can be too long) or add 3rd party library
int8_t encode_u128_string_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, char *hex);

//C11 Generic Helper to infer int type
#define encode_int_to_fixed_int_scale(elem, value) \
  _Generic( \
    (value), \
    int8_t:  encode_int8_to_fixed_int_scale, \
    uint8_t:  encode_uint8_to_fixed_int_scale, \
    int16_t:  encode_int16_to_fixed_int_scale, \
    uint16_t:  encode_uint16_to_fixed_int_scale, \
    int32_t:  encode_int32_to_fixed_int_scale, \
    uint32_t:  encode_uint32_to_fixed_int_scale, \
    int64_t:  encode_int64_to_fixed_int_scale, \
    uint64_t:  encode_uint64_to_fixed_int_scale \
  ) (elem, value)


//Helper Function to Swap Little Endian-encoded U128 hex into Big endian, pass this to encode_u128_string_to_fixed_int_scale
//Returns 0 if ok, -1 if got 00000 le string
int8_t swap_u128_le_to_be(char *be_out, char *le);

//Tries to convert a fixed-int hex string "0xFFFFFF00" to a scale_fixed_int Structure
//Returns 0 if successful, -1 otherwise
int8_t encode_fixed_hex_to_scale(scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex);


//Output an encoded scale_fixed_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, scale_fixed_int *fixed_int_elem);



//Returns a malloc'd (remember to free) hex string of the encoded scale_fixed_int value
//Returns NULL if invalid
char *decode_scale_fixed_to_hex(scale_fixed_int *fixed_int_elem);

//Decodes a fixed int scale element into an integer. User is responsible for passing a valid int type. Max currently is uint64_t
//Returns 0 on success, -1 otherwise
int8_t decode_scale_fixed_int(void *output, scale_fixed_int *fixed_int_elem);

//Decodes a serialized stream of fixed-int SCALE bytes into an integer. User is responsible for passing a valid int type. Max currently is uint64_t
//Returns 0 on success, -1 otherwise
int8_t deserialize_fixed_int(void *output, uint8_t *bytes, size_t len, bool is_signed);



/*
  *
  *   Compact/General Int: https://substrate.dev/docs/en/knowledgebase/advanced/codec#compactgeneral-integers
  *
*/

//Enum Containing A Compact/General Integer Mode
enum scale_compact_int_mode {
  SCALE_COMPACT_SINGLE_BYTE = 0, SCALE_COMPACT_TWO_BYTE = 1, SCALE_COMPACT_FOUR_BYTE = 2, SCALE_COMPACT_BIGNUM = 3
};


//Struct Containing A SCALE Compact/General Integer Value.
typedef struct {
  enum scale_compact_int_mode mode;
  uint8_t mode_upper_bits;
  uint8_t *data;
} scale_compact_int;


//These functions encode integer values into a scale_compact_int struct
//encode_uint64_to_fixed_int_scale
int8_t encode_uint8_to_compact_int_scale(scale_compact_int *compact_int_elem, uint8_t data);
int8_t encode_uint16_to_compact_int_scale(scale_compact_int *compact_int_elem, uint16_t data);
int8_t encode_uint32_to_compact_int_scale(scale_compact_int *compact_int_elem, uint32_t data);
int8_t encode_uint64_to_compact_int_scale(scale_compact_int *compact_int_elem, uint64_t data);

//Encode a valid U128 Hex String To a Compact Int SCALE structure
int8_t encode_u128_string_to_compact_int_scale(scale_compact_int *compact_int_elem, char *hex);

//C11 Generic Helper to infer int type
#define encode_compact(elem, value) \
  _Generic( \
    (value), \
    uint8_t:  encode_uint8_to_compact_int_scale, \
    uint16_t:  encode_uint16_to_compact_int_scale, \
    uint32_t:  encode_uint32_to_compact_int_scale, \
    uint64_t:  encode_uint64_to_compact_int_scale \
  ) (elem, value)


//Encode a valid Hex encoded Compact/General Int string into a scale_compact_int Structure
//Returns 0 on success, -1 otherwise
int8_t encode_compact_hex_to_scale(scale_compact_int *compact_int_elem, const char *hex);


//Frees malloc'd uint8_t *data pointer
void cleanup_scale_compact_int(scale_compact_int *compact);

//Output an encoded scale_compact_int struct to a serialized SCALE array of bytes.
//Byte length placed into serialized_len
int8_t serialize_compact_int(uint8_t *serialized, uint64_t *serialized_len, scale_compact_int *compact_int_elem);

//Returns a malloc'd hex value (remember to free) of the integer representation of this scale_compact_int Structure
//Returns NULL if unsuccessful
//You can read this value into an integer using strtoull
char* decode_compact_to_hex(scale_compact_int *compact_int_elem);

//Attempt to decode and read a scale_compact_int elem structure into a u64
//Returns 0 if it fails
uint64_t decode_compact_to_u64(scale_compact_int *compact_int_elem);

//Get the length, in bytes, of the serialized compact_int_elem
size_t compact_int_get_byte_length(scale_compact_int *compact_int_elem);

/*
  *
  *   Boolean: https://substrate.dev/docs/en/knowledgebase/advanced/codec#boolean
  *
*/

//Struct Containing A SCALE Boolean Value.
typedef struct {
  bool value;
} scale_boolean;


//Encode a bool value into a scale_boolean Structure
void encode_boolean(scale_boolean *boolean_elem, bool value);

//Consume a SCALE-encoded boolean hex string and populate a scale_boolean Structure
//Returns 0 on success, -1 otherwise
int8_t encode_boolean_from_hex(scale_boolean *boolean_elem, char *boolean_hex_value);

//Serialize a scale_boolean Structure into a SCALE array of bytes. (Length is 1)
void serialize_boolean(uint8_t *serialized, scale_boolean *boolean_elem);

//Deserialize an array of SCALE-encoded boolean bytes (Length is 1) and populate a scale_boolean Structure
void deserialize_boolean(scale_boolean *boolean_elem, const uint8_t* serialized);

//Return a bool value contained in the boolean element
bool decode_boolean(scale_boolean *boolean_elem);

//Returns a malloc'd (remember to free) hex SCALE-encoded boolean string
//Returns NULL if unsuccessful
char *decode_boolean_to_hex(scale_boolean *boolean_elem);


/*
  *
  *   Options: https://substrate.dev/docs/en/knowledgebase/advanced/codec#options
  *
*/

//Struct containing a union of several possible SCALE types
//TODO: add more supported types
typedef struct {
  enum scale_type type;
  union {
    scale_fixed_int _fixed_int;
    scale_compact_int _compact_int;
    scale_boolean _boolean;
  };
} option_value;

//Struct Containing A SCALE Option Value.
typedef struct {
  scale_boolean option;
  option_value value;
} scale_option;


//Encodes a scale_fixed SCALE Struct into an Option. Pass NULL as fixed_int to make Option None
int8_t encode_option_fixed_int(scale_option *option, scale_fixed_int *fixed_int);

//Encodes a compact_int SCALE Struct into an Option. Pass NULL as compact_int to make Option None
//This operation moves compact_int->data array, so remember to release this Option and no longer reference
//the compact_int->data
int8_t encode_option_compact_int(scale_option *option, scale_compact_int *compact_int);

//Encodes a boolean SCALE Struct into an Option. Pass NULL as boolean to make Option None
int8_t encode_option_boolean(scale_option *option, scale_boolean *boolean);

//Returns a malloc'd (remember to free) hex SCALE-encoded Option string
//Returns NULL if unsuccessful
char *decode_option_to_hex(scale_option *option);



/*
  *
  *   Vectors https://substrate.dev/docs/en/knowledgebase/advanced/codec#vectors-lists-series-sets
  *     Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/

//Struct Containing A SCALE Vector Value.
typedef struct {
  uint8_t *data;
  uint64_t data_len;
  scale_compact_int prefix_num_elements;
} scale_vector;

//Pushes an Array of Bytes of Length len Into a scale_vector Structure
//Array of Bytes Should be Serialized SCALE of Same Type (This is Not Currently Enforced)
//Returns 0 on success, -1 otherwise
int8_t push_vector(scale_vector *vec, uint8_t *bytes, size_t len);

//Serialize a scale_vector Structure Into a SCALE-encode Array of Bytes of Length serialized_len
void serialize_vector(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);

//Deserialize a SCALE_encoded Array of Bytes of Length serialized_len into a scale_vector Structure
void deserialize_vector(scale_vector *vec, uint8_t *serialized, size_t serialized_len);

//Points elem to the index'th element of the scale_vector vec.
//elem_width is byte width of each element in the vector. (won't work for variable length types)
//Returns true if successful, false if out of bounds
bool get_vector_index_element(uint8_t **elem, uint64_t index, uint8_t elem_width, scale_vector *vec);

#define scale_vector_foreach(elem, width, vec) \
          for( \
          uint64_t i=1;         \
          get_vector_index_element(elem, i-1, width, vec) == true; \
          i++ \
          )

//Frees malloc'd scale_vector Data
void cleanup_vector(scale_vector *vec);

/*
  *
  *   Strings: https://substrate.dev/docs/en/knowledgebase/advanced/codec#strings
  *
*/

//Helper Function To Create a scale_vector Structure from String
//For Utf8, pass the size in bytes (strlen) and not the number of codepoints ("📚Hamlet" has length of 10, not 7)
void inline create_string(scale_vector *vec, unsigned char *string, size_t len) {
  size_t i;
  for(i=0; i < len; i++) {
    push_vector(vec, &string[i], 1);
  }
}

//Helper Function To Serialize a String Vector Structure
void inline serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec) {
  serialize_vector(serialized, serialized_len, vec);
}

//Deserialize a Utf8 String Vector Structure
//Reads serialized and populates vec
//Returns total bytes of string
size_t deserialize_string(scale_vector *vec, uint8_t *serialized);

//Deserialize a vector of Utf8 Strings into a vec structure
//Places the number of strings read (length of *vec) into num_string_elems
//Returns total bytes read
size_t deserialize_vector_of_strings(scale_vector *vec, size_t *num_string_elems, uint8_t *serialized);


//Helper Function To Clean up a String Vector Structure
void inline cleanup_string(scale_vector *vec) {
  cleanup_vector(vec);
}


/*
  *
  *   Data Structures: https://substrate.dev/docs/en/knowledgebase/advanced/codec#data-structures
  *
*/

//Struct Containing A SCALE Structure Value.
//These functions should point to user-defined functions which serialize and deserialize members of user-defined Structures.
//Every user-defined structure which is to be serialized should contain a scale_structure Structure containing valid serialize and deserialize function pointers
//See tests/structure.c for examples
typedef struct {
  //Function pointer that consumes a user defined struct, void *structure, and outputs a custom SCALE Array of Bytes into uint8_t *serialized, as well as the Length in size_t *bytes
  void (*serialize)(uint8_t* serialized, size_t *bytes, void *structure);

  //Consumes SCALE-Serialized bytes and byte-array len and outputs a struct
  void (*deserialize)(void *structure_out, uint8_t *bytes, size_t len);
} scale_structure;


/*
  *
  *   Enumerations: https://substrate.dev/docs/en/knowledgebase/advanced/codec#enumerations-tagged-unions
  *
*/

//Struct Containing An Enumeration Type.
typedef struct {
  size_t num_elements;
  char *keys[256];
  char *values[256];
} scale_enum_type;

//Takes an Array of Data Types and Possible Data Primitives and Defines this scale_enum_type Enumeration
int8_t encode_scale_enum_type(scale_enum_type *enum_type, size_t num_elements, char *keys[], char *values[]);

//Struct Containing A SCALE Enumeration.
typedef struct {
  option_value value;
  scale_enum_type enum_types;
  char *key;
} scale_enumeration;


//Consumes an Array of Bytes, serialized, and its Length, serialized_len, and the Type of Value Being Encoded By the Enum, key,
// and the Defined scale_enum_type, enum_types, and outputs a Serialized SCALE Array of Bytes
//See test/enumeration.c for example
void encode_enumeration(uint8_t *bytes, scale_enum_type *enum_types, const char *key, uint8_t *serialized, size_t *serialized_len);

//Consume a SCALE encoded enumeration array of Bytes, serialized, and its length, serialized_len, and the Defined scale_enum_type, enum_types,
// and outputs the decoded SCALE-serialized object, bytes, as well as the offset of the enum type used, enum_type_index.
//See test/enumeration.c for example
void decode_enumeration(uint8_t *bytes, uint16_t *enum_type_index, scale_enum_type *enum_types, uint8_t *serialized, size_t *serialized_len);





/*
  *
  *   Tuples: https://substrate.dev/docs/en/knowledgebase/advanced/codec#tuples
  *
*/

//Serialize two SCALE-encoded objects, data_a and data_b, as well as their respective types, a and b, into a
//SCALE_encoded Tuple of the two objects, bytes, as well as its Length len
int8_t serialize_as_tuple(uint8_t *bytes, size_t *len, scale_type a, void *data_a, scale_type b, void *data_b);

//to deserialize, simply check values of a and b and call appropriate deserialization function with 1st and 2nd slice of array bytes
//TODO: add helper function?
#endif
