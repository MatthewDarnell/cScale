/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../scale.h"


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


int run_fixed_int_test() {

    _scale_fixed_int s_e;
    uint8_t serialized[64] = { 0 };
    uint64_t serialized_len = 0;


    int8_t one = 69;
    uint16_t two = 42; //00000001  00000001
    uint32_t three = 16777215; //00000001  00000001  00000001  00000001

    int i;

    _encode_fixed_int8_to_scale(&s_e, one);
    char *hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    int8_t output = 0;
    if(decode_scale_fixed_int((void*)&output, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 1\n");
    }

    fprintf(stderr, "int8 <69> -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%d)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output
    );

    free(hex);

    memset(serialized, 0, 64 * sizeof(uint8_t));
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &s_e);
    printf("Serialized.(%d). Length.(%llu). Value: 0x", one, serialized_len);
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n\n");



    _encode_fixed_uint16_to_scale(&s_e, two);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    uint16_t output2 = 0;
    if(decode_scale_fixed_int((void*)&output2, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 2\n");
    }

    printf("uint16 <42> -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%u)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output2
    );
    free(hex);

    memset(serialized, 0, 64 * sizeof(uint8_t));
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &s_e);
    printf("Serialized.(%d). Length.(%llu). Value: 0x", two, serialized_len);
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n\n");





    _encode_fixed_uint32_to_scale(&s_e, three);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    uint32_t output3 = 0;
    if(decode_scale_fixed_int((void*)&output3, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 3\n");
    }
    printf("uint32 <16777215> -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%u)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output3
    );
    free(hex);

    memset(serialized, 0, 64 * sizeof(uint8_t));
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &s_e);
    printf("Serialized.(%d). Length.(%llu). Value: 0x", three, serialized_len);
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n\n");



     int16_t four = 128;
    _encode_fixed_int16_to_scale(&s_e, four);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    int16_t output4 = 0;
    if(decode_scale_fixed_int((void*)&output4, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 4\n");
    }

    printf("int16 <128> -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%d)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output4
    );
    free(hex);

    memset(serialized, 0, 64 * sizeof(uint8_t));
    serialized_len = 0;
    serialize_fixed_int(serialized, &serialized_len, &s_e);
    printf("Serialized.(%d). Length.(%llu). Value: 0x", four, serialized_len);
    for(i = 0; i < serialized_len; i++) {
      printf("%02X", serialized[i]);
    }
    printf("\n\n");



    char *hex_string = "0x45";
    _encode_fixed_hex_to_scale(&s_e, true, hex_string);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    int8_t output6 = 0;
    if(decode_scale_fixed_int((void*)&output6, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 6\n");
    }
    printf("Decoding Hex String (0x45) -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%u)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output6
    );
    free(hex);



    hex_string = "0x8000";  //uint32
    _encode_fixed_hex_to_scale(&s_e, false, hex_string);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    int16_t output_sixteen = 0;
    if(decode_scale_fixed_int((void*)&output_sixteen, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 6\n");
    }
    printf("Decoding Hex String (0x8000) -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%u)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      hex,
      output_sixteen
    );
    free(hex);



    hex_string = "0xFFFFFF00";  //uint32
    _encode_fixed_hex_to_scale(&s_e, false, hex_string);
    hex = get_encoded_hex_from_scale_fixed_int(&s_e);
    uint32_t output5 = 0;
    if(decode_scale_fixed_int((void*)&output5, &s_e) < 0) {
      fprintf(stderr, "Failed to Decode 5\n");
    }
    printf("Decoding Hex String (0xFFFFFF00) -> "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" -> %s -> Decoded.(%u)\n",
      BYTE_TO_BINARY(s_e.data[0]),
      BYTE_TO_BINARY(s_e.data[1]),
      BYTE_TO_BINARY(s_e.data[2]),
      BYTE_TO_BINARY(s_e.data[3]),
      hex,
      output5
    );
    free(hex);


  return 0;
}
