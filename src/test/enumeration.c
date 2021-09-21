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

  /*
      Maps to Rust type Enum:

        enum IntOrBool {
          Int(u8),
          Bool(bool),
        }
  */
  _scale_enum_type IntOrBool;

  const char *strIntOrBool[][2] = {
    {"Int", "Bool"},
    {"uint8_t", "bool"}
  };

  _encode_scale_enum_type(&IntOrBool, 2, (char**)strIntOrBool[0], (char**)strIntOrBool[1]);

  printf("\tEncoding Boolean to Scale:\n");

  _scale_enumeration enumeration;

  char *type = "Int";
  uint8_t value = 42;
  _encode_enumeration(&enumeration, &IntOrBool, type, (void*)&value);


  char *hex = _serialize_enumeration_to_hex(&enumeration);
  printf("serialized: %s(%u) --- <0x%s>\n", type, value, hex);

  free(hex);
  type = "Bool";
  bool v = true;
  _encode_enumeration(&enumeration, &IntOrBool, type, (void*)&v);
  hex = _serialize_enumeration_to_hex(&enumeration);
  printf("serialized: %s(%u) --- <0x%s>\n", type, value, hex);

  free(hex);
  _cleanup_scale_enum_type(&IntOrBool);

  return 0;
}
