## Options in cScale

[Options](https://docs.substrate.io/v3/advanced/scale-codec/#options) represent optional values which may or may not exist. This maps to the Rust `Option` data type. Possible values are `None` or `Some`.

`scale_option` is not a struct, but an enum. It has three possible states: `None`, `Some`, and `BoolFalse`. This is because `BoolFalse` combines an `Option` and a `Boolean` type into a single byte, as opposed to other types where the serialized data type follows the `Option`.

When serializing an option, pass an array that is at least 1 byte longer than the serialized data type array. Underneath the hood, `memmove` is called to copy the `Option` byte plus the underlying data type to the new array.

---

### Example:

##### Serializing Into Bytes:
```c
/* Serialize a data type, here it's a u64 Compact Int */
scale_compact_int compact = SCALE_COMPACT_INT_INIT;
encode_uint64_to_compact_int_scale(&compact, (uint64_t)4611686018427387903);
uint8_t compact_serialized[32] = { 0 };
uint64_t compact_serialized_len = 0;

/* compact_serialized now contains bytes: 0x13FFFFFFFFFFFFFF3F */
serialize_compact_int(compact_serialized, &compact_serialized_len, &compact);

/* Serialize The Option: Some(Compact<u64>) */
uint8_t compact_option_serialized[64] = { 0 };
size_t compact_option_serialized_len = 0;

/* compact_option_serialized now contains 0x0113FFFFFFFFFFFFFF3F */
serialize_scale_option(compact_option_serialized, &compact_option_serialized_len, Some, compact_serialized, compact_serialized_len);
```

#### Deserializing From Bytes:

##### Option<Compact<u64>>:
```c
enum scale_option option;
deserialize_scale_option(&option, compact_option_serialized);
if(option == None) {
  //this option is 1 byte long, just contains None (0x00)
} else if(option == Some) {
  //compact_option_serialized[0] == 0x01
  //compact_option_serialized[1] begins the Compact<u64> value
  scale_compact_int compact = SCALE_COMPACT_INT_INIT;
  read_compact_int_from_data(&compact, &compact_option_serialized[1]);
  //do something...
  cleanup_scale_compact_int(&compact);
}
```

##### Option< Boolean >:
```c
/* Option<Boolean> is always 1 byte long, the value is included in the serialized Option */
enum scale_option option;
deserialize_scale_option(&option, boolean_serialized);
if(option == None) {
  //bool is None, no data
} else if(option == Some) {
  //bool is true
} else if(option == BoolFalse) {
  //bool is false
}
```

#### Serializing Into Hex String:
```c
char* hex = cscale_byte_array_to_hex(compact_option_serialized, compact_option_serialized_len);
free(hex);
```
