/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../util/hex.h"
#include "../scale.h"

int run_vector_test() {
  scale_vector vector = { 0 };
  uint16_t values[6] = { 4, 8, 15, 16, 23, 42 };
  int i;
  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;
  printf("\tPushing to Vector: [ ");
  for(i = 0; i < 6; i++) {
    scale_fixed_int fixed = { 0 };
    encode_int_to_fixed_int_scale(&fixed, values[i]);
    memset(serialized, 0, 64);
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &fixed);
    printf(" %u ", values[i]);
    push_vector(&vector, serialized, serialized_len);
  }
  printf("]\n");
  uint8_t bytes[128] = { 0 };
  size_t data_len = 0;
  serialize_vector(bytes, &data_len, &vector);
  printf("\n\tSerialized Vector: <");
  cscale_print_hash(bytes, data_len);
  printf(">\n");
  cleanup_vector(&vector);

  scale_vector decoded = { 0 };
  deserialize_vector(&decoded, bytes, &data_len);

  uint16_t output = 0;
  printf("\n\tDeserialized Vector: [ ");
  uint16_t offset = 0;
  for(i = 0; i < data_len; i+=2) {
    output = 0;
    deserialize_fixed_int((void*)&output, &decoded.data[i], 2, false);
    assert(output == values[offset++]);
    printf("%u ", output);
  }
  printf("]\n");

  return 0;
}