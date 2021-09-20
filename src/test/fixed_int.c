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

extern void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex);

static void run_test(uint64_t value, size_t width, uint8_t is_signed, const char *expected_hex_serialized) {
  _scale_fixed_int s_e;
  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;
  char *hex = NULL;
  printf("\t\tEncoding <%llu>: ", value);

  switch (width) {
    case 1: {
      if(is_signed) _encode_fixed_int8_to_scale(&s_e, (int8_t)value);
      else _encode_fixed_int8_to_scale(&s_e, (uint8_t)value);
      break;
    }
    case 2: {
      if(is_signed) _encode_fixed_int16_to_scale(&s_e, (int16_t)value);
      else _encode_fixed_uint16_to_scale(&s_e, (uint16_t)value);
      break;
    }
    case 4: {
      if(is_signed) _encode_fixed_int32_to_scale(&s_e, (int32_t)value);
      else _encode_fixed_uint32_to_scale(&s_e, (uint32_t)value);
      break;
    }
    case 8: {
      if(is_signed) _encode_fixed_int64_to_scale(&s_e, (int64_t)value);
      else _encode_fixed_uint64_to_scale(&s_e, (uint64_t)value);
      break;
    }
    default: {
      fprintf(stderr, "Invalid Byte Width for Fixed Int: %zu\n", width);
      assert(1==0);
    }
  }
  hex = decode_scale_fixed_to_hex(&s_e);
  uint64_t output = 0;
  decode_scale_fixed_int((void*)&output, &s_e);
  printf("output: %llu %llu ", value, output);
  assert(value == output);
  free(hex);
  memset(serialized, 0, 64 * sizeof(uint8_t));
  serialized_len = 0;
  serialize_fixed_int(serialized, &serialized_len, &s_e);
  assert_hash_matches_bytes(serialized, serialized_len, expected_hex_serialized);
  if(is_signed) printf(" -- Decoded: <%lld>\n", (int64_t)output);
  else printf(" -- Decoded: <%llu>\n", output);

}

static void run_test_fixed_hex(const char *hex, uint8_t is_signed, uint64_t expected) {
  _scale_fixed_int s_e;
  printf("\t\tRe-Encoding: <%s> --- ", hex);
  _encode_fixed_hex_to_scale(&s_e, is_signed, hex);
  char *hex_out = decode_scale_fixed_to_hex(&s_e);
  printf("Output: <%s> ", hex_out);
  assert(hex_out);
  assert(strcasecmp(hex, hex_out) == 0);
  assert(strlen(hex) == strlen(hex_out));
  if(is_signed) {
    int64_t out = 0;
    decode_scale_fixed_int((void*)&out, &s_e);
    printf("Yields: %lld\n", out);
    assert((int64_t)expected == out);
  } else {
    uint64_t out = 0;
    decode_scale_fixed_int((void*)&out, &s_e);
    printf("Yields: %llu\n", out);
    assert(expected == out);
  }
}

int run_fixed_int_test() {


    printf("\tEncoding Ints to Fixed Scale:\n");
    run_test(69, 1, 1, "45"); //int8_t
    run_test(1, 1, 0, "01"); //uint8_t
    run_test(128, 2, 1, "8000");  //int16_t
    run_test(42, 2, 0, "2A00"); //uint16_t
    run_test(1030404040, 4, 1, "C8B76A3D"); //int32_t
    run_test(4294967294, 4, 0, "FEFFFFFF");
    run_test(16777215, 4, 0, "FFFFFF00"); //uint32_t
    run_test(4294967296, 8, 0, "0000000001000000"); //uint64_t

    printf("\tEncoding Fixed Scale Hex to Fixed Scale:\n");
    run_test_fixed_hex("0x45", 0, 69);
    run_test_fixed_hex("0x8000", 0, 128);
    run_test_fixed_hex("0x2A", 0, 42);
    run_test_fixed_hex("0x2A00", 0, 42);
    run_test_fixed_hex("0xC8B76A3D", 1, 1030404040);
    run_test_fixed_hex("0xFFFFFF00", 0, 16777215);


  return 0;
}
