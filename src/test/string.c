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
    deserialize_string(&scale_string_deserialized, serialized);
    cleanup_string(&scale_string);
    printf(">\tDeserialized: <%s>\tLength.(%lu)\n", (char*)scale_string_deserialized.data, utf8len((void*)scale_string_deserialized.data));
  }
  printf("\tSerializing Vec<String>: ");
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

  //Test reading Vec<String>
  printf("\tDeserializing Vec<String>: ");
  memset(&VecOfStrings, 0, sizeof(scale_vector));
  size_t num_strings = 0;
  size_t bytes_read = deserialize_vector_of_strings(&VecOfStrings, &num_strings, bytes);
  assert(num_strings == 4);
  assert(bytes_read == 90);
  uint8_t bytes_read_back[128] = { 0 };
  bytes_len = 0;
  serialize_vector(bytes_read_back, &bytes_len, &VecOfStrings);
  assert_hash_matches_bytes(bytes_read_back, bytes_len,  "101848616d6c657450d0"
                                                              "92d0bed0b9d0bdd0b020"
                                                              "d0b820d0bcd0b8d18030"
                                                              "e4b889e59bbde6bc94e4"
                                                              "b989bcd8a3d98ed984d9"
                                                              "92d98120d984d98ed98a"
                                                              "d992d984d98ed8a920d9"
                                                              "88d98ed984d98ed98ad9"
                                                              "92d984d98ed8a9e2808e");

  cleanup_vector(&VecOfStrings);
  printf("\n");


  //Utf8 string
  uint8_t *data_str = utf8dup("📚Hamlet"); //4 byte utf8 + six single byte ascii chars = 10 bytes
  size_t vecs_len = strlen((char*)data_str);
  scale_vector utfvec = { 0 };
  create_string(&utfvec, data_str, vecs_len);
  uint8_t data[64] = { 0 };
  serialize_string(data, &vecs_len, &utfvec);
  assert(vecs_len == 11);  //vecs_len now contains str length + compact byte length for prefix num elements
  printf("\tVerifying Utf8 String <%s>, Serialized: ", data_str);
  free(data_str);
  assert_hash_matches_bytes(data, vecs_len, "28f09f939a48616d6c6574");
  printf("\n");
  cleanup_vector(&utfvec);



  //try to read first N chars of a utf8 String of Vectors
  const char *str_of_vecs = "1848616d6c6574"  //Hamlet
                            "50d092d0bed0b9d0bdd0b020d0b820d0bcd0b8d180";  //Война и мир
  cscale_hex_to_data(str_of_vecs, &data_str);
  vecs_len = strlen("Hamlet");
  memset(&utfvec, 0, sizeof(scale_vector));
  memset(data, 0, 64);
  create_string(&utfvec, data_str+1, vecs_len);
  serialize_string(data, &vecs_len, &utfvec);

  assert(vecs_len == 7);  //vecs_len now contains str length + compact byte length for prefix num elements
  printf("\n\tVerifying Utf8 String of Vectors can Retrieve First String: <%s>, Serialized: ", data_str);
  free(data_str);
  assert_hash_matches_bytes(data, vecs_len, "1848616d6c6574");
  printf("\n");


  //Testing read already serialized utf8 strings
  const char *contains_encoded_vec = "011c4163636f756e740101";  //1c 41 63 63 6f 75 6e 74
  uint8_t account[] = { 0x1c, 0x41, 0x63, 0x63, 0x6f, 0x75, 0x6e, 0x74 };
  scale_vector encoded_vec = { 0 };
  size_t offset = 1;
  cscale_hex_to_data(contains_encoded_vec, &data_str);
  size_t utf8_str_len = deserialize_string(&encoded_vec, &data_str[offset]);
  printf("\tVerifying Reading Utf8 String returns Correct Byte Length, (%s) - (%lu)\n", encoded_vec.data, strlen((char*)encoded_vec.data));
  assert(utf8_str_len == 8);  //utf8_str_len contains raw utf8 len + compact prefix byte length from vector
  assert(memcmp(encoded_vec.data, &account[1], 7) == 0);
  free(data_str);
  cleanup_vector(&encoded_vec);


  const char *second_encoded_vec = "30543a3a4163636f756e74496494";
  scale_vector encoded_vec_2 = { 0 };
  cscale_hex_to_data(second_encoded_vec, &data_str);
  size_t utf8_str_len_2 = deserialize_string(&encoded_vec_2, data_str);
  printf("\tVerifying Reading Utf8 String returns Correct Byte Length, (%s) - (%lu)\n", encoded_vec_2.data, strlen((char*)encoded_vec_2.data));
  assert(utf8_str_len_2 == 13);
  free(data_str);
  cleanup_vector(&encoded_vec_2);

  return 0;
}
