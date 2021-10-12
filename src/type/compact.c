/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Encode functions
int8_t serialize_compact_int(uint8_t *serialized, uint64_t *serialized_len, scale_compact_int *compact_int_elem) {
  *serialized_len = 0;

  enum scale_compact_int_mode mode = compact_int_elem->mode;
  uint8_t mode_upper_bits = compact_int_elem->mode_upper_bits;
  uint8_t *data = compact_int_elem->data;

  uint8_t lsb = mode_upper_bits; //0011111111
  lsb <<= 2; //11111100
  lsb |= (mode & 0x03); //11111100 & 00000011
  if (mode == SCALE_COMPACT_SINGLE_BYTE) {
    *serialized_len = 1;
    serialized[0] = lsb;
    return 0;
  } else if (mode == SCALE_COMPACT_TWO_BYTE) {
    *serialized_len = 2;
    serialized[0] = lsb;
    serialized[1] = data[0];
  } else if (mode == SCALE_COMPACT_FOUR_BYTE) {
    *serialized_len = 4;    
    serialized[0] = lsb;
    serialized[1] = data[0];
    serialized[2] = data[1];
    serialized[3] = data[2];
  } else {  //SCALE_COMPACT_BIGNUM
    uint64_t num_bytes = mode_upper_bits + 4;
    *serialized_len = num_bytes + 1;        
    size_t i;
    serialized[0] = lsb;    
    for(i = 0; i < num_bytes; i++) {
      serialized[i+1] = data[i];
    }
  }
  return 0;
}

//Get the length, in bytes, of the serialized compact_int_elem
size_t compact_int_get_byte_length(scale_compact_int *compact_int_elem) {
  enum scale_compact_int_mode mode = compact_int_elem->mode;
  if (mode == SCALE_COMPACT_SINGLE_BYTE) {
    return 1;
  } else if (mode == SCALE_COMPACT_TWO_BYTE) {
    return 2;
  } else if (mode == SCALE_COMPACT_FOUR_BYTE) {
    return 4;
  } else {  //SCALE_COMPACT_BIGNUM
    uint8_t mode_upper_bits = compact_int_elem->mode_upper_bits;
    uint8_t num_bytes = mode_upper_bits + 4;
    return num_bytes;
  }
}

