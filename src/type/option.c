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
typedef struct option_value {
  enum _option_type type;
  union value {
    scale_fixed_int _fixed_int;
    scale_compact_int _compact_int;
    scale_boolean _boolean;
  };
} option_value;
typedef struct scale_option {
  scale_boolean option;
  option_value value;
} scale_option;

*/
int8_t encode_option_fixed_int(scale_option *option, scale_fixed_int *fixed_int) {
  memset(option, 0, sizeof(scale_option));
  if(fixed_int) {
    encode_boolean(&option->option, true);
  }
  else {
    encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(option_value));
  option->value.type = FIXED_INT;
  memmove(&option->value._fixed_int, fixed_int, sizeof(scale_fixed_int));
  return 0;
}

int8_t encode_option_boolean(scale_option *option, scale_boolean *boolean) {
  memset(option, 0, sizeof(scale_option));
  if(boolean) {
    encode_boolean(&option->option, true);
  }
  else {
    encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(option_value));
  option->value.type = BOOLEAN;
  memmove(&option->value._boolean, boolean, sizeof(scale_boolean));
  return 0;
}

int8_t encode_option_compact_int(scale_option *option, scale_compact_int *compact_int) {
  memset(option, 0, sizeof(scale_option));
  if(compact_int) {
    encode_boolean(&option->option, true);
  }
  else {
    encode_boolean(&option->option, false);
    return 0;
  }
  memset(&option->value, 0, sizeof(option_value));
  option->value.type = COMPACT_INT;
  memmove(&option->value._compact_int, compact_int, sizeof(scale_compact_int));

  option->value._compact_int.data = compact_int->data;
  compact_int->data = NULL;
  return 0;
}

char *decode_option_to_hex(scale_option *option) {
  if(!option) {
    fprintf(stderr, "Error Decoding Option\n");
    return NULL;
  }

  char *out = NULL;

  if(decode_boolean(&option->option) == 0x00) {
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
    char *hex = cscale_byte_array_to_hex(serialized, serialized_len);
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
    if(decode_boolean(&option->value._boolean) == 0x01) {
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
