/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <locale.h>
#include "../util/utf8.h"
#include "../util/hex.h"
#include "../scale.h"

extern void assert_hash_matches_bytes(uint8_t* bytes, size_t byte_len, const char *hex);

int run_string_test() {
  setlocale(LC_ALL, "");

  const char *strings[4] = {
    (const char*)"Hamlet",
    (const char*)"Война и мир",
    (const char*)"三国演义",
    (const char*)"أَلْف لَيْلَة وَلَيْلَة‎"
  };

  int8_t i;
  scale_vector VecOfStrings = { 0 };

  for(i=0; i < 4; i++) {
    unsigned char serialized[64] = { 0 };
    size_t len = 0;
    void *string = utf8dup((char*)strings[i]);
    size_t utf_len = utf8size_lazy(string);
    scale_vector scale_string = { 0 }, scale_string_deserialized = { 0 };
    create_string(&scale_string, (unsigned char*)string, utf_len);
    free(string);
    serialize_string(serialized, &len, &scale_string);
    printf("\tSerializing: %s: <", (char*)strings[i]);
    cscale_print_hash(serialized, len);
    push_vector(&VecOfStrings, serialized, len);
    deserialize_string(&scale_string_deserialized, serialized, len);
    cleanup_string(&scale_string);
    printf(">\tDeserialized: <%s>\tLength.(%lu)\n", (char*)scale_string_deserialized.data, utf8len((void*)scale_string_deserialized.data));
  }
  printf("\tChecking Vec<String>: ");
  uint8_t bytes[128] = { 0 };
  size_t bytes_len = 0;
  serialize_vector(bytes, &bytes_len, &VecOfStrings);
  cleanup_vector(&VecOfStrings);
  assert(bytes_len == 90);
  assert_hash_matches_bytes(bytes, bytes_len, "101848616d6c657450d0"  //https://github.com/paritytech/parity-scale-codec/blob/master/src/codec.rs#L1395
                                                   "92d0bed0b9d0bdd0b020"
                                                   "d0b820d0bcd0b8d18030"
                                                   "e4b889e59bbde6bc94e4"
                                                   "b989bcd8a3d98ed984d9"
                                                   "92d98120d984d98ed98a"
                                                   "d992d984d98ed8a920d9"
                                                   "88d98ed984d98ed98ad9"
                                                   "92d984d98ed8a9e2808e");
  printf("\n");
  return 0;
}
