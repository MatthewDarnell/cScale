/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "../util/hex.h"

extern int run_compact_test();
extern int run_fixed_int_test();
extern int run_boolean_test();

void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex) {
  size_t i, hex_offset = 0;
  printf("Comparing: <%s> / <", hex);
  print_hash(bytes, byte_len);
  printf("> ");
  assert(strlen(hex) == byte_len * 2);  //otherwise 2A could match with 2A00 for fixed length int
  for(i = 0; i < byte_len; i++) {
    char *pHex = (char*)hex + hex_offset;
    char temp[4] = { 0 };
    memcpy(temp, pHex, 2);
    uint8_t hex_byte = strtol(temp, NULL, 16);
    assert(bytes[i] == hex_byte);
    hex_offset += 2;
  }

}

int main() {
  printf("\nRunning Fixed Int Tests\n");
  run_fixed_int_test();
  printf("\nRunning Compact Tests\n");
  run_compact_test();
  printf("\nRunning Boolean Tests\n");
  run_boolean_test();
  printf("\nDone\n");
  return 0;
}
