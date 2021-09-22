/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

#define t fprintf(stderr, "%s %d\n", __func__, __LINE__);
int8_t _encode_scale_enum_type(_scale_enum_type *enum_type, size_t num_elements, char *keys[], char *values[]) {
  if(num_elements < 1) {
    fprintf(stderr, "Error Encoding Scale Enum Type. Invalid Number of Elements.(%lu)\n", num_elements);
    return -1;
  }
  enum_type->num_elements = num_elements;

  size_t i;
  for(i=0; i < num_elements; i++) {
    enum_type->keys[i] = strdup(keys[i]);
    enum_type->values[i] = strdup(values[i]);
  }

  return 0;
}
void _print_scale_enum_type(_scale_enum_type *enum_type) {
  size_t i;
  for(i = 0; i < enum_type->num_elements; i++) {
    fprintf(stderr, "%lu: %s(%s)\n", i, enum_type->keys[i], enum_type->values[i]);
  }
}

void _cleanup_scale_enum_type(_scale_enum_type *enum_type) {
  size_t i;
  for(i =0; i < enum_type->num_elements; i++) {
    free(enum_type->keys[i]);
    free(enum_type->values[i]);
  }
  memset(enum_type, 0, sizeof(_scale_enum_type));
}

char* _serialize_enumeration_to_hex(_scale_enumeration *enumeration) {
  uint8_t serialized[64] = { 0 };
  size_t serialized_len = 0;
  if( _serialize_enumeration(serialized, &serialized_len, enumeration) < 0) {
    fprintf(stderr, "Error Serializing Enumeration to Hex\n");
    return NULL;
  }
  return _byte_array_to_hex(serialized, serialized_len);
}

int8_t _serialize_enumeration(uint8_t *serialized, size_t *serialized_len, _scale_enumeration *enumeration) {
  char *key = enumeration->key;
  enum _option_type type = enumeration->value.type;
  uint8_t enum_offset_prefix = 0x00;
  size_t i;
  bool found = false;
  for(i = 0; i < enumeration->enum_types.num_elements; i++) {
    if(strcasecmp(key, enumeration->enum_types.keys[i]) == 0) {
      enum_offset_prefix = i;
      found = true;
      break;
    }
  }
  if(!found) {
    fprintf(stderr, "Error Serializing Enumeration. Invalid Key Type: %s\n", key);
    return -1;
  }

  serialized[0] = enum_offset_prefix;
  *serialized_len = 1;
  uint64_t element_serialized_len = 0;
  switch (type) {
    case FIXED_INT: {
      if(serialize_fixed_int(&serialized[1], &element_serialized_len, &enumeration->value._fixed_int) < 0) {
        fprintf(stderr, "Error Serializing Fixed Int Enumeration\n");
        return -1;
      }
      *serialized_len += element_serialized_len;
      break;
    }
    /*
    case STRUCT_FIXED_INT: {

      _scale_struct p_scale_struct = enumeration->value._struct;
      size_t num_elements = p_scale_struct.num_elements;
      _scale_fixed_int *elements = (_scale_fixed_int*)p_scale_struct.data;

      size_t i;
      for(i = 0; i < num_elements; i++) {
        uint64_t len = 0;
        if(serialize_fixed_int(&serialized[*serialized_len], &len, &elements[i]) < 0) {
          fprintf(stderr, "Error Serializing Fixed Int Enumeration\n");
          return -1;
        }
        *serialized_len += len;
      }
      break;

    }
    */
    case BOOLEAN: { //bools in scale always 1 byte
      serialize_boolean(&serialized[1], &enumeration->value._boolean);
      *serialized_len = 2;
      break;
    }
    default: {
      fprintf(stderr, "Error Serializing Enumeration. Unknown Type\n");
      return -1;
    }
  }
  return 0;

}


//Supported keys: [Int, Compact, Bool, Struct]
void _encode_enumeration(uint8_t *bytes, _scale_enum_type *enum_types, const char *key, uint8_t *serialized, size_t *serialized_len) {
  size_t num_elements = enum_types->num_elements;
  char **keys = enum_types->keys;
  char **values = enum_types->values;
  size_t i;
  for(i = 0; i < num_elements; i++) {
    char *temp_key = keys[i];
    char *temp_value = values[i];
    if(strcasecmp(temp_key, key) == 0) {
      bytes[0] = i;
      printf("%s matches %s at index %lu\n", temp_key, key, i);
      memcpy(&bytes[1], serialized, *serialized_len);
      return;
    }
  }
  fprintf(stderr, "Error Encoding Enumeration. Type not Found!\n");
}
