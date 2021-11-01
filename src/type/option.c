/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "../util/hex.h"
#include "../scale.h"

//Serializes a SCALE option value and places the data into serialized, of length serialized_len
//User is responsible for ensuring that serialized can hold at least data_len+1 bytes
//scale_option enum defines how to encode this Option
//data and data_len represent the SCALE-serialized data of some other object. data can be NULL if option value is None
void serialize_scale_option(uint8_t *serialized, size_t *serialized_len, enum scale_option option, uint8_t* data, size_t data_len) {
  if(option == None) {
    memset(serialized, 0, 1);
    *serialized = 0x00;
    *serialized_len = 1;
  } else if(option == Some) {
    memset(serialized, 0, data_len + 1);
    *serialized = 0x01;
    *serialized_len = data_len + 1;
    memmove(&serialized[1], data, data_len);
  }
  else if(option == BoolFalse) {
    memset(serialized, 0, 1);
    *serialized = 0x02;
    *serialized_len = 1;
  }
}

//Value of this Option placed into pointer option
//data+1 now points to the raw value, if option is Some value
int8_t deserialize_scale_option(enum scale_option *option, const uint8_t *serialized) {
  switch(serialized[0]) {
    case 0x00: {
      *option = None;
      return 0;
    }
    case 0x01: {
      *option = Some;
      return 0;
    }
    case 0x02: {
      *option = BoolFalse;
      return 0;
    }
    default: {
      fprintf(stderr, "Decoding Invalid Option!\n");
      return -1;
    }
  }
}
