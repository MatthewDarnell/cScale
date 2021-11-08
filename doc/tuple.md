## Tuples in cScale

[Tuples](https://docs.substrate.io/v3/advanced/scale-codec/#tuples) represent pairs of values (tuples).

Tuples in SCALE are simply concatenated fixed-width serialized types. Currently cScale is supporting most types using the helper enum `scale_type`

```c
enum scale_type { 
  FIXED_INT, 
  COMPACT_INT, 
  BOOLEAN,
  VECTOR, 
  STRING  
};
```

If you want to use a custom data structure, Option, or Enum, you can simply call its serialization function and concatenate the resulting bytes with the other value.

---

### Example:

##### Serializing Into Bytes::
```c
scale_compact_int compact = SCALE_COMPACT_INT_INIT; //compact int
encode_compact(&compact, (uint8_t)3);

scale_boolean boolean = { 0 };  //boolean
encode_boolean(&boolean, false);

uint8_t serialized[32] = { 0 };
size_t len = 0;
serialize_as_tuple(serialized, &len, COMPACT_INT, (void*)&compact, BOOLEAN, (void*)&boolean);
cleanup_scale_compact_int(&compact);
//serialized now contains [0x0c, 0x00]
```

#### Deserializing From Bytes:
```c
//We know this contains a compact uint8_t and a bool, so 1 byte each.
//therefore, we can parse them using the appropriate function for each value
scale_compact_int compact = SCALE_COMPACT_INT_INIT; //compact int
read_compact_int_from_data(&compact_int_elem, serialized);

scale_boolean boolean;
deserialize_boolean(&boolean, &serialized[1]);

//Each value now contains its part of the tuple
cleanup_scale_compact_int(&compact);
```
