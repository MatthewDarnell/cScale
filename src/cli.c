/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scale.h"

#define help  "Usage: app [encode/decode] [fixed/compact] [signed (0/1)] [size (1/2/4/8/16)] [value]\n"


  void run_fixed(uint64_t value, int8_t is_signed, int8_t byte_length) {
    _scale_fixed_int scale_fixed;
    uint8_t serialized[64] = { 0 };
    uint64_t serialized_len = 0;

    int i;

    if(byte_length == 1) {
      if (is_signed) {
        _encode_fixed_int8_to_scale(&scale_fixed, (int8_t)value);
      } else {
        _encode_fixed_uint8_to_scale(&scale_fixed, (uint8_t)value);
      }
    } else if(byte_length == 2) {
      if (is_signed) {
        _encode_fixed_int16_to_scale(&scale_fixed, (int16_t)value);

      } else {
        _encode_fixed_uint16_to_scale(&scale_fixed, (uint16_t)value);
      }
    } else if(byte_length == 4) {
      if (is_signed) {
        _encode_fixed_int32_to_scale(&scale_fixed, (int32_t)value);
      } else {
        _encode_fixed_uint32_to_scale(&scale_fixed, (uint32_t)value);
      }
    }

    memset(serialized, 0, 64 * sizeof(uint8_t));
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &scale_fixed);
    printf("0x");
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n");
  }

  void decode_fixed(char *value, int8_t is_signed) {
    _scale_fixed_int scale_fixed;
    if(_encode_fixed_hex_to_scale(&scale_fixed, is_signed, value) < 0) {
      fprintf(stderr, "Error Decoding <%s>\n", value);
      return;
    }
    uint32_t decoded = 0;
    if(decode_scale_fixed_int((void*)&decoded, &scale_fixed) < 0) {
      fprintf(stderr, "Error Decoding <%s>\n", value);
      return;
    }
    printf("%u\n", decoded);
  }

  void run_compact(char *hexValue, uint64_t value, int8_t byte_length) {
    _scale_compact_int scale_compact;

    if(byte_length == 1) {
        _encode_compact_8(&scale_compact, (uint8_t)value);
    } else if(byte_length == 2) {
      _encode_compact_16(&scale_compact, (uint16_t)value);
    } else if(byte_length == 4) {
      _encode_compact_32(&scale_compact, (uint16_t)value);
    } else if(byte_length == 8) {
      _encode_compact_64(&scale_compact, (uint16_t)value);
    } else if(byte_length == 16) {
      if(hexValue) {
        _encode_compact_128_from_hex(&scale_compact, hexValue);
      } else {
        printf("Unable to Encode to u128. Did you pass a valid hex string?\n");
        return;
      }
    }

    uint8_t serialized[32] = { 0 };
    uint64_t serialized_len = 0;
    serialize_compact_int(serialized, &serialized_len, &scale_compact);

    printf("0x");
    int i;
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n");
  }

int main(int argc, char **argv) {
  if(argc != 6) {
    fprintf(stderr, help);
    return -1;
  }
  const char *command = argv[1];
  const char *type = argv[2];
  int is_signed = atoi(argv[3]);
  int size = atoi(argv[4]);
  const char *value = argv[5];


  int dCommand = 0;
  if(strcasecmp(command, "decode") == 0) {
    dCommand = 1;
  }

  int dType = 0;
  if(strcasecmp(type, "compact") == 0) {
    dType = 1;
  }
  if(dType == 0 && size > 4) {
    fprintf(stderr, "Invalid Size. Fixed Size Ints Valid Sizes: 1/2/4 \n");
    return -1;
  }

  if(dCommand == 0) {  //encoded
    uint64_t dValue = strtoull(value, NULL, 10);
    if(dValue) {
      if(dType == 0) {  //fixed
          run_fixed(dValue, is_signed, size);
      } else if (dType == 1) {  //compact
          if(is_valid_hex(value) && size == 16) { //u128 must be hex string
            run_compact((char*)value, 0, size);
          } else {
            run_compact(NULL, dValue, size);
          }
      }
    } else {
      if(dType == 0) {
        run_compact(NULL, 0, 1);
      } else if (dType == 1) {
        run_compact((char*)value, 0, size);
      }
    }
  } else {  //decode
      if(dType == 0) {  //fixed
        decode_fixed((char*)value, is_signed);
      } else if(dType == 1) {  //compact

      }
  }


   return 0;
}
