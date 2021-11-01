/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell

    Note: Current limitation is 2^30 -1 elements, due to native uint64_t support but not u128. Should add a u128 dependency?
*/
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"


//Deserialize a vector of Utf8 Strings into a vec structure
//Places the number of strings read (length of *vec) into num_string_elems
//Returns total bytes read
size_t deserialize_vector_of_strings(scale_vector *vec, size_t *num_string_elems, const uint8_t *restrict serialized) {
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  size_t compact_int_length = read_compact_int_from_data(&compact, serialized);
  *num_string_elems = (size_t)decode_compact_to_u64(&compact);
  cleanup_scale_compact_int(&compact);
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
    cleanup_string(&scale_string);
  }
  return compact_int_length + length;
}



extern void create_string(scale_vector *vec, unsigned char *string, size_t len);
extern void serialize_string(uint8_t *serialized, size_t *serialized_len, scale_vector *vec);
extern size_t inline deserialize_string(scale_vector *vec, const uint8_t *restrict serialized);
extern void cleanup_string(scale_vector *vec);
