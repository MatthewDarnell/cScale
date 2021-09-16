/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Encode functions

int8_t serialize_compact_int(uint8_t *serialized, uint64_t *serialized_len, _scale_compact_int *compact_int_elem) {
  *serialized_len = 0;

  enum _scale_compact_int_mode mode = compact_int_elem->mode;
  uint8_t mode_upper_bits = compact_int_elem->mode_upper_bits;
  uint8_t *data = compact_int_elem->data;

  uint8_t lsb = mode_upper_bits; //0011111111
  lsb <<= 2; //11111100
  lsb |= (mode & 0x03); //11111100 & 00000011

  serialized[0] = lsb;

  if (mode == SINGLE_BYTE) {
    *serialized_len = 1;
  } else if (mode == TWO_BYTE) {
    serialized[1] = data[0];
    *serialized_len = 2;
  } else if (mode == FOUR_BYTE) {
    serialized[1] = data[2];
    serialized[2] = data[1];
    serialized[3] = data[0];
    *serialized_len = 4;
  } else {  //BIGNUM
    uint64_t num_bytes = mode_upper_bits + 4;
    uint64_t i = num_bytes - 1;
    uint64_t count = 1;
    while(i != 0 ) {
      serialized[count++] = data[i--];
    }
    serialized[count] = data[0];
    *serialized_len = num_bytes + 1;
  }
  return 0;
}

//These functions encode decimal values into a _scale_compact_int struct
int8_t _encode_compact_8(_scale_compact_int *compact_int_elem, uint8_t data) {
  if(data > 63) {  //Max 2^6 - 1 or 00111111
    fprintf(stderr, "Error Encoding Single Byte Compact. Data Too Large.(%u)\n", data);
    return -1;
  }
  memset(compact_int_elem, 0, sizeof(_scale_compact_int));
  compact_int_elem->mode = SINGLE_BYTE;
  uint8_t upper = data & 0x3F;
  compact_int_elem->mode_upper_bits = upper;
  return 0;
}

int8_t _encode_compact_16(_scale_compact_int *compact_int_elem, uint16_t data) {
  if(data < 63) {
    return _encode_compact_8(compact_int_elem, (uint8_t)data);
  }
  if(data > 16383) {  //Max 2^14 - 1 or 00111111 11111111
    fprintf(stderr, "Error Encoding Two Byte Compact. Data Too Large.(%u)\n", data);
    return -1;
  }
  memset(compact_int_elem, 0, sizeof(_scale_compact_int));
  compact_int_elem->mode = TWO_BYTE;
  uint8_t lsb = data & 0xFF;
  uint8_t upper = lsb & 0x3F;  //Get last 6 bits of lsb
  compact_int_elem->mode_upper_bits = upper;
  compact_int_elem->data = (uint8_t*)calloc(1, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Two Byte Compact!\n");
    return -1;
  }
  compact_int_elem->data[0] = (data >> 6) & 0xFF; //Get next byte
  return 0;
}
int8_t _encode_compact_32(_scale_compact_int *compact_int_elem, uint32_t data) {
  if(data < 63) {
    return _encode_compact_8(compact_int_elem, (uint8_t)data);
  } else if (data < 16383) {
    return _encode_compact_16(compact_int_elem, (uint16_t)data);
  }
  if(data > 1073741823) {  //Max 2^30 - 1 or 00111111 11111111 11111111 11111111
    fprintf(stderr, "Error Encoding Four Byte Compact. Data Too Large.(%u)\n", data);
    return -1;
  }
  memset(compact_int_elem, 0, sizeof(_scale_compact_int));
  compact_int_elem->mode = FOUR_BYTE;
  uint8_t lsb = data & 0xFF;
  uint8_t upper = lsb & 0x3F;  //Get last 6 bits of lsb
  compact_int_elem->mode_upper_bits = upper;
  compact_int_elem->data = (uint8_t*)calloc(3, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Four Byte Compact!\n");
    return -1;
  }
  compact_int_elem->data[0] = (data >> 6) & 0xFF;
  compact_int_elem->data[1] = (data >> (6 + 8)) & 0xFF;
  compact_int_elem->data[2] = (data >> (6 + 8 + 8)) & 0xFF;

  return 0;
}