//These functions encode decimal values into a scale_compact_int struct
int8_t encode_uint8_to_compact_int_scale(scale_compact_int *compact_int_elem, uint8_t data) {
  if(data > 63) {  //Max 2^6 - 1 or 00111111
    return encode_uint16_to_compact_int_scale(compact_int_elem, (uint16_t)data);
    //fprintf(stderr, "Error Encoding Single Byte Compact. Data Too Large.(%u)\n", data);
    //return -1;
  }
  memset(compact_int_elem, 0, sizeof(scale_compact_int));
  compact_int_elem->mode = SCALE_COMPACT_SINGLE_BYTE;
  uint8_t upper = data & 0x3F;
  compact_int_elem->mode_upper_bits = upper;
  compact_int_elem->data = NULL;
  return 0;
}
int8_t encode_uint16_to_compact_int_scale(scale_compact_int *compact_int_elem, uint16_t data) {
  if(data < 63) {
    return encode_uint8_to_compact_int_scale(compact_int_elem, (uint8_t)data);
  }
  if(data > 16383) {  //Max 2^14 - 1 or 00111111 11111111
    return encode_uint32_to_compact_int_scale(compact_int_elem, (uint32_t)data);
    //fprintf(stderr, "Error Encoding Two Byte Compact. Data Too Large.(%u)\n", data);
    //return -1;
  }
  memset(compact_int_elem, 0, sizeof(scale_compact_int));
  compact_int_elem->mode = SCALE_COMPACT_TWO_BYTE;
  uint8_t lsb = data & 0xFF;
  uint8_t upper = lsb & 0x3F;  //Get last 6 bits of lsb
  compact_int_elem->mode_upper_bits = upper;
  compact_int_elem->data = calloc(1, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Two Byte Compact!\n");
    return -1;
  }
  compact_int_elem->data[0] = (data >> 6) & 0xFF; //Get next byte
  return 0;
}
int8_t encode_uint32_to_compact_int_scale(scale_compact_int *compact_int_elem, uint32_t data) {
  if(data < 63) {
    return encode_compact(compact_int_elem, (uint8_t)data);
  } else if (data < 16383) {
    return encode_compact(compact_int_elem, (uint16_t)data);
  }
  if(data > 1073741823) {  //Max 2^30 - 1 or 00111111 11111111 11111111 11111111
    return encode_compact(compact_int_elem, (uint64_t)data);
    //fprintf(stderr, "Error Encoding Four Byte Compact. Data Too Large.(%u)\n", data);
    //return -1;
  }
  memset(compact_int_elem, 0, sizeof(scale_compact_int));
  compact_int_elem->mode = SCALE_COMPACT_FOUR_BYTE;
  uint8_t lsb = data & 0xFF;
  uint8_t upper = lsb & 0x3F;  //Get last 6 bits of lsb


  compact_int_elem->mode_upper_bits = upper;
  compact_int_elem->data = calloc(3, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Four Byte Compact!\n");
    return -1;
  }
  compact_int_elem->data[0] = (data >> 6) & 0xFF;
  compact_int_elem->data[1] = (data >> (6 + 8)) & 0xFF;
  compact_int_elem->data[2] = (data >> (6 + 8 + 8)) & 0xFF;

  return 0;
}
int8_t encode_uint64_to_compact_int_scale(scale_compact_int *compact_int_elem, uint64_t data) {
  if(data < 63) {
    return encode_compact(compact_int_elem, (uint8_t)data);
  } else if (data < 16383) {
    return encode_compact(compact_int_elem, (uint16_t)data);
  } else if(data < 1073741823) {
    return encode_compact(compact_int_elem, (uint32_t)data);
  }
  if(data > 4611686018427387903) {  //Max: 2^62 -1 or 00111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    fprintf(stderr, "Error Encoding Eight Byte Compact. Data Too Large.(%llu)\n", data);
    return -1;
  }
  memset(compact_int_elem, 0, sizeof(scale_compact_int));
  compact_int_elem->mode = SCALE_COMPACT_BIGNUM;
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

  compact_int_elem->data = calloc(byte_len, sizeof(uint8_t));
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
size_t encode_u128_data_to_compact_int_scale(scale_compact_int *compact_int_elem, const uint8_t *data) {
  enum scale_compact_int_mode mode = data[0] & 0x03; //00000011
  uint8_t upper_bits = (data[0] & 0xFC);  //11111100
  if(mode != SCALE_COMPACT_BIGNUM) {
    fprintf(stderr, "Invalid Scale! Unknown Compact Mode\n");
    return 0;
  }
  size_t i = 0;
  uint8_t byte_length = (upper_bits >> 2) + 4;
  if(byte_length <= 8) {
    uint64_t value = 0;
    for(i = byte_length; i > 1; i--) {
      value |= data[i] & 0xFF;
      value <<= 8;
    }
    value |= data[1] & 0xFF;
    if(value <= 4611686018427387903) {  //2^62 - 1 is MAX compact 8 byte scale value
      if(encode_compact(compact_int_elem, value) < 0) {
        fprintf(stderr, "Error Reading Compact Byte!\n");
        return 0;
      }
      return byte_length + 1;
    }
  }

  compact_int_elem->mode = SCALE_COMPACT_BIGNUM;
  compact_int_elem->data = calloc(byte_length, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact!\n");
    return 0;
  }

  int offset = 0;
  for(i=1; i <= byte_length; i++) {
    compact_int_elem->data[offset++] = data[i];
  }
  compact_int_elem->mode_upper_bits = byte_length - 4;
  return byte_length;
}

int8_t encode_u128_string_to_compact_int_scale(scale_compact_int *compact_int_elem, char *hex) {
  char *pHex = hex;
  if(pHex[0] == '0' && (pHex[1] == 'x' || pHex[1] == 'X')) {
    pHex +=2;
  }

  if(!cscale_is_valid_hex(pHex)) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Invalid Hex String.(%s)\n", hex);
    return -1;
  }

  uint8_t *bytes;
  size_t num_bytes = cscale_hex_to_data((const char *) pHex, &bytes);
  if(num_bytes < 1) {
    free(bytes);
    fprintf(stderr, "Error getting Byte Array, len.(%u)\n", (unsigned)num_bytes);
    return -1;
  }


  if(num_bytes > 32) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Data too large.(%s)\n", hex);
    free(bytes);
    return -1;
  }

  char *pEnd = pHex+1;
  int first_num = (int)strtol(pHex, &pEnd, 10);

  if(num_bytes == 32 && first_num > 3) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact. Data too large.(%s)\n", hex);
    free(bytes);
    return -1;
  }


  int64_t i = 0;
  while(bytes[num_bytes-1-i] == 0) {
    num_bytes--;
    i++;
  }


  if(num_bytes <= 8) {
    uint64_t value = 0;
    for(i = 0; i < num_bytes-1; i++) {
      value |= bytes[i] & 0xFF;
      value <<= 8;
    }
    value |= bytes[num_bytes-1] & 0xFF;

    if(value <= 4611686018427387903) {
      free(bytes);
      return encode_uint64_to_compact_int_scale(compact_int_elem, value);
    }
  }

  compact_int_elem->mode = SCALE_COMPACT_BIGNUM;
  compact_int_elem->data = calloc(num_bytes, sizeof(uint8_t));
  if(!compact_int_elem->data) {
    fprintf(stderr, "Error Encoding Sixteen Byte Compact!\n");
    return -1;
  }

  int offset = 0;
  for(i=num_bytes-1; i >=0; i--) {
    compact_int_elem->data[offset++] = bytes[i];
  }
  compact_int_elem->mode_upper_bits = num_bytes - 4;
  free(bytes);
  return 0;
}

