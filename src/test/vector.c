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


static void run_vector_test_read_from_bytes(uint8_t width, uint8_t *bytes, size_t expected_num_elements, size_t expected_bytes_read) {
  scale_vector vector = SCALE_VECTOR_INIT;
  printf("\tReading Vector From Unknown Length of Bytes: ");
  size_t bytes_read = read_vector_from_data(&vector, width, bytes);
  uint64_t num_elements = decode_compact_to_u64(&vector.prefix_num_elements);
  printf("\n\t\tNum bytes read.(%u) vs expected.(%u)", (unsigned)bytes_read, (unsigned)expected_bytes_read);
  printf("\n\t\tNum elements read.(%llu) vs expected.(%u)\n", (unsigned long long)num_elements, (unsigned)expected_num_elements);
  assert(bytes_read == expected_bytes_read);
  assert(num_elements == expected_num_elements);
  cleanup_vector(&vector);
}
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
  size_t data_len_read_back = read_vector_from_data(&decoded, sizeof(uint16_t), bytes);

  assert(data_len == data_len_read_back);

  uint64_t num_elems = decode_compact_to_u64(&decoded.prefix_num_elements);
  printf("\tNumber Elements Read.(%llu)", (unsigned long long)num_elems);
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

  uint8_t array[32] = { 0 };
  array[1] = 0x18; // <--begin vector
  array[2] = 0x04;
  array[3] = 0x00;
  array[4] = 0x08;
  array[5] = 0x00;
  array[6] = 0x0f;
  array[7] = 0x00;
  array[8] = 0x10;
  array[9] = 0x00;
  array[10] = 0x17;
  array[11] = 0x00;
  array[12] = 0x2a;
  array[13] = 0x00; // <--end vector
  array[14] = 0x01;
  run_vector_test_read_from_bytes(2, &array[1], 6, 13);

  memset(array, 0, 15);
  array[1] = 0x28; // <--begin vector
  array[2] = 0xf0;
  array[3] = 0x9f;
  array[4] = 0x93;
  array[5] = 0x9a;
  array[6] = 0x48;
  array[7] = 0x61;
  array[8] = 0x6d;
  array[9] = 0x6c;
  array[10] = 0x65;
  array[11] = 0x74; // <--end vector
  array[12] = 0x2a;
  array[13] = 0x0a;
  array[14] = 0x01;
  run_vector_test_read_from_bytes(1, &array[1], 10, 11);
  memset(array, 0, 15);
  return 0;
}
