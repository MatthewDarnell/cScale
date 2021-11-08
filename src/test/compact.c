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
#include "../util/hex.h"
extern void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex);

static void run_test(uint64_t value, size_t width, const char *expected_hex_serialized) {
    scale_compact_int s_e = SCALE_COMPACT_INT_INIT;

    printf("\t\tEncoding <%llu>: ", value);

    switch (width) {
      case 1: {
        encode_uint8_to_compact_int_scale(&s_e, (uint8_t)value);
        break;
      }
      case 2: {
        encode_uint16_to_compact_int_scale(&s_e, (uint16_t)value);
        break;
      }
      case 4: {
        encode_uint32_to_compact_int_scale(&s_e, (uint32_t)value);
        break;
      }
      case 8: {
        encode_uint64_to_compact_int_scale(&s_e, value);
        break;
      }
      default: {
        fprintf(stderr, "Invalid Byte Width for Compact Int: %zu\n", width);
        assert(1==0);
      }
    }

  printf("Verifying ByteWidth=(%u)\t", (unsigned)width);
  size_t num_bytes = compact_int_get_byte_length(&s_e);
  printf("Num Bytes: %u\n", (unsigned)num_bytes);
  if(width < 8) {
    assert(num_bytes == width);
  } else {  //bignum, num_bytes are variable but should be, at maximum, width
    assert(num_bytes >= 4 && num_bytes <= width);
  }

  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;

  assert(serialize_compact_int(serialized, &serialized_len, &s_e) == 0);
  cleanup_scale_compact_int(&s_e);

  assert(serialized_len > 0);

  //Read Byte Stream Into Compact
  scale_compact_int read_from_bytes = SCALE_COMPACT_INT_INIT;
  size_t bytes_read = read_compact_int_from_data(&read_from_bytes, serialized);
  if(s_e.mode == SCALE_COMPACT_BIGNUM) {
    assert(bytes_read == num_bytes+1);  //BigNum Compact stores data from index 1, compact_int_get_byte_length does not include prefix
  } else {
    assert(bytes_read == num_bytes);
  }
  cleanup_scale_compact_int(&read_from_bytes);


  char *hex = cscale_byte_array_to_hex(serialized, serialized_len);
  if(!hex) {
    fprintf(stderr, "Failed to Allocate Memory to Convert Data to hex!\n");
    assert(1 == 0);
  }

  printf("Comparing: <%s> / <%s>\n", expected_hex_serialized, hex);
  assert(strcasecmp(expected_hex_serialized, hex) == 0);
  free(hex);
}

void run_compact_128(const char *value, const char *expected_hex_serialized) {
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  printf("\n\t\tEncoding <%s>: ", value);
  assert(encode_u128_string_to_compact_int_scale(&compact, (char*)value) == 0);
  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;
  serialize_compact_int(serialized, &serialized_len, &compact);
  cleanup_scale_compact_int(&compact);
  char *str_serialized = cscale_byte_array_to_hex(serialized, serialized_len);
  assert(str_serialized);
  free(str_serialized);
  assert_hash_matches_bytes(serialized, serialized_len, expected_hex_serialized);
}

static void run_test_fixed_hex(const char *hex, uint64_t expected) {
  scale_compact_int s_e = SCALE_COMPACT_INT_INIT;
  printf("\t\tRe-Encoding: <%s> --- ", hex);
  assert(encode_compact_hex_to_scale(&s_e, hex) == 0);
  char *hex_out = decode_compact_to_hex(&s_e);
  assert(hex_out);
  uint64_t value = strtoull(hex_out, NULL, 16);
  printf("Output: <0x%s> Yields: %llu\n", hex_out, value);
  free(hex_out);
  assert(expected == value);
  cleanup_scale_compact_int(&s_e);
}

static void run_test_fixed_hex_128(const char *hex, const char *expected) {
  scale_compact_int s_e = SCALE_COMPACT_INT_INIT;
  printf("\t\tRe-Encoding (128 bit hex value): <%s> --- ", hex);
  assert(encode_compact_hex_to_scale(&s_e, hex) == 0);
  char *hex_out = decode_compact_to_hex(&s_e);
  assert(hex_out);
  printf("Output: <0x%s>\n", hex_out);
  assert(strcasecmp(expected, hex_out) == 0);
  free(hex_out);
  cleanup_scale_compact_int(&s_e);
}

static void run_test_read_data_to_compact(uint8_t *data, size_t byte_length, uint64_t expected) {
  //Read Byte Stream Into Compact
  scale_compact_int read_from_bytes = SCALE_COMPACT_INT_INIT;
  size_t bytes_read = read_compact_int_from_data(&read_from_bytes, data);
  uint64_t read_back_byte_length = compact_int_get_byte_length(&read_from_bytes);
  uint64_t value = decode_compact_to_u64(&read_from_bytes);

  if(read_from_bytes.mode == SCALE_COMPACT_BIGNUM) {
    printf("\t\tReading Bignum Compact Value: <");
    cscale_print_hash(data, byte_length);
    printf("> As Data Stream. Got (%u) Bytes, Expected.(%u)\n", (unsigned)bytes_read, (unsigned)byte_length);
    assert(bytes_read == byte_length+1);  //BigNum Compact stores data from index 1, compact_int_get_byte_length does not include prefix
  } else {
    const char *mode = read_from_bytes.mode == SCALE_COMPACT_SINGLE_BYTE ? "Single" :
      (read_from_bytes.mode == SCALE_COMPACT_TWO_BYTE ? "Two" : "Four");
    printf("\t\tReading %s Byte Compact Value: <", mode);
    cscale_print_hash(data, byte_length);
    printf("> As Data Stream. Got (%u) Bytes, Expected.(%u).\tVerifying Read Back Value.(%llu) == (%llu)\n", (unsigned)bytes_read, (unsigned)byte_length, value, expected);
    assert(bytes_read == byte_length);
  }
  assert(byte_length == read_back_byte_length);
  assert(value == expected);
  cleanup_scale_compact_int(&read_from_bytes);
}

