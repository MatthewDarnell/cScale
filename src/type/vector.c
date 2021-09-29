/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell

    Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"


/*
typedef struct scale_vector {
  uint8_t *data;
  scale_compact_int prefix_num_elements;
} scale_vector;

void serialize_vector(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
void deserialize_vector(scale_vector *vec, uint8_t *serialized, size_t *serialized_len);
void cleanup_vector(scale_vector *vec);

*/
void deserialize_vector(scale_vector *vec, uint8_t *serialized, size_t *serialized_len) {
  cleanup_vector(vec);
  uint8_t lsb = serialized[0];
  enum scale_compact_int_mode mode = lsb & 0x03;
  uint64_t compact_num_bytes = 0;
  switch(mode) {
    case SINGLE_BYTE: {
      compact_num_bytes = 1;
      break;
    }
    case TWO_BYTE: {
      compact_num_bytes = 2;
      break;
    }
    case FOUR_BYTE: {
      compact_num_bytes = 4;
      break;
    }
    case BIGNUM: {
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

  vec->data_len = *serialized_len;
  vec->data = calloc(*serialized_len, sizeof(uint8_t));
  if(!vec->data) {
    fprintf(stderr, "Error deserializing vector! Vector Memory failed to initialize\n");
    return;
  }
  *serialized_len -= 1;

  if(mode == BIGNUM) {
    memcpy(vec->data, &serialized[1], compact_num_bytes);
  } else {

    memcpy(vec->data, &serialized[1], *serialized_len);
  }
}

void serialize_vector(uint8_t *serialized, size_t *serialized_len, scale_vector *vec) {

  serialize_compact_int(serialized, (uint64_t*)serialized_len, &(vec->prefix_num_elements));
  memcpy(&serialized[*serialized_len], vec->data, vec->data_len);
  *serialized_len += vec->data_len;
}

void cleanup_vector(scale_vector *vec) {
  if(vec->data) {
    free(vec->data);
  }
  memset(vec, 0, sizeof(scale_vector));
}
int8_t push_vector(scale_vector *vec, uint8_t *bytes, size_t len) {
  scale_compact_int *prefix_num_elements = &vec->prefix_num_elements;
  char *hex_num_elements = decode_compact_to_hex(prefix_num_elements);
  uint64_t num_elements = strtoull(hex_num_elements, NULL, 16);
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
  encode_compact(prefix_num_elements, num_elements);
  return 0;
}



extern void create_string(scale_vector *vec, unsigned char *string, size_t len);
extern void serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
extern void deserialize_string(scale_vector *vec, uint8_t *serialized, size_t *serialized_len);;
extern void cleanup_string(scale_vector *vec);