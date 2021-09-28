/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <locale.h>
#include "../util/utf8.h"
#include "../scale.h"

extern void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex);



int run_tuple_test() {
  uint8_t bytes[32] = { 0 };
  size_t len = 0;

  _scale_compact_int compact = { 0 };
  _encode_compact(&compact, (uint8_t)3);

  _scale_boolean boolean = { 0 };
  _encode_boolean(&boolean, false);

  _serialize_as_tuple(bytes, &len, COMPACT_INT, (void*)&compact, BOOLEAN, (void*)&boolean);
  printf("\n\tTuple<Compact<u8>, bool> = (3, false)\t");
  assert_hash_matches_bytes(bytes, len, "0c00");

  len = 0;
  _scale_fixed_int fixed = { 0 };
  _encode_fixed_int_to_scale(&fixed, (uint32_t)645);

  _scale_vector vec = { 0 };
  create_string(&vec, (unsigned char*)"Hello, World!", strlen("Hello, World!"));

  _serialize_as_tuple(bytes, &len, FIXED_INT, (void*)&fixed, STRING, (void*)&vec);

  printf("\n\tTuple<u32, String> = (645, \"Hello, World!\")\t");
  assert_hash_matches_bytes(bytes, len, "850200003448656c6c6f2c20576f726c6421");
  printf("\n");

  return 0;
}