/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

int8_t serialize_fixed_int(uint8_t *serialized, uint64_t *serialized_len, _scale_fixed_int *fixed_int_elem) {
  *serialized_len = 0;

  uint8_t byte_width = fixed_int_elem->byte_width;
  uint8_t is_signed = fixed_int_elem->is_signed;

  if(byte_width != 1 && byte_width != 2 && byte_width > 4) {
    fprintf(stderr, "Error Serializing Fixed Int. Invalid Byte Width. (%u)\n", byte_width);
    return -1;
  }

  if(byte_width == 1) { //8 bit fixed int
    *serialized_len = 1;
    if(is_signed) {
      serialized[0] = (int8_t)fixed_int_elem->data[3];
    } else {
      serialized[0] = fixed_int_elem->data[3];
    }
  } else if(byte_width == 2) { //16 bit fixed int
    *serialized_len = 2;
    if(is_signed) {
      serialized[0] = (int8_t)fixed_int_elem->data[2];
      serialized[1] = (int8_t)fixed_int_elem->data[3];
    } else {
      serialized[0] = fixed_int_elem->data[2];
      serialized[1] = fixed_int_elem->data[3];
    }
  } else { //32 bit fixed int
    *serialized_len = 4;
    if(is_signed) {
      serialized[0] = (int8_t)fixed_int_elem->data[0];
      serialized[1] = (int8_t)fixed_int_elem->data[1];
      serialized[2] = (int8_t)fixed_int_elem->data[2];
      serialized[3] = (int8_t)fixed_int_elem->data[3];
    } else {
      serialized[0] = fixed_int_elem->data[0];
      serialized[1] = fixed_int_elem->data[1];
      serialized[2] = fixed_int_elem->data[2];
      serialized[3] = fixed_int_elem->data[3];
    }
  }
  return 0;
}

void _encode_fixed_int8_to_scale(_scale_fixed_int *fixed_int_elem, int8_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed = true;
  fixed_int_elem->data[3] = data;
}

void _encode_fixed_uint8_to_scale(_scale_fixed_int *fixed_int_elem, uint8_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  fixed_int_elem->byte_width = 1;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[3] = data;
}

void _encode_fixed_int16_to_scale(_scale_fixed_int *fixed_int_elem, int16_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  if(_is_big_endian(&data)) {
    data = swap_int16(data);
  }
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[2] = data & 0xFF;
  data >>= 8;
  fixed_int_elem->data[3] = data & 0xFF;
}

void _encode_fixed_uint16_to_scale(_scale_fixed_int *fixed_int_elem, uint16_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  if(_is_big_endian(&data)) {
    data = swap_uint16(data);
  }
  fixed_int_elem->byte_width = 2;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[2] = (data & 0xFF);
  fixed_int_elem->data[3] = ((data >> 8) & 0xFF);
}

