## Compact/General Ints in cScale

[Compact/General Ints](https://docs.substrate.io/v3/advanced/scale-codec/#compactgeneral-integers) represent variable byte width integers. These are only unsigned. These functions support 1 byte through 16 integers.

If a value is greater than the type to which it is attempting to encode (encoding a `uint16_t` as a `uint8_t`), or vice versa, cScale will attempt to encode it in the smallest possible data type.

---

#### Initialization And Freeing:

Make sure to assign `scale_compact_int` values to the macro `SCALE_COMPACT_INT_INIT`. This guarantees that pointers are set to `NULL` and will be safe to free if left unused.

Call `cleanup_scale_compact_int` to free any heap-allocated memory.
```c
scale_compact_int compact = SCALE_COMPACT_INT_INIT;
...
cleanup_scale_compact_int(&compact);
```


---

### Example:

##### Encoding:
```c
scale_compact_int compact = SCALE_COMPACT_INT_INIT;
encode_uint16_to_compact_int_scale(&compact, (uint16_t)254);
```

#### Serializing Into Byte Array:
```c
uint8_t serialized[64] = { 0 };
uint64_t len = 0;
serialize_compact_int(serialized, &len, &compact);
```

#### Getting the Byte Length of this Compact:
```c
uint64_t byte_length = compact_int_get_byte_length(&compact);
// Remember, if compact.mode == SCALE_COMPACT_BIGNUM, bytes_read will be byte width + 1
// because bignum mode stores the length of the value in a prefix byte. (bytes_read counts the prefix byte)
```

#### Deserializing From Byte Array:
```c
scale_compact_int read_from_bytes = SCALE_COMPACT_INT_INIT;
size_t bytes_read = read_compact_int_from_data(&read_from_bytes, serialized);
cleanup_scale_compact_int(&read_from_bytes);
```

#### Decoding Into Integer (Max of `uint64_t`):
```c
char *compact_hex = decode_compact_to_hex(&compact);
uint16_t value = strtoull(compact_hex, NULL, 16);
printf("%s = %u\n", compact_hex, value);
free(compact_hex);
```

#### Serializing Into Hex String:
```c
char *compact_hex = cscale_byte_array_to_hex(serialized, serialized_len);
free(compact_hex);
```

#### Deserializing From Hex String:
```c
encode_compact_hex_to_scale(&compact, compact_hex);
cleanup_scale_compact_int(&compact);
```

---

For 16 byte (u128) integers, values will have to be interpreted as strings since standard C is lacking uint128_t. (It is available as GCC extension but literal int values are sometimes too long.)


### Example:

#### Encoding 128 Bit Hex String To SCALE:
```c
scale_compact_int compact = SCALE_COMPACT_INT_INIT;
encode_u128_string_to_compact_int_scale(&compact, "0x1C8E02B7B2367C5E1C8A7787CB3816C5");
cleanup_scale_compact_int(&compact);
```