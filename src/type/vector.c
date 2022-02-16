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

//Reads the serialized Vector byte array starting at serialized[0] into a scale_vector Structure
//element_width should contain the byte length of each element. (u16=2, char=1)
//Returns the total number of bytes read
//Returns 0 if fails to read
size_t read_vector_from_data(scale_vector *vec, uint8_t element_width, const uint8_t *restrict serialized, size_t size) {
  size_t prefix_num_element_byte_length = read_compact_int_from_data(&vec->prefix_num_elements, serialized, size);

  uint64_t data_length = decode_compact_to_u64(&vec->prefix_num_elements) * element_width;

  vec->data_len = data_length * sizeof(uint8_t);
  vec->data = calloc(data_length, sizeof(uint8_t));
  if(!vec->data) {
    fprintf(stderr, "Error deserializing vector! Vector Memory failed to initialize\n");
    return 0;
  }

  memcpy(vec->data, &serialized[prefix_num_element_byte_length], data_length);
  return prefix_num_element_byte_length + data_length;
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
  vec->data = NULL;
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
