/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../scale.h"
#include "../util/hex.h"
extern void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex);

int run_option_test() {
  scale_compact_int s_c = SCALE_COMPACT_INT_INIT;
  encode_uint64_to_compact_int_scale(&s_c, (uint64_t)4611686018427387903);
  uint8_t compact_serialized[64] = { 0 };
  size_t compact_serialized_len = 0;
  serialize_compact_int(compact_serialized, (uint64_t*)&compact_serialized_len, &s_c);
  uint8_t compact_option_serialized[64] = { 0 };
  size_t compact_option_serialized_len = 0;
  serialize_scale_option(compact_option_serialized, &compact_option_serialized_len, Some, compact_serialized, compact_serialized_len);
  char *compact_hex = cscale_byte_array_to_hex(compact_option_serialized, compact_option_serialized_len);
  printf("\tOption Compact Hex: %s\n", compact_hex);
  assert(strcasecmp(compact_hex, "0113FFFFFFFFFFFFFF3F") == 0);
  free(compact_hex);


  scale_vector vec = SCALE_VECTOR_INIT;

  scale_fixed_int one, two;
  encode_int_to_fixed_int_scale(&one, (int8_t)1);
  encode_int_to_fixed_int_scale(&two, (int8_t)-1);

  uint8_t fixed_int_one_serialized[16] = { 0 };
  size_t fixed_int_one_serialized_len = 0;
  uint8_t fixed_int_two_serialized[16] = { 0 };
  size_t fixed_int_two_serialized_len = 0;

  serialize_fixed_int(fixed_int_one_serialized, (uint64_t*)&fixed_int_one_serialized_len, &one);
  serialize_fixed_int(fixed_int_two_serialized, (uint64_t*)&fixed_int_two_serialized_len, &two);

  uint8_t option_fixed_int_one_serialized[16] = { 0 };
  size_t option_fixed_int_one_serialized_len = 0;
  uint8_t option_fixed_int_two_serialized[16] = { 0 };
  size_t option_fixed_int_two_serialized_len = 0;

  serialize_scale_option(option_fixed_int_one_serialized, &option_fixed_int_one_serialized_len, Some, fixed_int_one_serialized, fixed_int_one_serialized_len);
  serialize_scale_option(option_fixed_int_two_serialized, &option_fixed_int_two_serialized_len, Some, fixed_int_two_serialized, fixed_int_two_serialized_len);

  uint8_t option_fixed_int_three_serialized[16] = { 0 };
  size_t option_fixed_int_three_serialized_len = 0;

  serialize_scale_option(option_fixed_int_three_serialized, &option_fixed_int_three_serialized_len, None, NULL, 0);

  enum scale_option op1, op2, op3;
  deserialize_scale_option(&op1, option_fixed_int_one_serialized);
  deserialize_scale_option(&op2, option_fixed_int_two_serialized);
  deserialize_scale_option(&op3, option_fixed_int_three_serialized);


  push_vector(&vec, option_fixed_int_one_serialized, option_fixed_int_one_serialized_len);
  push_vector(&vec, option_fixed_int_two_serialized, option_fixed_int_two_serialized_len);
  push_vector(&vec, option_fixed_int_three_serialized, option_fixed_int_three_serialized_len);

  uint8_t serialized_vector[32] = { 0 };
  size_t serialized_vector_len = 0;
  serialize_vector(serialized_vector, &serialized_vector_len, &vec);

  printf("\tOption<Vec<i8>>: [Some(1i8), Some(-1), None] ->\t");

  //https://github.com/paritytech/parity-scale-codec/blob/master/src/codec.rs#L1336
  assert_hash_matches_bytes(serialized_vector, serialized_vector_len, "0c010101ff00");


  cleanup_vector(&vec);



  return 0;
}
