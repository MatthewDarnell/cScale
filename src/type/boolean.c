/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Output 0x01 or 0x00 SCALE value for this bool
void serialize_boolean(uint8_t *serialized, _scale_boolean *boolean_elem) {
  *serialized = boolean_elem->value ? 0x01 : 0x00;
}
//Encode a bool value into a boolean element
void _encode_boolean(_scale_boolean *boolean_elem, bool value) {
  memset(boolean_elem, 0, sizeof(_scale_boolean));
  boolean_elem->value = value ? 0x01 : 0x00;;
}

int8_t _encode_boolean_from_hex(_scale_boolean *boolean_elem, char *boolean_hex_value) {
  memset(boolean_elem, 0, sizeof(_scale_boolean));
  char *pHex = boolean_hex_value;
  if(strlen(pHex) < 2) {
    fprintf(stderr, "Failed to Decode Boolean. Invalid Hex.(%s)\n", boolean_hex_value);
    return -1;
  }
  if(pHex[0] == '0' && (pHex[1] == 'x' || pHex[1] == 'X')) {
    pHex += 2;
  }
  if(strcasecmp(pHex, "00") == 0) {
    boolean_elem->value = 0x00;
  } else if(strcasecmp(pHex, "01") == 0) {
    boolean_elem->value = 0x01;
  } else {
    fprintf(stderr, "Failed to Decode Boolean. Invalid Hex Value.(%s)\n", boolean_hex_value);
    return -1;
  }
  return 0;
}

//Return a bool value contained in the boolean element
bool _decode_boolean(_scale_boolean *boolean_elem) {
  return boolean_elem->value;
}

char *_decode_boolean_to_hex(_scale_boolean *boolean_elem) {
  uint8_t serialized = 0;
  serialize_boolean(&serialized, boolean_elem);
  char *hex = (char*)calloc(6, sizeof(char));
  if(!hex) {
    fprintf(stderr, "Error Decoding Boolean To Hex. Out of Memory\n");
    return NULL;
  }
  snprintf(hex, 5, "0x%02X", serialized);
  return hex;
}
