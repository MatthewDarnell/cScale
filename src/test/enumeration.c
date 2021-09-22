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



  return 0;
}
