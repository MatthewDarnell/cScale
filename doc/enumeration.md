## Enumerations in cScale

[Enums](https://docs.substrate.io/v3/advanced/scale-codec/#enumerations-tagged-unions) represent user-defined enum values.

In cScale, the structure `scale_enum_type` is encoded with a list of Enum names along with their respective C data types.

From the Rust Docs,
``` 
enum IntOrBool {
  Int(u8),
  Bool(bool),
}
```

would then be `uint8_t` or `bool`


You can find more examples in `src/test/enumeration.c`

---

### Example:

##### Encoding:
```c
scale_enum_type CustomEnum; //Our scale_enum_type to encode
const char *strCustomEnum[][2] = {  //A double array of strings containing Names and Types
  {"Int", "Bool"},  //Int Or Bool
  {"uint8_t", "bool"} //uint8_t or bool
};
//Encode this scale_enum_type with the list of Enum names with their data types
encode_scale_enum_type(&CustomEnum, 2, (char**)strCustomEnum[0], (char**)strCustomEnum[1]);
```

#### Serializing Into Bytes:
```c

uint8_t bytes[32] = { 0 };  //Fixed Int Serialized
uint64_t len = 0;

uint8_t value = 42;
scale_fixed_int v;
encode_int_to_fixed_int_scale(&v, value);
serialize_fixed_int(bytes, &len, &v);

//Now Encode as Int(u8)
uint8_t serialized[32] = { 0 }; //Output for serialized Int(u8) scale enum
encode_enumeration(serialized, &CustomEnum, "Int", bytes, (size_t*)&len);
//serialized now contains the serialized enum
```

#### Deserializing From Bytes:
```c
uint8_t bytes[32] = { 0 };
uint16_t offset = 0;
decode_enumeration(bytes, &offset, &CustomEnum, enum_bytes, (size_t*)&len);
//bytes now contains the serialized uint8_t
```