# cScale
A SCALE encoding library


## What is This?
cScale is a library written in C that adheres to the SCALE (Simple Concatenated Aggregate Little-Endian) codec
[documented here](https://substrate.dev/docs/en/knowledgebase/advanced/codec). It allows for the serialization and deserialization of data into valid SCALE.

It has only one external dependency which is a pure header and included. C99 is generally used except for several C11 `_Generic` helper definitions.


---

## Compilation 
```shell
make all
```
Default compiler is `clang`. You can override with `CC=gcc make all`

This will build all targets, currently:

Target | Generated | Purpose
--- | --- | --- |
cli | ./cli | Basic command line app
test | ./test | Testing app
libcScale.a | ./libcScale.a | static library


Link to the generated `libcScale.a` and copy `src/scale.h` to your include path. Then include it
```c
#include <scale.h>
```

## Usage

cScale currently supports the following SCALE data types:

Data Type | cScale Structure
---: | :---:
Fixed Width Integer | `scale_fixed_int`
Compact/General Integer | `scale_compact_int`
Boolean | `scale_boolean`
Option | `scale_option` (enum)
Vector | `scale_vector`
String | `scale_vector`
Data Structures | User-defined `struct` containing a `scale_structure`
Enumeration | `scale_enumeration`
Tuple | None, helper functions only


## Examples

---
#### Serializing and Deserializing a Fixed Width Integer:
```c
scale_fixed_int fixed = { 0 };
encode_int_to_fixed_int_scale(&fixed, (uint16_t)42);
uint8_t serialized[64] = { 0 };
size_t serialized_len = 0;
serialize_fixed_int(serialized, &serialized_len, &fixed);

uint16_t output = 0;
decode_scale_fixed_int((void*)&output, &fixed);

for(int i=0; i < serialized_len; i++) printf("%02X", serialized[i]);
printf(" --- %u\n", output);
```
#### Output:
```shell
2A00 --- 42
```

---
#### Serializing and Deserializing a Compact Integer:

```c
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  encode_compact(&compact, (uint32_t)69);
  uint8_t serialized[64] = { 0 };
  size_t serialized_len = 0;
  char *output = decode_compact_to_hex(&compact);
  serialize_compact_int(serialized, &serialized_len, &compact);
  uint32_t decoded = strtoul(output, NULL, 16);
  printf("SCALE=<");
  for(int i=0; i < serialized_len; i++) printf("%02X", serialized[i]);
  printf("> --- Hex=<%s> --- Decoded=<%u>\n", output, decoded);
  free(output);
```
#### Output:
```shell
SCALE=<1501> --- Hex=<45> --- Decoded=<69>
```


For more information, you can have a look over the test files in `src/test/` to see examples of each kind. 


## Current Status/Limitations

This is a working build that has not been subject to any extensive audits. 

#### Current Limitations:
|Type | Reason
:---: | :---
|u128 | C does not support native uint128_t. GCC extension exists but literal u128 values too long. Hex Strings currently used.|
|Enum | Currently only supporting fixed int/compact int/boolean types|
|Vector | Due to lack of u128, vector length is currently limited to uint64 max values as max elements. <br /> <br /> Helper Functions rely on same byte-width elements, such as fixed int or characters. Parsing can be done manually for variable width elements, such as Options. |
