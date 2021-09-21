/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Output 0x01 or 0x00 SCALE value for this bool
void seralize_boolean(uint8_t *serialized, _scale_boolean *boolean_elem) {
  *serialized = boolean_elem->value ? 0x01 : 0x00;
}
//Encode a bool value into a boolean element
void _encode_boolean(_scale_boolean *boolean_elem, bool value) {
  memset(boolean_elem, 0, sizeof(_scale_boolean));
  boolean_elem->value = value ? 0x01 : 0x00;;
}
//Return a bool value contained in the boolean element
bool _decode_boolean(_scale_boolean *boolean_elem) {
  return boolean_elem->value;
}

char *_decode_boolean_to_hex(_scale_boolean *boolean_elem) {
  uint8_t serialized = 0;
  seralize_boolean(&serialized, boolean_elem);
  char *hex = (char*)calloc(6, sizeof(char));
  if(!hex) {
    fprintf(stderr, "Error Decoding Boolean To Hex. Out of Memory\n");
    return NULL;
  }
  snprintf(hex, 5, "0x%02X", serialized);
  return hex;
}
