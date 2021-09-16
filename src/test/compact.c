/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../scale.h"


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


void run_compact_8(uint8_t value) {
  _scale_compact_int compact;
  _encode_compact_8(&compact, value);
  printf("(uint8 %u) Decoding Compact 1 Byte "BYTE_TO_BINARY_PATTERN" -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" \n",
    value,
    BYTE_TO_BINARY(value),
    BYTE_TO_BINARY(compact.mode),
    BYTE_TO_BINARY(compact.mode_upper_bits)
  );

  char *hex_8_out = _decode_compact_to_hex(&compact);
  printf("Hex Decoded 8: %s\n\n", hex_8_out);
  free(hex_8_out);

  uint8_t serialized[4] = { 0 };
  uint64_t serialized_len = 0;
  int i;
  serialize_compact_int(serialized, &serialized_len, &compact);
  printf("Output len: %llu\n\tBytes: ", serialized_len);
  for(i = 0; i < serialized_len; i++) {
    printf("%02X", serialized[i]);
  }
  printf("\n\n");
}

void run_compact_16(uint16_t value) {
  _scale_compact_int compact;
  _encode_compact_16(&compact, value);
  printf("(uint16 %u) Decoding Compact 2 Byte -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
    value,
    BYTE_TO_BINARY(compact.mode),
    BYTE_TO_BINARY(compact.mode_upper_bits),
    BYTE_TO_BINARY(compact.data[0])
  );
  char *hex_16_out = _decode_compact_to_hex(&compact);
  printf("Hex Decoded 16: %s\n\n", hex_16_out);
  free(hex_16_out);

  uint8_t serialized[4] = { 0 };
  uint64_t serialized_len = 0;
  int i;
  serialize_compact_int(serialized, &serialized_len, &compact);
  printf("Output len: %llu\n\tBytes: ", serialized_len);
  for(i = 0; i < serialized_len; i++) {
    printf("%02X", serialized[i]);
  }
  printf("\n\n");
}

void run_compact_32(uint32_t value) {
   _scale_compact_int compact;
   _encode_compact_32(&compact, value);
   printf("(uint32 %u) Decoding Compact 4 Byte "BYTE_TO_BINARY_PATTERN" -> mode.("BYTE_TO_BINARY_PATTERN")  upper.("BYTE_TO_BINARY_PATTERN") bytes: ("BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN")\n",
     value,
     BYTE_TO_BINARY(value),
     BYTE_TO_BINARY(compact.mode),
     BYTE_TO_BINARY(compact.mode_upper_bits),
     BYTE_TO_BINARY(compact.data[2]),
     BYTE_TO_BINARY(compact.data[1]),
     BYTE_TO_BINARY(compact.data[0])
   );
  char *hex_32_out = _decode_compact_to_hex(&compact);
  printf("Hex Decoded 32: %s\n\n", hex_32_out);
  free(hex_32_out);

  uint8_t serialized[4] = { 0 };
  uint64_t serialized_len = 0;
  int i;
  serialize_compact_int(serialized, &serialized_len, &compact);
  printf("Output len: %llu\n\tBytes: ", serialized_len);
  for(i = 0; i < serialized_len; i++) {
    printf("%02X", serialized[i]);
  }
  printf("\n\n");
}

void run_compact_64(uint64_t value) {
  _scale_compact_int compact;
  _encode_compact_64(&compact, value);
  printf("(uint64 %llu) Decoding Compact 8 Byte -> mode.("BYTE_TO_BINARY_PATTERN")  upper (num bytes - 4).("BYTE_TO_BINARY_PATTERN" : %u)\n\tbytes: ",
    value,
    BYTE_TO_BINARY(compact.mode),
    BYTE_TO_BINARY(compact.mode_upper_bits),
    compact.mode_upper_bits
  );
  int i;
  for(i = compact.mode_upper_bits + 3; i >= 0; i--) {
    printf(" "BYTE_TO_BINARY_PATTERN" ",
      BYTE_TO_BINARY(compact.data[i])
    );
  }
  printf("\n");
  char *hex_64_out = _decode_compact_to_hex(&compact);
  printf("Hex Decoded 64: %s\n\n", hex_64_out);
  free(hex_64_out);

  uint8_t serialized[4] = { 0 };
  uint64_t serialized_len = 0;
  serialize_compact_int(serialized, &serialized_len, &compact);
  printf("Output len: %llu\n\tBytes: ", serialized_len);
  for(i = 0; i < serialized_len; i++) {
    printf("%02X", serialized[i]);
  }
  printf("\n\n");
}

void run_compact_128(char *value) {
  _scale_compact_int compact;
  _encode_compact_128_from_hex(&compact, value);

  printf("(uint128 %s) Decoding Compact 128 Byte -> mode.("BYTE_TO_BINARY_PATTERN")  upper (num bytes - 4).("BYTE_TO_BINARY_PATTERN" : %u)\n",
    value,
    BYTE_TO_BINARY(compact.mode),
    BYTE_TO_BINARY(compact.mode_upper_bits),
    compact.mode_upper_bits
  );
  int i;
  for(i = 15; i >= 0; i--) {
    printf(
      ""BYTE_TO_BINARY_PATTERN" ",
        BYTE_TO_BINARY(compact.data[i] & 0xFF)
    );
  }
  printf("\n\t");

  for(i = 15; i >= 0; i--) {
    printf(
      ""BYTE_TO_BINARY_PATTERN"",
        BYTE_TO_BINARY(compact.data[i] & 0xFF)
    );
  }
 printf("\n");

 char *hex_128_out = _decode_compact_to_hex(&compact);
 printf("\nHex Decoded 128: %s\n\n", hex_128_out);
 free(hex_128_out);

 uint8_t serialized[4] = { 0 };
 uint64_t serialized_len = 0;
 serialize_compact_int(serialized, &serialized_len, &compact);
 printf("Output len: %llu\n\tBytes: ", serialized_len);
 for(i = 0; i < serialized_len; i++) {
   printf("%02X", serialized[i]);
 }
 printf("\n\n");
}


int run_compact_test() {

  run_compact_8(0);
  run_compact_8(1);
  run_compact_8(42);

//  run_compact_16(65);
  run_compact_16(69);

  run_compact_32(20000000);
  //run_compact_64(4888899999999999);
  //run_compact_128("0x3fffffffffffffffffffffffffffffff");






  return 0;
}
