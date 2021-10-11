/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "../util/hex.h"
#include "../scale.h"

int run_vector_test() {
  scale_vector vector = SCALE_VECTOR_INIT;
  uint16_t values[6] = { 4, 8, 15, 16, 23, 42 };
  int i;
  uint8_t serialized[64] = { 0 };
  uint64_t serialized_len = 0;
  printf("\tPushing to Vector<u16>: [ ");
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
  uint8_t bytes[256] = { 0 };
  size_t data_len = 0;
  serialize_vector(bytes, &data_len, &vector);
  printf("\n\tSerialized Vector<u16>: <");
  cscale_print_hash(bytes, data_len);
  printf(">\n");
  cleanup_vector(&vector);

  scale_vector decoded = SCALE_VECTOR_INIT;
  deserialize_vector(&decoded, bytes, data_len);


  uint64_t num_elems = decode_compact_to_u64(&decoded.prefix_num_elements);
  printf("\tNumber Elements Read.(%llu)", num_elems);
  assert(num_elems == 6);

  uint16_t output = 0;
  printf("\tDeserialized Vector: [ ");
  uint16_t offset = 0;

  uint8_t *b;
  scale_vector_foreach(&b, sizeof(uint16_t), &decoded) {
    output = 0;
    deserialize_fixed_int((void*)&output, b, 2, false);
    assert(output == values[offset++]);
    printf("%u ", output);
  }

  printf("]\n");
  cleanup_vector(&decoded);

  return 0;
}