int8_t encode_compact_hex_to_scale(scale_compact_int *compact_int_elem, const char *hex) {
  uint8_t *data;
  char *pHex = (char*)hex;
  if(hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
    pHex += 2;
  }
  if(cscale_hex_to_data((const char *) pHex, &data) < 1) {
    fprintf(stderr, "Invalid Hex String\n");
    if(data) {
      free(data);
    }
    return -1;
  }

  enum scale_compact_int_mode mode = data[0] & 0x03; //00000011
  uint8_t upper_bits = (data[0] & 0xFC);  //11111100

  switch (mode) {
    case SCALE_COMPACT_SINGLE_BYTE: {
      uint8_t value = upper_bits >> 2;
      free(data);
      return encode_uint8_to_compact_int_scale(compact_int_elem, value);
    }
    case SCALE_COMPACT_TWO_BYTE: {
      uint16_t value = data[1];
      value <<= 8;
      value |= upper_bits & 0xFF;
      value >>=2;
      free(data);
      return encode_uint16_to_compact_int_scale(compact_int_elem, value);
    }
    case SCALE_COMPACT_FOUR_BYTE: {
      uint32_t value = data[3];
      value <<= 8;
      value |= data[2] & 0xFF;
      value <<= 8;
      value |= data[1] & 0xFF;
      value <<= 8;
      value |= upper_bits & 0xFF;
      value >>= 2;
      free(data);
      return encode_uint32_to_compact_int_scale(compact_int_elem, value);
    }
    case SCALE_COMPACT_BIGNUM: {
      uint8_t byte_length = (upper_bits >> 2) + 4;
      if(byte_length <= 8) {
        uint64_t value = 0;
        int i;
        for(i = byte_length; i > 1; i--) {
          value |= data[i] & 0xFF;
          value <<= 8;
        }
        value |= data[1] & 0xFF;
        if(value <= 4611686018427387903) {  //2^62 - 1 is MAX compact 8 byte scale value
          free(data);
          return encode_uint64_to_compact_int_scale(compact_int_elem, value);
        }
      }

      //get raw hex from this scale value
      uint8_t bytes[byte_length + 1];
      memset(bytes, 0, byte_length+1);
      int offset = 0;

      char stack_raw_hex[byte_length + 1];
      memset(stack_raw_hex, 0, byte_length + 1);

      size_t i = 0;
      for(i = byte_length; i>= 1; i--) {
        char temp[4] = {0};
        snprintf(temp, 3, "%02X", data[i]);
        strcat(stack_raw_hex, temp);
        bytes[offset++] = data[i];
      }
      free(data);
      return encode_u128_string_to_compact_int_scale(compact_int_elem, stack_raw_hex);
    }
    default: {
      fprintf(stderr, "Invalid Scale! Unknown Compact Mode\n");
      free(data);
      return -1;
    }
  }
}


