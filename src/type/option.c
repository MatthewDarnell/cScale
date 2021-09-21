/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

/*
typedef enum _option_type { FIXED_INT, COMPACT_INT, BOOLEAN };
typedef struct _option_value {
  enum _option_type type;
  union value {
    _scale_fixed_int _fixed_int;
    _scale_compact_int _compact_int;
    _scale_boolean _boolean;
  };
} _option_value;
typedef struct _scale_option {
  _scale_boolean option;
  _option_value value;
} _scale_option;

*/
int8_t _encode_option_fixed_int(_scale_option *option, _scale_fixed_int *fixed_int) {
  memset(option, 0, sizeof(_scale_option));
  if(fixed_int) {
    _encode_boolean(&option->option, true);
  }
  else {
    _encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(_option_value));
  option->value.type = FIXED_INT;
  memmove(&option->value._fixed_int, fixed_int, sizeof(_scale_fixed_int));
  return 0;
}

int8_t _encode_option_boolean(_scale_option *option, _scale_boolean *boolean) {
  memset(option, 0, sizeof(_scale_option));
  if(boolean) {
    _encode_boolean(&option->option, true);
  }
  else {
    _encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(_option_value));
  option->value.type = BOOLEAN;
  memmove(&option->value._boolean, boolean, sizeof(_scale_boolean));
  return 0;
}

int8_t _encode_option_compact_int(_scale_option *option, _scale_compact_int *compact_int) {
  memset(option, 0, sizeof(_scale_option));
  if(compact_int) {
    _encode_boolean(&option->option, true);
  }
  else {
    _encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(_option_value));
  option->value.type = COMPACT_INT;
  memmove(&option->value._compact_int, compact_int, sizeof(_scale_compact_int));

  option->value._compact_int.data = compact_int->data;
  compact_int->data = NULL;
  return 0;
}

char *_decode_option_to_hex(_scale_option *option) {
  if(!option) {
    fprintf(stderr, "Error Decoding Option\n");
    return NULL;
  }

  char *out = NULL;

  if(_decode_boolean(&option->option) == 0x00) {
    out = calloc(4, sizeof(char));
    if(!out) {
      fprintf(stderr, "Error Initializing Memory For Option Hex\n");
      return NULL;
    }
    strcpy(out, "0x00");
    return out;
  }

  if(option->value.type == FIXED_INT) {
    char *serialized = NULL;
    if(!(serialized = decode_scale_fixed_to_hex(&option->value._fixed_int))) {
      fprintf(stderr, "Error Serializing Fixed Int For Option Hex\n");
      return NULL;
    }
    out = calloc(3 + strlen(serialized) + 1, sizeof(char));
    if(!out) {
      fprintf(stderr, "Error Initializing Memory For Option Hex\n");
      return NULL;
    }
    strcpy(out, "0x01");
    strcat(out, serialized);
    free(serialized);
    return out;
  } else if(option->value.type == COMPACT_INT) {

    uint8_t serialized[64] = { 0 };
    uint64_t serialized_len = 0;
    serialize_compact_int(serialized, &serialized_len, &option->value._compact_int);
    char *hex = _byte_array_to_hex(serialized, serialized_len);
    if(!hex) {
      fprintf(stderr, "Error Initializing Memory For Option Hex\n");
      return NULL;
    }
    out = calloc(3 + strlen(hex) + 1, sizeof(char));
    strcpy(out, "0x01");
    strcat(out, hex);
    free(hex);

    return out;

  } else if(option->value.type == BOOLEAN) {
    char *out = calloc(3, sizeof(char));
    if(_decode_boolean(&option->value._boolean) == 0x01) {
      strcpy(out, "0x01");
    } else {
      strcpy(out, "0x02");
    }
    return out;
  } else {
    fprintf(stderr, "Unknown Option Type\n");
    return NULL;
  }
}
