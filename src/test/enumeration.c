/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../util/hex.h"
#include "../scale.h"

extern void struct_BoolTest_deserialize(void *structure_out, uint8_t *bytes, size_t len);
extern void struct_BoolTest_serialize(uint8_t* serialized, size_t *bytes, void *structure);
void struct_MyStruct_deserialize(void *structure_out, uint8_t *bytes, size_t len);
void struct_MyStruct_serialize(uint8_t* serialized, size_t *bytes, void *structure);

struct BoolTest {
  bool a;
  bool b;
  int8_t c;
  scale_structure scaleencoder;
};

struct MyStruct {
  uint32_t a;
  uint64_t b;
  struct BoolTest c;
  scale_structure scaleencoder;
};



int run_enumeration_test() {
  scale_enum_type CustomEnum;

  const char *strCustomEnum[][4] = {
    {"Int", "Bool", "Struct", "Compact32"},
    {"uint8_t", "bool", "", ""}
  };

  encode_scale_enum_type(&CustomEnum, 4, (char**)strCustomEnum[0], (char**)strCustomEnum[1]);

  printf("\tEncoding Boolean to Scale:\n");

  uint8_t enum_bytes[32] = { 0 };

  uint8_t bytes[32] = { 0 };
  uint64_t len = 0;

  char *type = "Int";
  uint8_t value = 42;
  scale_fixed_int v;
  encode_int_to_fixed_int_scale(&v, value);
  serialize_fixed_int(bytes, &len, &v);

  encode_enumeration(enum_bytes, &CustomEnum, type, bytes, (size_t*)&len);

  char *hex = _byte_array_to_hex(enum_bytes, len+1);
  printf("\t\tFixed Int Serialized: %s(%u) --- <0x%s>\n", type, value, hex);
  assert(strcasecmp(hex, "002a") == 0);
  free(hex);


  type = "Bool";
  bool b_value = true;
  memset(enum_bytes, 0, 32);
  memset(bytes, 0, 32);
  len = 1;
  scale_boolean b;
  encode_boolean(&b, b_value);
  serialize_boolean(bytes, &b);
  encode_enumeration(enum_bytes, &CustomEnum, type, bytes, (size_t*)&len);
  hex = _byte_array_to_hex(enum_bytes, len+1);
  printf("\t\tBool Serialized: %s(%u) --- <0x%s>\n", type, b_value, hex);
  assert(strcasecmp(hex, "0101") == 0);
  free(hex);

  memset(enum_bytes, 0, 32);
  memset(bytes, 0, 32);
  type = "Struct";


  struct BoolTest bool_test;
  bool_test.a = false;
  bool_test.b = true;
  bool_test.c = 42;
  bool_test.scaleencoder.serialize = &struct_BoolTest_serialize;
  bool_test.scaleencoder.deserialize = &struct_BoolTest_deserialize;

  struct MyStruct t;
  t.a = 1;
  t.b = 2;
  t.c = bool_test;
  t.scaleencoder.serialize = &struct_MyStruct_serialize;
  t.scaleencoder.deserialize = &struct_MyStruct_deserialize;

  printf("\t\tSerializing Struct {\n\t\t\ta: <u32> = %u\n\t\t\tb: <u64> = %llu\n\t\t\tc: { \n\t\t\t\ta: <bool> = %d\n\t\t\t\tb: <bool> = %d\n\t\t\t\tc: <int8_t> = %d\n\t\t\t} \n    \t\t\t}\n",
  t.a, t.b, t.c.a, t.c.b, t.c.c);

  t.scaleencoder.serialize(bytes, (size_t*)&len, &t);
  encode_enumeration(enum_bytes, &CustomEnum, type, bytes, (size_t*)&len);
  hex = _byte_array_to_hex(enum_bytes, len+1);
  printf("\t\tStruct Enum Serialized: %s --- <0x%s>\n", type, hex);
  assert(strcasecmp(hex, "0201000000020000000000000000012A") == 0);
  free(hex);

  memset(bytes, 0, 32);
  uint16_t offset = 0;
  decode_enumeration(bytes, &offset, &CustomEnum, enum_bytes, (size_t*)&len);
  hex = _byte_array_to_hex(bytes, len);
  printf("\t\tStruct Enum Deserialzied: %s --- <0x%s>\n", type, hex);
  assert(strcasecmp(hex, "01000000020000000000000000012A") == 0);
  free(hex);

  struct MyStruct t2 = { 0 };
  t2.scaleencoder.serialize = &struct_MyStruct_serialize;
  t2.scaleencoder.deserialize = &struct_MyStruct_deserialize;
  t2.c.scaleencoder.serialize = &struct_BoolTest_serialize;
  t2.c.scaleencoder.deserialize = &struct_BoolTest_deserialize;


  t2.scaleencoder.deserialize(&t2, bytes, len);

  printf("\t\tDeserialized Struct From Enum {\n\t\t\ta: <u32> = %u\n\t\t\tb: <u64> = %llu\n\t\t\tc: { \n\t\t\t\ta: <bool> = %d\n\t\t\t\tb: <bool> = %d\n\t\t\t\tc: <int8_t> = %d\n\t\t\t} \n    \t\t\t}\n",
  t2.a, t2.b, t2.c.a, t2.c.b, t2.c.c);
  return 0;
}