//Decode functions

//remember to free
char* decode_compact_to_hex(scale_compact_int *compact_int_elem) {
  if(compact_int_elem->mode == SCALE_COMPACT_SINGLE_BYTE) {
    char *hex = calloc(1 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    snprintf(hex, 4, "%02X", compact_int_elem->mode_upper_bits);
    return hex;
  }

  else if(compact_int_elem->mode == SCALE_COMPACT_TWO_BYTE) {
    char *hex = calloc(8 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }
    uint16_t value = compact_int_elem->data[0] << 6;
    value |= compact_int_elem->mode_upper_bits & 0xFF;
    snprintf(hex, 8, "%02X",  value);
    return hex;
  }


  else if(compact_int_elem->mode == SCALE_COMPACT_FOUR_BYTE) {
    char *hex = calloc(32 + 1, sizeof(char));
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

  else if(compact_int_elem->mode == SCALE_COMPACT_BIGNUM) {
    size_t byte_len = compact_int_elem->mode_upper_bits + 4;
    char *hex = calloc(byte_len*2 + 1, sizeof(char));
    if(!hex) {
      fprintf(stderr, "Failed to Decode value to hex. Out of Memory\n");
      return NULL;
    }

    int32_t i = 0;
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
}


uint64_t decode_compact_to_u64(scale_compact_int *compact_int_elem) {
  char *hex = decode_compact_to_hex(compact_int_elem);
  if(!hex) {
    return 0;
  }
  uint64_t ret_val = strtoull(hex, NULL, 16);
  free(hex);
  return ret_val;
}

//Reads the serialized Compact/General Int byte array into a scale_compact_int Structure
//Returns the total number of bytes read
//Returns 0 if fails to read
size_t read_compact_int_from_data(scale_compact_int *compact_int_elem, uint8_t *serialized) {
  enum scale_compact_int_mode mode = serialized[0] & 0x03; //00000011
  uint8_t upper_bits = (serialized[0] & 0xFC);  //11111100

  switch (mode) {
    case SCALE_COMPACT_SINGLE_BYTE: {
      uint8_t value = upper_bits >> 2;
      if(encode_compact(compact_int_elem, value) < 0) {
        fprintf(stderr, "Error Reading Compact Byte!\n");
        return 0;
      }
      return 1;
    }
    case SCALE_COMPACT_TWO_BYTE: {
      uint16_t value = serialized[1];
      value <<= 8;
      value |= upper_bits & 0xFF;
      value >>=2;
      if(encode_compact(compact_int_elem, value) < 0) {
        fprintf(stderr, "Error Reading Compact Byte!\n");
        return 0;
      }
      return 2;
    }
    case SCALE_COMPACT_FOUR_BYTE: {
      uint32_t value = serialized[3];
      value <<= 8;
      value |= serialized[2] & 0xFF;
      value <<= 8;
      value |= serialized[1] & 0xFF;
      value <<= 8;
      value |= upper_bits & 0xFF;
      value >>= 2;
      if(encode_compact(compact_int_elem, value) < 0) {
        fprintf(stderr, "Error Reading Compact Byte!\n");
        return 0;
      }
      return 4;
    }
    case SCALE_COMPACT_BIGNUM: {
      uint8_t byte_length = (upper_bits >> 2) + 4;
      if(byte_length <= 8) {
        uint64_t value = 0;
        int i;
        for(i = byte_length; i > 1; i--) {
          value |= serialized[i] & 0xFF;
          value <<= 8;
        }
        value |= serialized[1] & 0xFF;
        if(value <= 4611686018427387903) {  //2^62 - 1 is MAX compact 8 byte scale value
          if(encode_compact(compact_int_elem, value) < 0) {
            fprintf(stderr, "Error Reading Compact Byte!\n");
            return 0;
          }
          return byte_length + 1;
        }
      }
      return encode_u128_data_to_compact_int_scale(compact_int_elem, serialized);
    }
    default: {
      fprintf(stderr, "Invalid Scale! Unknown Compact Mode\n");
      return 0;
    }
  }
}

void cleanup_scale_compact_int(scale_compact_int *compact) {
  if(compact->data) {
    free(compact->data);
  }
}