int8_t _encode_compact_64(_scale_compact_int *compact_int_elem, uint64_t data) {
  if(data < 63) {
    return _encode_compact_8(compact_int_elem, (uint8_t)data);
  } else if (data < 16383) {
    return _encode_compact_16(compact_int_elem, (uint16_t)data);
  } else if(data < 1073741823) {
    return _encode_compact_32(compact_int_elem, (uint32_t)data);
  }
  if(data > 4611686018427387903) {  //Max: 2^62 -1 or 00111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    fprintf(stderr, "Error Encoding Eight Byte Compact. Data Too Large.(%llu)\n", data);
    return -1;
  }
  memset(compact_int_elem, 0, sizeof(_scale_compact_int));
  compact_int_elem->mode = BIGNUM;
  uint8_t byte_array[8] = { 0 };
  int i;
  for(i = 0; i < 8; i++) {
    byte_array[i] = data & 0xFF;
    data >>= 8;
  }

  uint8_t byte_len = 8;
  i = 0;
  while(byte_array[ 7 - i] == 0) {
    byte_len--;
    i++;
  }
  if(byte_len < 1) {
    fprintf(stderr, "Uint64 has 0 byte length, wtf\n");
    return -1;
  }

  compact_int_elem->data = (uint8_t*)calloc(byte_len, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Eight Byte Compact!\n");
    return -1;
  }

  for(i=0; i < byte_len; i++) {
    compact_int_elem->data[i] = byte_array[i];
  }

  compact_int_elem->mode_upper_bits = byte_len - 4;

  return 0;
}


int8_t _encode_compact_128_from_hex(_scale_compact_int *compact_int_elem, char *hex) {
  memset(compact_int_elem, 0, sizeof(_scale_compact_int));

  char *pHex = hex;
  if(pHex[0] == '0' && (pHex[1] == 'x' || pHex[1] == 'X')) {
    pHex +=2;
  }
  if(!is_valid_hex(pHex)) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Invalid Hex String.(%s)\n", hex);
    return -1;
  }

  size_t len = strlen(pHex);
  if(len > 32) {  //Max: 2^126 -1 or                     00111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Data Too Large.(%s)\n", pHex); // 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    return -1;
  }
  char *pEnd = pHex+1;
  int first_num = (int)strtol(pHex, &pEnd, 10);
  if(len == 32 && first_num > 3) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Data Too Large.(%s)\n", pHex);
    return -1;
  }
  compact_int_elem->mode = BIGNUM;

  uint64_t byte_array[16] = { 0 };
  int rounds = len / 16; //8 bytes at a time (uint64_t)
  int8_t offset = 0;
  int i;
  for(i = rounds-1; i >=0; i--) {
    char *pHexOffset = pHex + (16 * i);
    char chunk[18] = { 0 };
    strncpy(chunk, pHexOffset, 16);
    uint64_t data = (uint64_t)strtoull(chunk, NULL, 16);
    int j;
    for(j = 0; j < 8; j++) {
      byte_array[offset++] = data & 0xFF;
      data >>= 8;
    }
  }


  uint8_t byte_len = 16;
  i = 0;
  while(byte_array[ 15 - i] == 0) {
    byte_len--;
    i++;
  }

  if(byte_len < 1) {
    fprintf(stderr, "Uint128 has 0 byte length, wtf\n");
    return -1;
  }

  compact_int_elem->data = (uint8_t*)calloc(byte_len, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact!\n");
    return -1;
  }

  for(i=0; i < byte_len; i++) {
    compact_int_elem->data[i] = byte_array[i];
  }

  compact_int_elem->mode_upper_bits = byte_len - 4;

  return 0;
}


//Decode functions


//remember to free
char* _decode_compact_to_hex(_scale_compact_int *compact_int_elem) {

  if(compact_int_elem->mode == SINGLE_BYTE) {
    char *hex = (char*)calloc(1 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    snprintf(hex, 4, "%02X", compact_int_elem->mode_upper_bits);
    return hex;
  }

  else if(compact_int_elem->mode == TWO_BYTE) {
    char *hex = (char*)calloc(8 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    uint16_t value = compact_int_elem->data[0] << 6;
    value |= compact_int_elem->mode_upper_bits & 0xFF;
    snprintf(hex, 8, "%02X",  value);
    return hex;
  }


  else if(compact_int_elem->mode == FOUR_BYTE) {
    char *hex = (char*)calloc(32 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    uint32_t value = compact_int_elem->data[2] << (6 + 8 + 8);
    value |= compact_int_elem->data[1] << (6 + 8);
    value |= compact_int_elem->data[0] << 6;
    value |= compact_int_elem->mode_upper_bits & 0xFF;
    snprintf(hex, 16, "%02X",  value);
    return hex;
  }

  else if(compact_int_elem->mode == BIGNUM) {
    int8_t byte_len = compact_int_elem->mode_upper_bits + 4;
    char *hex = (char*)calloc(byte_len + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    int i;
    for(i = byte_len-1; i >= 0; i--) {
      char temp[16] = { 0 };
      snprintf(temp, 8, "%02X", compact_int_elem->data[i]);
      strcat(hex, temp);
    }
    return hex;
  }

  else {
   fprintf(stderr, "Failed to Decode 128 to hex. Unknown Mode\n");
   return NULL;
  }
  return NULL;
}
