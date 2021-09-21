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

int run_boolean_test() {
  uint8_t out = 0;
  char *hex = NULL;

  _scale_boolean boolean;

  printf("\tEncoding Boolean to Scale:\n");
  _encode_boolean(&boolean, true);
  seralize_boolean(&out, &boolean);
  hex = _decode_boolean_to_hex(&boolean);
  assert(hex);
  assert(strcasecmp(hex, "0x01") == 0);

  assert(out == 0x01);
  assert(_decode_boolean(&boolean) == 0x01);
  printf("\t\tBoolean.(true) is: .(%s)\n", hex);
  free(hex);


  out = 1;
  _encode_boolean(&boolean, false);
  seralize_boolean(&out, &boolean);
  hex = _decode_boolean_to_hex(&boolean);
  assert(hex);
  assert(strcasecmp(hex, "0x00") == 0);
  printf("\t\tBoolean.(false) is: .(%s)\n", hex);
  free(hex);
  assert(out == 0x00);
  assert(_decode_boolean(&boolean) == 0x00);
  return 0;
}