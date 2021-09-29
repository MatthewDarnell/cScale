/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

int8_t serialize_as_tuple(uint8_t *bytes, size_t *len, scale_type a, void *data_a, scale_type b, void *data_b) {
  //typedef enum { FIXED_INT, COMPACT_INT, BOOLEAN, OPTION, ENUM, VECTOR, STRING, STRUCT  } scale_type;
  *len = 0;
  switch(a) {
    case FIXED_INT: {
      scale_fixed_int *value = (scale_fixed_int*)data_a;
      serialize_fixed_int(bytes, (uint64_t*)len, value);
      break;
    }
    case COMPACT_INT: {
      scale_compact_int *value = (scale_compact_int*)data_a;
      serialize_compact_int(bytes, (uint64_t*)len, value);
      break;
    }
    case BOOLEAN: {
      scale_boolean  *value = (scale_boolean*)data_a;
      serialize_boolean(bytes, value);
      *len = 1;
      break;
    }
    case VECTOR: {
      scale_vector *value = (scale_vector*)data_a;
      serialize_vector(bytes, len, value);
      break;
    }
    case STRING: {
      scale_vector *value = (scale_vector*)data_a;
      serialize_vector(bytes, len, value);
      break;
    }
    default: {
      fprintf(stderr, "Unknown Value for Tuple type a. If it is an Enum, Option, or Structure, Try serializing directly and prepending to second serialized value.\n");
      return -1;
    }
  };

  size_t len_2 = 0;
  switch(b) {
    case FIXED_INT: {
      scale_fixed_int *value = (scale_fixed_int*)data_b;
      serialize_fixed_int(bytes+ *len, (uint64_t*)&len_2, value);
      break;
    }
    case COMPACT_INT: {
      scale_compact_int *value = (scale_compact_int*)data_b;
      serialize_compact_int(bytes+ *len, (uint64_t*)&len_2, value);
      break;
    }
    case BOOLEAN: {
      scale_boolean  *value = (scale_boolean*)data_b;
      serialize_boolean(bytes+ *len, value);
      len_2 = 1;
      break;
    }
    case VECTOR: {
      scale_vector *value = (scale_vector*)data_b;
      serialize_vector(bytes+ *len, &len_2, value);
      break;
    }
    case STRING: {
      scale_vector *value = (scale_vector*)data_b;
      serialize_vector(bytes+ *len, &len_2, value);
      break;
    }
    default: {
      fprintf(stderr, "Unknown Value for Tuple type b. If it is an Enum, Option, or Structure, Try serializing directly and appending to first serialized value.\n");
      return -1;
    }
  };
  *len += len_2;

  return 0;
}