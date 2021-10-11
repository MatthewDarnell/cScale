/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell

    Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"


//Deserialize a vector of Utf8 Strings into a vec structure
//Places the number of strings read (length of *vec) into num_string_elems
//Returns total bytes read
size_t deserialize_vector_of_strings(scale_vector *vec, size_t *num_string_elems, uint8_t *serialized) {
  uint8_t lsb = serialized[0];
  enum scale_compact_int_mode mode = lsb & 0x03;
  uint64_t compact_num_bytes = 0;
  switch(mode) {
    case SCALE_COMPACT_SINGLE_BYTE: {
      compact_num_bytes = 1;
      break;
    }
    case SCALE_COMPACT_TWO_BYTE: {
      compact_num_bytes = 2;
      break;
    }
    case SCALE_COMPACT_FOUR_BYTE: {
      compact_num_bytes = 4;
      break;
    }
    default:
    case SCALE_COMPACT_BIGNUM: {
      compact_num_bytes = lsb >> 2;
      break;
    }
  }

  char *hex = cscale_byte_array_to_hex(serialized, compact_num_bytes);
  if(!hex) {
    fprintf(stderr, "Error deserializing vector! Memory failed to initialize\n");
    return 0;
  }
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  encode_compact_hex_to_scale(&compact, hex);
  free(hex);
  *num_string_elems = decode_compact_to_u64(&compact);
  size_t compact_int_length = compact_int_get_byte_length(&compact);
  size_t i, length = 0;
  for(i = 0; i < *num_string_elems; i++) {
    scale_vector scale_string = SCALE_VECTOR_INIT;
    size_t str_len = deserialize_string(&scale_string, &serialized[compact_int_length + length]);
    uint8_t scale_vec_serialized[str_len+1];
    memset(scale_vec_serialized, 0, str_len+1);
    size_t serialized_len = 0;
    serialize_string(scale_vec_serialized, &serialized_len, &scale_string);
    push_vector(vec, scale_vec_serialized, serialized_len);
    length += str_len;
  }
  return compact_int_length + length;
}


//Deserialize a Utf8 String Vector Structure
//Reads serialized and populates vec
//Returns total number of bytes read, including compact prefix of string
size_t deserialize_string(scale_vector *vec, uint8_t *serialized) {
  uint8_t lsb = serialized[0] & 0xFF;
  enum scale_compact_int_mode mode = lsb & 0x03;
  uint64_t compact_num_bytes = 0;
  switch(mode) {
    case SCALE_COMPACT_SINGLE_BYTE: {
      compact_num_bytes = 1;
      break;
    }
    case SCALE_COMPACT_TWO_BYTE: {
      compact_num_bytes = 2;
      break;
    }
    case SCALE_COMPACT_FOUR_BYTE: {
      compact_num_bytes = 4;
      break;
    }
    default:
    case SCALE_COMPACT_BIGNUM: {
      compact_num_bytes = lsb >> 2;
      break;
    }
  }

  char *hex = cscale_byte_array_to_hex(serialized, compact_num_bytes);
  if(!hex) {
    fprintf(stderr, "Error deserializing vector! Memory failed to initialize\n");
    return 0;
  }
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  encode_compact_hex_to_scale(&compact, hex);
  free(hex);
  size_t string_length = decode_compact_to_u64(&compact);

  size_t compact_int_length = compact_int_get_byte_length(&compact);
  create_string(vec, &serialized[compact_int_length], string_length);
  return compact_int_length + string_length;
}

extern void create_string(scale_vector *vec, unsigned char *string, size_t len);
extern void serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
extern void cleanup_string(scale_vector *vec);