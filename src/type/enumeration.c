/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

#define t fprintf(stderr, "%s %d\n", __func__, __LINE__);
int8_t encode_scale_enum_type(scale_enum_type *enum_type, size_t num_elements, char *keys[], char *values[]) {
  if(num_elements < 1) {
    fprintf(stderr, "Error Encoding Scale Enum Type. Invalid Number of Elements.(%u)\n", (unsigned)num_elements);
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

void decode_enumeration(uint8_t *bytes, uint16_t *enum_type_index, scale_enum_type *enum_types, uint8_t *serialized, size_t *serialized_len) {
  uint8_t enum_type = serialized[0];
  char **keys = enum_types->keys;
  char *temp_key = keys[enum_type];
  *enum_type_index = enum_type;
  printf("%s matches at index %u\n", temp_key, enum_type);
  memcpy(bytes, &serialized[1], *serialized_len);
}

//Supported keys: [Int, Compact, Bool, Struct]
void encode_enumeration(uint8_t *bytes, scale_enum_type *enum_types, const char *key, uint8_t *serialized, size_t *serialized_len) {
  size_t num_elements = enum_types->num_elements;
  char **keys = enum_types->keys;
  size_t i;
  for(i = 0; i < num_elements; i++) {
    char *temp_key = keys[i];
    if(strcasecmp(temp_key, key) == 0) {
      bytes[0] = i;
      printf("%s matches %s at index %u\n", temp_key, key, (unsigned)i);
      memcpy(&bytes[1], serialized, *serialized_len);
      return;
    }
  }
  fprintf(stderr, "Error Encoding Enumeration. Type not Found!\n");
}
