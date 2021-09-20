/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

static void run_test(uint64_t value, size_t width, const char *expected_hex_serialized) {
    _scale_compact_int s_e;

    printf("\t\tEncoding <%llu>: ", value);

    switch (width) {
      case 1: {
        _encode_compact_8(&s_e, (uint8_t)value);
        break;
      }
      case 2: {
        _encode_compact_16(&s_e, (uint16_t)value);
        break;
      }
      case 4: {
        _encode_compact_32(&s_e, (uint32_t)value);
        break;
      }
      case 8: {
        _encode_compact_64(&s_e, value);
        break;
      }
      default: {
        fprintf(stderr, "Invalid Byte Width for Compact Int: %zu\n", width);
        assert(1==0);
      }
    }
    uint64_t output = 0;

    uint8_t serialized[64] = { 0 };
    uint64_t serialized_len = 0;

    assert(serialize_compact_int(serialized, &serialized_len, &s_e) == 0);
    assert(serialized_len > 0);

    char *hex = _byte_array_to_hex(serialized, serialized_len);
    printf("Comparing: <%s> / <%s>\n", expected_hex_serialized, hex);
    assert(strcasecmp(expected_hex_serialized, hex) == 0);
    free(hex);
    _cleanup_scale_compact_int(&s_e);
  }

void run_compact_128(const char *value, const char *expected_hex_serialized) {
  _scale_compact_int compact;
  printf("\n\t\tEncoding <%s>: ", value);
  assert(_encode_compact_128_from_hex(&compact, (char*)value) == 0);
 uint8_t serialized[64] = { 0 };
 uint64_t serialized_len = 0;
 serialize_compact_int(serialized, &serialized_len, &compact);
 char *str_serialized = _byte_array_to_hex(serialized, serialized_len);
 assert(str_serialized);
 free(str_serialized);

 assert_hash_matches_bytes(serialized, serialized_len, expected_hex_serialized);
 _cleanup_scale_compact_int(&compact);
}

static void run_test_fixed_hex(const char *hex, uint64_t expected) {
  _scale_compact_int s_e;
  printf("\t\tRe-Encoding: <%s> --- ", hex);
  assert(_encode_compact_hex_to_scale(&s_e, hex) == 0);
  char *hex_out = _decode_compact_to_hex(&s_e);
  assert(hex_out);
  uint64_t value = strtoul(hex_out, NULL, 16);
  printf("Output: <0x%s> Yields: %llu\n", hex_out, value);
  assert(expected == value);
  _cleanup_scale_compact_int(&s_e);
}

static void run_test_fixed_hex_128(const char *hex, const char *expected) {
  _scale_compact_int s_e;
  printf("\t\tRe-Encoding (128 bit hex value): <%s> --- ", hex);
  assert(_encode_compact_hex_to_scale(&s_e, hex) == 0);
  char *hex_out = _decode_compact_to_hex(&s_e);
  assert(hex_out);
  printf("Output: <0x%s>\n", hex_out);
  assert(strcasecmp(expected, hex_out) == 0);
  _cleanup_scale_compact_int(&s_e);
}


int run_compact_test() {

  printf("\tEncoding Ints to Compact Scale:\n");
  run_test(127, 1, "0xfd01"); //uint8_t
  run_test(60, 1, "0xf0"); //uint8_t
  run_test(254, 2, "0xf903"); //uint16_t
  run_test(16161616, 4, "0x426dda03"); //uint32_t max 32 uint compact: 2^30 - 1
  run_test(4611686018427387903, 8, "0x13ffffffffffffff3f"); //uint64_t max 64 uint compact: 2^62 - 1

  printf("\tEncoding Hex to Compact Scale:\n");
  run_compact_128("0x3fffffffffffffffffffffffffffffff", "33ffffffffffffffffffffffffffffff3f"); //85070591730234615865843651857942052863
  run_compact_128("0x0EFBBEEFBBEEFBBEEFBBEEFBBE003FDF", "33df3f00befbeebbefbefbeebbefbefb0e"); //19916331103999208699774735698886672351

  printf("\n\tEncoding Compact Scale Hex to Compact Scale:\n");
  run_test_fixed_hex("0xFD01", 127);
  run_test_fixed_hex("0xf0", 60);
  run_test_fixed_hex("0xf903", 254);
  run_test_fixed_hex("0x426dda03", 16161616);
  run_test_fixed_hex("0x13ffffffffffffff3f", 4611686018427387903);
  run_test_fixed_hex_128("33ffffffffffffffffffffffffffffff3f", "3fffffffffffffffffffffffffffffff");

  return 0;
}