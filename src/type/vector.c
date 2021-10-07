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

void deserialize_vector(scale_vector *vec, uint8_t *serialized, size_t serialized_len) {
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
    return;
  }
  encode_compact_hex_to_scale(&vec->prefix_num_elements, hex);
  free(hex);

  vec->data_len = serialized_len * sizeof(uint8_t);
  vec->data = calloc(serialized_len, sizeof(uint8_t));
  if(!vec->data) {
    fprintf(stderr, "Error deserializing vector! Vector Memory failed to initialize\n");
    return;
  }
  serialized_len -= 1;

  if(mode == SCALE_COMPACT_BIGNUM) {
    memcpy(vec->data, &serialized[1], compact_num_bytes);
  } else {
    memcpy(vec->data, &serialized[1], serialized_len);
  }
}

void serialize_vector(uint8_t *serialized, size_t *serialized_len, scale_vector *vec) {
  serialize_compact_int(serialized, (uint64_t*)serialized_len, &(vec->prefix_num_elements));
  memcpy(&serialized[*serialized_len], vec->data, vec->data_len);
  *serialized_len += vec->data_len;
}

void cleanup_vector(scale_vector *vec) {
  cleanup_scale_compact_int(&vec->prefix_num_elements);
  if(vec->data) {
    free(vec->data);
  }
  memset(vec, 0, sizeof(scale_vector));
}
int8_t push_vector(scale_vector *vec, uint8_t *bytes, size_t len) {
  uint64_t num_elements = decode_compact_to_u64(&vec->prefix_num_elements);
  if(num_elements > 4611686018427387903) {
    fprintf(stderr, "Error appending vector! Too many elements!\n");
    return -1;
  }
  if(!vec->data) {
    vec->data = calloc(len, sizeof(uint8_t));
    if(!vec->data) {
      fprintf(stderr, "Error appending vector! Memory failed to initialize!\n");
      return -1;
    }
  } else {
    uint8_t *elements = realloc(vec->data, (vec->data_len + len) * sizeof(uint8_t));
    if(elements) {
      vec->data = elements;
    } else {
      fprintf(stderr, "Error appending vector! Memory failed to re-initialize!\n");
      return -1;
    }
  }
  memcpy(vec->data + vec->data_len, bytes, len);
  vec->data_len += len;
  num_elements++;
  encode_compact(&vec->prefix_num_elements, num_elements);
  return 0;
}

//Points elem to the index'th element of the scale_vector vec.
//elem_width is byte width of each element in the vector. (won't work for variable length types)
//Returns true if successful, false if out of bounds
bool get_vector_index_element(uint8_t **elem, uint64_t index, uint8_t elem_width, scale_vector *vec) {
  uint64_t vec_num_elems = decode_compact_to_u64(&vec->prefix_num_elements);
  if(index >= vec_num_elems || vec->data_len < (vec_num_elems * elem_width)) {
    return false;
  }
  *elem = &(vec->data[index*elem_width]);
  return true;
}
