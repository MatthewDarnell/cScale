/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "../scale.h"

int run_option_test() {
  scale_option option;

  scale_compact_int s_c;
  encode_compact_64(&s_c, (uint64_t)4611686018427387903);
  char *compact = decode_compact_to_hex(&s_c);
  free(compact);
  assert(encode_option_compact_int(&option, &s_c) == 0);
  compact = decode_compact_to_hex(&option.value._compact_int);
  printf("value=%s\n", compact);
  assert(strcasecmp(compact, "3FFFFFFFFFFFFFFF") == 0);
  free(compact);
  assert(option.value._compact_int.mode_upper_bits = 0x04);
  char *decoded = decode_option_to_hex(&option);
  assert(strcasecmp(decoded, "0x0113FFFFFFFFFFFFFF3F") == 0);
  free(decoded);


  scale_fixed_int fixed;
  encode_fixed_int_to_scale(&fixed, (uint32_t)4294967294);
  encode_option_fixed_int(&option, &fixed);
  char *fixed_hex = decode_scale_fixed_to_hex(&option.value._fixed_int);
  assert(strcasecmp(fixed_hex, "FEFFFFFF") == 0);
  free(fixed_hex);


  scale_boolean boolean;
  encode_boolean(&boolean, false);
  encode_option_boolean(&option, &boolean);  //pass NULL to set Option to `None`
  decoded = decode_option_to_hex(&option);
  printf("\tDecoded Boolean Option: <%s>\n", decoded);
  assert(strcasecmp(decoded, "0x02") == 0);
  free(decoded);


  return 0;
}
