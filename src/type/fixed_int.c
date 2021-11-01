/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Example: cef512a72670a59a0000000000000000 to f7d8fca02ff8ab99
int8_t swap_u128_le_to_be(char *be_out, char *le) {
  char *pLe = le;
  if(pLe[0] == '0' && (
      pLe[1] == 'X' || pLe[1] == 'x'
    )) {
    pLe += 2;
  }
  size_t i = strlen(pLe) - 1;
  memset(be_out, 0, strlen(pLe));
  while(pLe[i] == '0' && pLe[i-1] == '0') {
    i -= 2;
    if(i < 2) { //just 0 value
      strcpy(be_out, pLe);
      return -1;
    }
  }

  if((i+1) % 2 != 0) {
    fprintf(stderr, "Odd number U128: %s\n", le);
    return -1;
  }
  size_t offset = 0;

  char temp[4] = { 0 };

  for(offset = i; offset > 1; offset-=2) {
    memset(temp, 0, 4);
    snprintf(temp, 4, "%c%c", pLe[offset-1], pLe[offset]);
    strcat(be_out, temp);
  }
  snprintf(temp, 4, "%c%c", pLe[0], pLe[1]);
  strcat(be_out, temp);

  return 0;
}


//Encode functions

int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, scale_fixed_int *fixed_int_elem) {
  *serialized_len = 0;

  uint8_t byte_width = fixed_int_elem->byte_width;
  if(byte_width != 1 && byte_width != 2 && byte_width > FIXED_INT_MAX_BYTES) {
    fprintf(stderr, "Error Serializing Fixed Int. Invalid Byte Width. (%u)\n", byte_width);
    return -1;
  }

  if(byte_width == 1) { //8 bit fixed int
    *serialized_len = 1;
    memcpy(serialized, fixed_int_elem->data + FIXED_INT_MAX_BYTES - 1, 1);
  } else if(byte_width == 2) { //16 bit fixed int
    *serialized_len = 2;
    memcpy(serialized, fixed_int_elem->data + FIXED_INT_MAX_BYTES - 2, 2);
  } else if(byte_width == 4) { //32 bit fixed int
    *serialized_len = 4;
    memcpy(serialized, fixed_int_elem->data + FIXED_INT_MAX_BYTES - 4, 4);
  } else if(byte_width == 8) {
    *serialized_len = 8;
    memcpy(serialized, fixed_int_elem->data + FIXED_INT_MAX_BYTES - 8, 8);
  } else {
    *serialized_len = FIXED_INT_MAX_BYTES;
    memcpy(serialized, fixed_int_elem->data, FIXED_INT_MAX_BYTES);
  }
  return 0;
}

//These functions encode decimal values into a scale_fixed_int struct
void encode_int8_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int8_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data;
}
void encode_uint8_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint8_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data;
}

void encode_int16_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int16_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = data & 0xFF;
  data >>= 8;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data & 0xFF;
}
void encode_uint16_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint16_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 8) & 0xFF);
}

void encode_int32_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int32_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 24) & 0xFF);
}
void encode_uint32_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint32_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 24) & 0xFF);
}

void encode_int64_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, int64_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 8;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-8] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-7] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-6] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-5] = ((data >> 24) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = ((data >> 32) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 40) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 48) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 56) & 0xFF);
}

void encode_uint64_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, uint64_t data) {
  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 8;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-8] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-7] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-6] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-5] = ((data >> 24) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = ((data >> 32) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 40) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 48) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 56) & 0xFF);
}

