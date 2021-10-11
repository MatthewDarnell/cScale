/*
    2021 cScale - A SCALE Library written in C
    Created by Matthew Darnell
*/
#include <stdlib.h>
#include <string.h>
#include "util/hex.h"
#include "scale.h"

  //https://stackoverflow.com/questions/2182002/convert-big-endian-to-little-endian-in-c-without-using-provided-func
  //! Byte swap unsigned short
  uint16_t swap_uint16( uint16_t val )
  {
      return (val << 8) | (val >> 8 );
  }

  //! Byte swap short
  int16_t swap_int16( int16_t val )
  {
      return (val << 8) | ((val >> 8) & 0xFF);
  }

  //! Byte swap unsigned int
  uint32_t swap_uint32( uint32_t val )
  {
      val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
      return (val << 16) | (val >> 16);
  }

  //! Byte swap int
  int32_t swap_int32( int32_t val )
  {
      val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
      return (val << 16) | ((val >> 16) & 0xFFFF);
  }

  //https://www.geeksforgeeks.org/little-and-big-endian-mystery/
  bool is_big_endian(void *value) {
    char *c = (char*)&value;
    if (*c) {
      return false;
    }
    return true;
  }