static void run_test_read_data_128_to_compact(uint8_t *data, size_t byte_length, const char *expected_raw_hex_value, const char *expected) {
  //Read Byte Stream Into Compact
  scale_compact_int read_from_bytes = SCALE_COMPACT_INT_INIT;
  size_t bytes_read = read_compact_int_from_data(&read_from_bytes, data);
  uint64_t read_back_byte_length = compact_int_get_byte_length(&read_from_bytes);

  printf("\n\t\tReading u128 Bignum Compact Value: <");
  cscale_print_hash(data, byte_length+1);
  printf("> As Data Stream. Got (%u) Bytes, Expected.(%u)\n", (unsigned)bytes_read, (unsigned)byte_length);

  char *value = decode_compact_to_hex(&read_from_bytes);
  assert(value);
  printf("\t\t\tVerifying Raw Hex Value.(%s)", value);
  assert(strcasecmp(expected_raw_hex_value, value) == 0);
  free(value);

  assert(bytes_read == byte_length);  //BigNum Compact stores data from index 1, compact_int_get_byte_length does not include prefix
  assert(byte_length == read_back_byte_length);

  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;
  serialize_compact_int(serialized, &serialized_len, &read_from_bytes);
  char *str_serialized = cscale_byte_array_to_hex(serialized, serialized_len);
  assert(str_serialized);
  printf("\n\t\t\tVerifying Serialized Hex Value.(%s)\n", str_serialized);
  assert(strcasecmp(expected, str_serialized) == 0);
  free(str_serialized);

  cleanup_scale_compact_int(&read_from_bytes);
}

int run_compact_test() {

  printf("\tEncoding Ints to Compact Scale:\n");
  run_test(127, 2, "fd01"); //uint16_t
  run_test(60, 1, "f0"); //uint8_t
  run_test(254, 2, "f903"); //uint16_t
  run_test(65535, 4, "feff0300"); //uint32_t
  run_test(16161616, 4, "426dda03"); //uint32_t max 32 uint compact: 2^30 - 1
  run_test(100000000000000, 8, "0b00407a10f35a"); //uint64_t max 64 uint compact: 2^62 - 1
  run_test(4611686018427387903, 8, "13ffffffffffffff3f"); //uint64_t max 64 uint compact: 2^62 - 1

  printf("\tEncoding Hex to Compact Scale:");
  run_compact_128("0x3fffffffffffffffffffffffffffffff", "33ffffffffffffffffffffffffffffff3f"); //85070591730234615865843651857942052863
  run_compact_128("0x0EFBBEEFBBEEFBBEEFBBEEFBBE003FDF", "33df3f00befbeebbefbefbeebbefbefb0e"); //19916331103999208699774735698886672351
  run_compact_128("0x1C8E02B7B2367C5E1C8A7787CB3816C5", "33c51638cb87778a1c5e7c36b2b7028e1c"); //37955745154616738430225574257755690693
  run_compact_128("0x01", "04"); //1

  printf("\n\tEncoding Compact Scale Hex to Compact Scale:\n");
  run_test_fixed_hex("0xFD01", 127);
  run_test_fixed_hex("0xf0", 60);
  run_test_fixed_hex("0xf903", 254);
  run_test_fixed_hex("0x426dda03", 16161616);
  run_test_fixed_hex("0x13ffffffffffffff3f", 4611686018427387903);
  run_test_fixed_hex_128("33ffffffffffffffffffffffffffffff3f", "3fffffffffffffffffffffffffffffff");


  printf("\n\tReading Byte Stream Into Compact Scale:\n");
  uint8_t array[32] = { 0 };
  array[0] = 0xfd;
  array[1] = 0x01;
  run_test_read_data_to_compact(array, 2, 127);
  array[0] = 0xf0;
  array[1] = 0;
  run_test_read_data_to_compact(array, 1, 60);
  //Byte Stream Containing More Data than Compact
  memset(array, 0, 4);
  array[4] = 0x01;
  array[5] = 0x42; // <--begin compact
  array[6] = 0x6d;
  array[7] = 0xda;
  array[8] = 0x03; // <--end compact
  array[9] = 0xda;
  array[10] = 0xda;
  run_test_read_data_to_compact(&array[5], 4, 16161616);
  memset(array, 0, 11);

  memset(array, 0, 3);
  array[3] = 0xa4;
  array[4] = 0x01;
  array[5] = 0x33; // <--begin compact
  array[6] = 0xdf;
  array[7] = 0x3f;
  array[8] = 0x00;
  array[9] = 0xbe;
  array[10] = 0xfb;
  array[11] = 0xee;
  array[12] = 0xbb;
  array[13] = 0xef;
  array[14] = 0xbe;
  array[15] = 0xfb;
  array[16] = 0xee;
  array[17] = 0xbb;
  array[18] = 0xef;
  array[19] = 0xbe;
  array[20] = 0xfb;
  array[21] = 0x0e; // <--end compact
  array[22] = 0x0e;
  array[23] = 0x0e;
  array[24] = 0x1f;
  run_test_read_data_128_to_compact(&array[5], 16, "0EFBBEEFBBEEFBBEEFBBEEFBBE003FDF", "33df3f00befbeebbefbefbeebbefbefb0e");
  memset(array, 0, 25);

  array[2] = 0x04; //<--begin compact
  array[3] = 0x05;
  run_test_read_data_128_to_compact(&array[2], 1, "01", "04");
  memset(array, 0, 4);

  return 0;
}
