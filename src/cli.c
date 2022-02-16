/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util/hex.h"
#include "scale.h"

#define help  "Usage:\n" \
              "\tapp encode fixed [signed (0/1)] [size (1/2/4/8)] [value]\n" \
              "\tapp decode fixed [signed (0/1)] [value (hex)]\n\n" \
              "\tapp encode compact [size (1/2/4/8/16)] [value]\t(Note: 16 byte encoding requires hex value. Others are decimal)\n" \
              "\tapp decode compact [value (hex)]\n" \



  void run_fixed(uint64_t value, int8_t is_signed, int8_t byte_length) {
    scale_fixed_int scale_fixed;
    uint8_t serialized[64] = { 0 };
    uint64_t serialized_len = 0;

    int i;

    if(byte_length == 1) {
      if (is_signed) {
        encode_int_to_fixed_int_scale(&scale_fixed, (int8_t)value);
      } else {
        encode_int_to_fixed_int_scale(&scale_fixed, (uint8_t)value);
      }
    } else if(byte_length == 2) {
      if (is_signed) {
        encode_int_to_fixed_int_scale(&scale_fixed, (int16_t)value);

      } else {
        encode_int_to_fixed_int_scale(&scale_fixed, (uint16_t)value);
      }
    } else if(byte_length == 4) {
      if (is_signed) {
        encode_int_to_fixed_int_scale(&scale_fixed, (int32_t)value);
      } else {
        encode_int_to_fixed_int_scale(&scale_fixed, (uint32_t)value);
      }
    } else {
      if (is_signed) {
        encode_int_to_fixed_int_scale(&scale_fixed, (int64_t)value);
      } else {
        encode_int_to_fixed_int_scale(&scale_fixed, (uint64_t)value);
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
    scale_fixed_int scale_fixed;
    if(encode_fixed_hex_to_scale(&scale_fixed, is_signed, value) < 0) {
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

  void decode_compact(const char *value) {
    scale_compact_int scale_compact = SCALE_COMPACT_INT_INIT;
    printf("decoding %s\n", value);
    if(encode_compact_hex_to_scale(&scale_compact, value, strlen(value)) < 0) {
      fprintf(stderr, "Failed to Decode!\n Valid Hex?");
      return;
    }
    char *decoded = decode_compact_to_hex(&scale_compact);
    if(!decoded) {
      fprintf(stderr, "Failed to Decode Compact!\n");
      return;
    }
    printf("decoded: %s\n", decoded);
    free(decoded);


  }

  void run_compact(char *hexValue, uint64_t value, int8_t byte_length) {
    scale_compact_int scale_compact = SCALE_COMPACT_INT_INIT;

    if(byte_length == 1) {
        if(encode_compact(&scale_compact, (uint8_t)value) < 0) return;
    } else if(byte_length == 2) {
      if(encode_compact(&scale_compact, (uint16_t)value) < 0) return;
    } else if(byte_length == 4) {
      if(encode_compact(&scale_compact, (uint32_t)value) < 0) return;
    } else if(byte_length == 8) {
      if(encode_compact(&scale_compact, (uint64_t)value) < 0) return;
    } else if(byte_length == 16) {
      if(hexValue) {
        if(encode_u128_string_to_compact_int_scale(&scale_compact, hexValue, strlen(hexValue)) < 0) return;
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
  if(argc < 4) {
    fprintf(stderr, help);
    return -1;
  }
  const char *command = argv[1];
  const char *type = argv[2];

  int dCommand = 0;
  if(strcasecmp(command, "decode") == 0) {
    dCommand = 1;
  }
  int dType = 0;
  if(strcasecmp(type, "compact") == 0) {
    dType = 1;
  }

  uint8_t required_args = 0;
  if(dCommand == 0 && dType == 0) required_args = 6;
  else if(dCommand == 0 && dType == 1) required_args = 5;
  else if(dCommand == 1 && dType == 0) required_args = 5;
  else if(dCommand == 1 && dType == 1) required_args = 4;



  if(argc < required_args) {
    fprintf(stderr, help);
    return -1;
  }

  int is_signed;
  int size;
  const char *value;


  if(dCommand == 0 && dType == 0) { //encode fixed
    is_signed = atoi(argv[3]);
    size = atoi(argv[4]);
    value = argv[5];
  }
  else if(dCommand == 0 && dType == 1) { //encode compact
    size = atoi(argv[3]);
    value = argv[4];
  }
  else if(dCommand == 1 && dType == 0) { //decode fixed
    is_signed = atoi(argv[3]);
    value = argv[4];
  }
  else if(dCommand == 1 && dType == 1) { //decode compact
    value = argv[3];
  }





  if(dType == 0 && ( (dCommand == 0 && size > 8) || (dCommand == 0 && size > 1 && size % 2 != 0))) {
    fprintf(stderr, "Invalid Size. Fixed Size Ints Valid Sizes: 1/2/4/8 \n");
    return -1;
  }

  if(dCommand == 0) {  //encoded
    uint64_t dValue = strtoull(value, NULL, 10);
    if(dValue) {
      if(dType == 0) {  //fixed
          run_fixed(dValue, is_signed, size);
      } else if (dType == 1) {  //compact
          if(cscale_is_valid_hex(value) && size == 16) { //u128 must be hex string
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
        decode_compact(value);
      }
  }


   return 0;
}
