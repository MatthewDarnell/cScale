## Vectors/Strings in cScale

[Vectors](https://docs.substrate.io/v3/advanced/scale-codec/#vectors-lists-series-sets) are lists in SCALE. They can be lists of any type, and should always contain elements of a uniform data type.

Currently, helper functions used to parse vectors and read from data are assuming a data type of constant byte-width (`i8`, `u32`, etc) but this does not hold true for variable-width elements, such as `Option`. At present, variable width vectors will have to be manually parsed from data. This may change in the future, perhaps using a combination of function pointers with C11 `_Generic` or changing the underlying data of a `scale_vector` to a linked list.



---

#### Initialization And Freeing:

Make sure to assign `scale_vector` values to the macro `SCALE_VECTOR_INIT`. This guarantees that pointers are set to `NULL` and will be safe to free if left unused.

Call `cleanup_vector` to free any heap-allocated memory.
```c
scale_vector vector = SCALE_VECTOR_INIT;
...
cleanup_vector(&vector);
```


---

### Example:

##### Pushing an Item Into Vector:
```c
scale_vector vector = SCALE_VECTOR_INIT;

scale_fixed_int fixed = { 0 };
encode_int_to_fixed_int_scale(&fixed, 127);
uint8_t serialized[16] = { 0 };
size_t serialized_len = 0;
serialize_fixed_int(serialized, &serialized_len, &fixed);

push_vector(&vector, serialized, serialized_len);
```

#### Serializing Into Byte Array:
```c
uint8_t bytes[32] = { 0 };
size_t data_len = 0;
serialize_vector(bytes, &data_len, &vector);
```

#### Getting the Number of Elements in this Vector:
```c
uint64_t num_elements = decode_compact_to_u64(&vector.prefix_num_elements);
```

#### Deserializing From Byte Array (constant byte-width elements):
```c
scale_vector vector = SCALE_VECTOR_INIT;

uint8_t serialized[32]; //Populated with some serialized vector
uint8_t width = 1; //byte width of each element, perhaps a string (Vec<char>), Vec<i8>, or Vec<Bool>
size_t bytes_read = read_vector_from_data(&vector, width, serialized);
cleanup_vector(&vector);
```

#### Parsing Vector (constant byte-width elements):
```c
uint8_t *elem;
scale_vector_foreach(&elem, sizeof(uint16_t), &vector) { //2 byte elements, Vec<fixed_u16>
  uint16_t output = 0;
  deserialize_fixed_int((void*)&output, elem, sizeof(uint16_t), false);
  printf("%u\n", output);
}
```

#### Serializing Into Hex String:
```c
char *vector_hex = cscale_byte_array_to_hex(serialized, serialized_len);
free(vector_hex);
```
---

For variable width elements, manual parsing will have to be done. Following is an example:

### Example:

#### Parsing Vec<Option< i8>>
```c
uint8_t serialized_vector[32]; //contains serialized Vec<Option<i8>>
scale_vector vector = SCALE_VECTOR_INIT; //vector to populate

scale_compact_int compact_num_elems = SCALE_COMPACT_INT_INIT;
read_compact_int_from_data(&compact_num_elems, serialized);
uint8_t num_elems = (uint8_t)decode_compact_to_u64(&compact_num_elems);
size_t byte_length_num_elems = compact_int_get_byte_length(&compact_num_elems);
if(compact_num_elems.mode == SCALE_COMPACT_BIGNUM) {
  byte_length_num_elems++;  //bignum compacts contain a prefix byte + bytes which contain the numeric value
}

uint32_t offset = byte_length_num_elems;  //serialized_vector[offset] will now point to the first element
for(uint8_t i=0; i < num_elems; i++) {
    uint8_t *temp = &serialized_vector[offset];
    enum scale_option op;
    deserialize_scale_option(&op, temp);
    if(op == None) {  //1 byte
      offset++;
      push_vector(&vector, temp, 1);  //push 1 byte Option, None
    } else if(op == Some) { //1 byte + 1 byte for int8_t data type
      offset+=2;
      push_vector(&vector, temp, 2); //push 2 byte Option, Some, plus a 1 byte int8_t
    }
}

//vector now contains the Vec<Option<i8>>
cleanup_vector(&vector);
```