void _encode_fixed_int32_to_scale(_scale_fixed_int *fixed_int_elem, int32_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  if(_is_big_endian(&data)) {
    data = swap_int32(data);
  }
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = true;
  fixed_int_elem->data[0] = (data & 0xFF);
  fixed_int_elem->data[1] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[3] = ((data >> 24) & 0xFF);
}
void _encode_fixed_uint32_to_scale(_scale_fixed_int *fixed_int_elem, uint32_t data) {
  memset(fixed_int_elem, 0, sizeof(_scale_fixed_int));
  if(_is_big_endian(&data)) {
    data = swap_uint32(data);
  }
  fixed_int_elem->byte_width = 4;
  fixed_int_elem->is_signed  = false;
  fixed_int_elem->data[0] = (data & 0xFF);
  fixed_int_elem->data[1] = ((data >> 8) & 0xFF);
  fixed_int_elem->data[2] = ((data >> 16) & 0xFF);
  fixed_int_elem->data[3] = ((data >> 24) & 0xFF);
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
        int8_t value = (int8_t)data[0];
        _encode_fixed_int8_to_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        int16_t value = data[1];
        value <<= 8;
        value |= data[0] & 0xFF;
        _encode_fixed_int16_to_scale(fixed_int_elem, value);
        break;
      }
      case 4: {
        int32_t value = data[3];
        value <<= 8;
        value |= data[2] & 0xFF;
        value <<= 8;
        value |= data[1] & 0xFF;
        value <<= 8;
        value |= data[0] & 0xFF;
        _encode_fixed_int32_to_scale(fixed_int_elem, value);
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
        uint8_t value = data[0];
        _encode_fixed_uint8_to_scale(fixed_int_elem, value);
        break;
      }
      case 2: {
        uint16_t value = data[1];
        value <<= 8;
        value |= data[0] & 0xFF;
        _encode_fixed_uint16_to_scale(fixed_int_elem, value);
        break;
      }
      case 4: {
        uint32_t value = data[3];
        value <<= 8;
        value |= data[2] & 0xFF;
        value <<= 8;
        value |= data[1] & 0xFF;
        value <<= 8;
        value |= data[0] & 0xFF;
        _encode_fixed_uint32_to_scale(fixed_int_elem, value);
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


char *get_encoded_hex_from_scale_fixed_int(_scale_fixed_int *fixed_int_elem) {
  if(fixed_int_elem->byte_width < 1 || fixed_int_elem->byte_width > 4) {  //currently 1 byte - 4 byte max
    return NULL;
  }
  return byte_array_to_hex(fixed_int_elem->data, fixed_int_elem->byte_width);
}


//decode fixed int scale elem and place the output in void*out, user can cast due to known output type
int8_t decode_scale_fixed_int(void *output, _scale_fixed_int *fixed_int_elem) {
  if (
    fixed_int_elem->byte_width < 1 ||
    fixed_int_elem->byte_width > FIXED_INT_MAX_BYTES ||
    (fixed_int_elem->byte_width > 1 && fixed_int_elem->byte_width % 2 != 0)
  ) {
    fprintf(stderr, "Invalid Byte Width.(%d)\n", fixed_int_elem->byte_width);
    return -1;
  }

  if (fixed_int_elem->is_signed) {
    switch (fixed_int_elem->byte_width) {
      case 1:
        (*(int8_t*)output) = (int8_t)fixed_int_elem->data[3] & 0xFF;
        break;
      case 2:
        (*(int16_t*)output) = (int8_t)fixed_int_elem->data[3] & 0xFF;
        (*(int16_t*)output) <<= 8;
        (*(int16_t*)output) |= (int8_t)fixed_int_elem->data[2] & 0xFF;
        break;
      case 4:
        (*(int32_t*)output) = (int8_t)fixed_int_elem->data[3] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[2] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[1] & 0xFF;
        (*(int32_t*)output) <<= 8;
        (*(int32_t*)output) |= (int8_t)fixed_int_elem->data[0] & 0xFF;
        break;
      default:
        fprintf(stderr, "Invalid Byte Width.(%d). Cannot Decode\n", fixed_int_elem->byte_width);
        return -1;
        break;
    }
  } else {
    switch (fixed_int_elem->byte_width) {
      case 1:
        (*(uint8_t*)output) = (uint8_t)fixed_int_elem->data[3] & 0xFF;
        break;
      case 2:
        (*(uint16_t*)output) = (uint8_t)fixed_int_elem->data[3] & 0xFF;
        (*(uint16_t*)output) <<= 8;

        (*(uint16_t*)output) |= (uint8_t)fixed_int_elem->data[2] & 0xFF;
        break;
      case 4:
        (*(uint32_t*)output) = (uint8_t)fixed_int_elem->data[3] & 0xFF;
        (*(uint32_t*)output) <<= 8;

        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[2] & 0xFF;
        (*(uint32_t*)output) <<= 8;

        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[1] & 0xFF;
        (*(uint32_t*)output) <<= 8;

        (*(uint32_t*)output) |= (uint8_t)fixed_int_elem->data[0] & 0xFF;

        break;
      default:
        fprintf(stderr, "Invalid Byte Width.(%d). Cannot Decode\n", fixed_int_elem->byte_width);
        return -1;
        break;
    }
  }
  return 0;
}
