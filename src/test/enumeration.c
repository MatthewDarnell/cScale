/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../scale.h"

struct MyStruct {
  uint32_t a;
  uint64_t b;
};

void struct_MyStruct_to_fixed(void *_struct, void *data) {
  size_t num_struct_elements = 2;
  _scale_struct *pCompactStruct = (_scale_struct*)_struct;

  pCompactStruct->data = (_scale_fixed_int*)calloc(num_struct_elements, sizeof(_scale_fixed_int));
  _scale_fixed_int *fixed_int_array = pCompactStruct->data;

  if(!fixed_int_array) {
      fprintf(stderr, "Error Fix-ing Struct! Memory Failed to Initialize!\n");
      return;
  }
  pCompactStruct->num_elements = num_struct_elements;
  struct MyStruct *value = (struct MyStruct*)data;
  _encode_fixed_uint32_to_scale(&fixed_int_array[0], value->a);
  _encode_fixed_uint64_to_scale(&fixed_int_array[1], value->b);
}

int run_enumeration_test() {
  _scale_enum_type CustomEnum;

  const char *strCustomEnum[][4] = {
    {"Int", "Bool", "Struct", "Compact32"},
    {"uint8_t", "bool", "fixed", "compact32_t"}
  };

  _encode_scale_enum_type(&CustomEnum, 4, (char**)strCustomEnum[0], (char**)strCustomEnum[1]);

  printf("\tEncoding Boolean to Scale:\n");

  _scale_enumeration enumeration;

  char *type = "Int";
  uint8_t value = 42;
  _encode_enumeration(&enumeration, &CustomEnum, type, (void*)&value);
  char *hex = _serialize_enumeration_to_hex(&enumeration);
  printf("\t\tFixed Int Serialized: %s(%u) --- <0x%s>\n", type, value, hex);
  assert(strcasecmp(hex, "002a") == 0);
  free(hex);

  type = "Bool";
  bool v = true;
  _encode_enumeration(&enumeration, &CustomEnum, type, (void*)&v);
  hex = _serialize_enumeration_to_hex(&enumeration);
  printf("\t\tBool Serialized: %s(%u) --- <0x%s>\n", type, value, hex);
  assert(strcasecmp(hex, "0101") == 0);
  free(hex);


  //User Defined Struct
  struct MyStruct t;
  t.a = 1;
  t.b = 2;
  _scale_encoded_struct c_s;
  c_s.data = (void*)&t;
  c_s.fixed = &struct_MyStruct_to_fixed;
  type = "Struct";
  _encode_enumeration(&enumeration, &CustomEnum, type, (void*)&c_s);
  hex = _serialize_enumeration_to_hex(&enumeration);
  printf("\t\tStruct Serialized To Fixed: %s(%u %llu) --- <0x%s>\n", type, t.a, t.b, hex);
  assert(strcasecmp(hex, "02010000000200000000000000") == 0);
  free(hex);
  _cleanup_scale_enum_type(&CustomEnum);

  return 0;
}
