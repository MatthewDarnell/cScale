/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell

    Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../util/utf8.h"
#include "../scale.h"

//Deserialize a Utf8 String Vector Structure
//Reads serialized and populates vec
//Returns total number of bytes read, including compact prefix of string
size_t deserialize_string(scale_vector *vec, uint8_t *serialized) {
  cleanup_vector(vec);
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
  scale_compact_int compact = { 0 };
  encode_compact_hex_to_scale(&compact, hex);
  free(hex);
  size_t string_length = decode_compact_to_u64(&compact);

  size_t compact_int_length = compact_int_get_byte_length(&compact);
  return compact_int_length + create_utf8_string(vec, &serialized[compact_int_length], string_length);
}

size_t create_utf8_string(scale_vector *vec, uint8_t *string, size_t string_length) {
  utf8_int32_t codepoint = 0;
  uint8_t out_str[16];  //max utf8 codepoint is 4 bytes
  memset(out_str, 0, 16);
  size_t i;
  size_t total_size = 0;
  char *next_codepoint = utf8codepoint(string, &codepoint);
  for(i=0; i < string_length; i++) {
    memset(out_str, 0, 16);
    size_t codepoint_size = utf8codepointsize(codepoint);
    total_size += codepoint_size;
    utf8catcodepoint(out_str, codepoint, 15);
    push_vector(vec, out_str, codepoint_size);
    next_codepoint = utf8codepoint(next_codepoint, &codepoint);
  }
  return total_size;
}

extern void create_string(scale_vector *vec, unsigned char *string, size_t len);
extern void serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
extern void cleanup_string(scale_vector *vec);