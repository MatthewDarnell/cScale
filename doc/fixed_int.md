## Fixed Ints in cScale

[Fixed Ints](https://docs.substrate.io/v3/advanced/scale-codec/#fixed-width-integers) represent fixed byte width integers. These can be signed or unsigned. These functions support 1 byte through 8 byte integers.

---

### Example:

##### Encoding:
```c
scale_fixed_int s_e;
encode_int_to_fixed_int_scale(&s_e, (int32_t)1030404040);
```

#### Serializing Into Hex String:
```c
char *fixed_hex = decode_scale_fixed_to_hex(&s_e);
printf("%s\n", fixed_hex); //C8B76A3D
free(fixed_hex);
```

#### Deserializing From Hex String:
```c
encode_fixed_hex_to_scale(&s_e, true, fixed_hex);
```

#### Serializing Into Byte Array:
```c
uint8_t serialized[64] = { 0 };
uint64_t len = 0;
serialize_fixed_int(serialized, &len, &s_e);
```

#### Deserializing From Byte Array:
```c
read_fixed_int_from_data(&s_e, 4, true, (const uint8_t*)serialized);
```

#### Decoding Into Integer (Max of `uint64_t`):
```c
int32_t output = 0;
decode_scale_fixed_int((void*)&output, &s_e);
```

---

For 16 byte (u128) integers, values will have to be interpreted as strings since standard C is lacking uint128_t. (It is available as GCC extension but literal int values are sometimes too long.)


### Example:

#### Encoding Big Endian To SCALE:
```c
scale_fixed_int s_e;
encode_u128_string_to_fixed_int_scale(&s_e, "9aa57026a712f5ce0000000000000000");
```

#### Encoding Little Endian To SCALE:
```c
char big_endian[64] = { 0 };
swap_u128_le_to_be(big_endian, "cef512a72670a59a0000000000000000");
encode_u128_string_to_fixed_int_scale(&s_e, big_endian);
```