int8_t encode_u128_string_to_fixed_int_scale(scale_fixed_int *fixed_int_elem, char *hex) {
  char *pHex = hex;
  if(strlen(pHex) < 2) {
    return -1;
  }
  if(pHex[0] == '0' && (
    pHex[1] == 'X' || pHex[1] == 'x'
  )) {
    pHex += 2;
  }
  uint8_t *data;
  size_t length = cscale_hex_to_data((const char *) pHex, &data);
  if(length < 1) {
    fprintf(stderr, "Error Encoding u128. Failed to Read Hex\n");
    return -1;
  }

  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  fixed_int_elem->byte_width = 16;
  fixed_int_elem->is_signed  = false;
  int i;

  for(i=15; i >= 0; i--) {
    if(i < length) {
      fixed_int_elem->data[i] = (data[i] & 0xFF);
    } else {
      fixed_int_elem->data[i] = 0x00;
    }
  }
  free(data);
  return 0;
}

int8_t encode_fixed_hex_to_scale(scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex) {
  uint8_t *data;
  int32_t byte_width = 0;
  char *pHex = (char*)hex;
  if(hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
    pHex += 2;
  }
  if((byte_width = cscale_hex_to_data((const char *) pHex, &data)) < 1) {
    fprintf(stderr, "Invalid Hex String\n");
    if(data) {
      free(data);
    }
    return -1;
  }

  if(byte_width < 1 || (byte_width > 1 && byte_width % 2 != 0)) {
    fprintf(stderr, "Invalid Hex String or too Small!\n");
    free(data);
    return -1;
  }

  if(byte_width > FIXED_INT_MAX_BYTES) {
    fprintf(stderr, "Hex Value too Large!\n");
    free(data);
    return -1;
  }

  memset(fixed_int_elem, 0, sizeof(scale_fixed_int));
  if(is_signed) {
    switch (byte_width) {
      case 1: {
        int8_t value = (int8_t)data[byte_width-1];
        encode_int8_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        int16_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        encode_int16_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 4: {
        int32_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        value <<= 8;
        value |= data[byte_width-3] & 0xFF;
        value <<= 8;
        value |= data[byte_width-4] & 0xFF;
        encode_int32_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 8: {
        int64_t value = data[7];
        value <<= 8;
        value |= data[6] & 0xFF;
        value <<= 8;
        value |= data[5] & 0xFF;
        value <<= 8;
        value |= data[4] & 0xFF;
        value <<= 8;
        value |= data[3] & 0xFF;
        value <<= 8;
        value |= data[2] & 0xFF;
        value <<= 8;
        value |= data[1] & 0xFF;
        value <<= 8;
        value |= data[0] & 0xFF;
        encode_int64_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      default: {
        free(data);
        return -1;
      }
    }
  }
  else {
    switch (byte_width) {
      case 1: {
        uint8_t value = data[byte_width-1];
        encode_uint8_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        uint16_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        encode_uint16_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 4: {
        uint32_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        value <<= 8;
        value |= data[byte_width-3] & 0xFF;
        value <<= 8;
        value |= data[byte_width-4] & 0xFF;
        encode_uint32_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      case 8: {
        uint64_t value = data[7];
        value <<= 8;
        value |= data[6] & 0xFF;
        value <<= 8;
        value |= data[5] & 0xFF;
        value <<= 8;
        value |= data[4] & 0xFF;
        value <<= 8;
        value |= data[3] & 0xFF;
        value <<= 8;
        value |= data[2] & 0xFF;
        value <<= 8;
        value |= data[1] & 0xFF;
        value <<= 8;
        value |= data[0] & 0xFF;
        encode_uint64_to_fixed_int_scale(fixed_int_elem, value);
        break;
      }
      default: {
        free(data);
        return -1;
      }
    }
  }
  free(data);
  return 0;
}


//Decode functions


//Reads the serialized Fixed Int byte array into a scale_fixed_int Structure
//fixed_int_width is the number of bytes of the fixed int. (i8,u8=1, i64,u64=8, etc)
//is_signed is whether or not the value is signed
//Returns the total number of bytes read
//Returns 0 if fails to read
size_t read_fixed_int_from_data(scale_fixed_int *fixed_int_elem, size_t fixed_int_width, bool is_signed, const uint8_t *restrict serialized) {
  if(fixed_int_width != 1 && fixed_int_width % 2 != 0) {
    fprintf(stderr, "Invalid Byte Width for Reading Fixed Int! Invalid Length.(%u)\n", (unsigned)fixed_int_width);
    return 0;
  }
  char *hex = cscale_byte_array_to_hex((uint8_t*)serialized, fixed_int_width);
  if(!hex) {
    fprintf(stderr, "Memory Error in Reading Compact Int From Data!\n");
    return 0;
  }
  char stack_hex[256] = { 0 };
  strcpy(stack_hex, hex);
  free(hex);
  encode_fixed_hex_to_scale(fixed_int_elem, is_signed, stack_hex);
  return fixed_int_width;
}

int8_t deserialize_fixed_int(void *output, uint8_t *bytes, size_t len, bool is_signed) {
  scale_fixed_int fixed;
  read_fixed_int_from_data(&fixed, len, is_signed, bytes);
  return decode_scale_fixed_int(output, &fixed);
}


//remember to free
char *decode_scale_fixed_to_hex(scale_fixed_int *fixed_int_elem) {
  if(fixed_int_elem->byte_width < 1 || fixed_int_elem->byte_width > FIXED_INT_MAX_BYTES) {  //currently 1 byte - 4 byte max
    return NULL;
  }
  return cscale_fixed_byte_array_to_hex(fixed_int_elem->data, fixed_int_elem->byte_width);
}

//decode fixed int scale elem and place the output in void*out, user can cast due to known output type
int8_t decode_scale_fixed_int(void *output, scale_fixed_int *fixed_int_elem) {
  if (
    fixed_int_elem->byte_width < 1 ||
    fixed_int_elem->byte_width > FIXED_INT_MAX_BYTES ||
    (fixed_int_elem->byte_width > 1 && fixed_int_elem->byte_width % 2 != 0)
  ) {
    fprintf(stderr, "Invalid Byte Width.(%d) (Max=%u)\n", fixed_int_elem->byte_width, FIXED_INT_MAX_BYTES);
    return -1;
  }

  if (fixed_int_elem->is_signed) {
    switch (fixed_int_elem->byte_width) {
      case 1: {
        (*(int8_t*)output) = (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        break;
      }
      case 2: {
        (*(int16_t*)output) = (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(int16_t*)output) <<= 8;
        (*(int16_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        break;
      }
      case 4: {
        (*(int32_t*)output) = (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] & 0xFF;
        break;
      }
      case 8: {
        (*(int64_t*)output) = (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-5] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-6] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-7] & 0xFF;
        (*(int64_t*)output) <<= 8;
        (*(int64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-8] & 0xFF;
        break;
      }
      default:
        fprintf(stderr, "Invalid Byte Width.(%d). Cannot Decode\n", fixed_int_elem->byte_width);
        return -1;
        break;
    }
  } else {
    switch (fixed_int_elem->byte_width) {
      case 1: {
        (*(uint8_t*)output) = (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        break;
      }
      case 2: {
        (*(uint16_t*)output) = (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(uint16_t*)output) <<= 8;
        (*(uint16_t*)output) |= (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        break;
      }
      case 4: {
        (*(uint32_t*)output) = (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(uint32_t*)output) <<= 8;
        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        (*(uint32_t*)output) <<= 8;
        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] & 0xFF;
        (*(uint32_t*)output) <<= 8;
        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] & 0xFF;
        break;
      }
      case 8: {
        (*(uint64_t*)output) = (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-5] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-6] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-7] & 0xFF;
        (*(uint64_t*)output) <<= 8;
        (*(uint64_t*)output) |= (int8_t)fixed_int_elem->data[FIXED_INT_MAX_BYTES-8] & 0xFF;
        break;
      }
      default:
        fprintf(stderr, "Invalid Byte Width.(%d). Cannot Decode\n", fixed_int_elem->byte_width);
        return -1;
        break;
    }
  }
  return 0;
}
