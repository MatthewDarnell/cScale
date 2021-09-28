/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Encode functions

int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, _scale_fixed_int *fixed_int_elem) {
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

//These functions encode decimal values into a _scale_fixed_int struct
void _encode_fixed_int8_to_scale(_scale_fixed_int *fixed_int_elem, int8_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data;
}
void _encode_fixed_uint8_to_scale(_scale_fixed_int *fixed_int_elem, uint8_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data;
}

void _encode_fixed_int16_to_scale(_scale_fixed_int *fixed_int_elem, int16_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = data & 0xFF;
  data >>= 8;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = data & 0xFF;
}
void _encode_fixed_uint16_to_scale(_scale_fixed_int *fixed_int_elem, uint16_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 8) & 0xFF);
}

void _encode_fixed_int32_to_scale(_scale_fixed_int *fixed_int_elem, int32_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 24) & 0xFF);
}
void _encode_fixed_uint32_to_scale(_scale_fixed_int *fixed_int_elem, uint32_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-4] = (data & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-3] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[FIXED_INT_MAX_BYTES-1] = ((data >> 24) & 0xFF);
}

void _encode_fixed_int64_to_scale(_scale_fixed_int *fixed_int_elem, int64_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
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

void _encode_fixed_uint64_to_scale(_scale_fixed_int *fixed_int_elem, uint64_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
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

int8_t _encode_fixed_u128_to_scale(_scale_fixed_int *fixed_int_elem, char *hex) {
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
  size_t length = hex_to_data((const char*)pHex, &data);
  if(length < 1) {
    fprintf(stderr, "Error Encoding u128. Failed to Read Hex\n");
    return -1;
  }

  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 16;
  fixed_int_elem->is_signed  = false;
  int i;
  for(i=0; i < 16; i++) {
    if(i < length) {
      fixed_int_elem->data[i] = (data[i] & 0xFF);
    } else {
      fixed_int_elem->data[i] = 0x00;
    }
  }
  free(data);
  return 0;
}

int8_t _encode_fixed_hex_to_scale(_scale_fixed_int *fixed_int_elem, bool is_signed, const char *hex) {
  uint8_t *data;
  int32_t byte_width = 0;
  char *pHex = (char*)hex;
  if(hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
    pHex += 2;
  }
  if((byte_width = hex_to_data((const char*)pHex, &data)) < 1) {
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

  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  if(is_signed) {
    switch (byte_width) {
      case 1: {
        int8_t value = (int8_t)data[byte_width-1];
        _encode_fixed_int8_to_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        int16_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        _encode_fixed_int16_to_scale(fixed_int_elem, value);
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
        _encode_fixed_int32_to_scale(fixed_int_elem, value);
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
        _encode_fixed_int64_to_scale(fixed_int_elem, value);
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
        _encode_fixed_uint8_to_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        uint16_t value = data[byte_width-1];
        value <<= 8;
        value |= data[byte_width-2] & 0xFF;
        _encode_fixed_uint16_to_scale(fixed_int_elem, value);
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
        _encode_fixed_uint32_to_scale(fixed_int_elem, value);
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
        _encode_fixed_uint64_to_scale(fixed_int_elem, value);
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
void deserialize_fixed_int(void *output, uint8_t *bytes, size_t len, bool is_signed) {
  char *hex = _byte_array_to_hex(bytes, len);
  char stack_hex[256] = { 0 };
  strcpy(stack_hex, hex);
  free(hex);
  _scale_fixed_int fixed;
  _encode_fixed_hex_to_scale(&fixed, is_signed, stack_hex);
  decode_scale_fixed_int(output, &fixed);
}

//remember to free
char *decode_scale_fixed_to_hex(_scale_fixed_int *fixed_int_elem) {
  if(fixed_int_elem->byte_width < 1 || fixed_int_elem->byte_width > FIXED_INT_MAX_BYTES) {  //currently 1 byte - 4 byte max
    return NULL;
  }
  return _fixed_byte_array_to_hex(fixed_int_elem->data, fixed_int_elem->byte_width);
}

//decode fixed int scale elem and place the output in void*out, user can cast due to known output type
int8_t decode_scale_fixed_int(void *output, _scale_fixed_int *fixed_int_elem) {
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
