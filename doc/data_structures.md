## Structures in cScale

[Data Structures](https://docs.substrate.io/v3/advanced/scale-codec/#data-structures) represent custom data structures. In cScale these are user-defined `struct` values.

These work in cScale by defining user created functions for encoding and decoding a structure and passing them as function pointers.

There exists a `scale_structure` `struct` in cScale which can be used to help maintain the relevant functions for a `struct`, but this is not necessary to use.

Serialization functions have the signature: `void serialize(uint8_t* serialized, size_t *bytes, void *structure);`
Deserialization functions have the signature: `void deserialize(void *structure_out, uint8_t *bytes, size_t len);`

---

### Example:

##### A Typical Structure:
```c
/* The User-Defined Structure to Serialize/Deserialize */
struct Structure { 
  bool a; //boolean
  int8_t b; //fixed int
  scale_structure scale_encoder;  //helper, contains our serialization functions
};
```

#### Serialization Function:
```c
/* read a void* Structure @structure and write a serialized scale byte array of length @bytes to @serialized */
void struct_Structure_serialize(uint8_t* serialized, size_t *bytes, void *structure) {
  struct Structure *value = (struct Structure*)structure; //Cast void* as our Structure
  
  scale_boolean bool_a; //our bool
  scale_fixed_int int8_b; //our fixed int
  
  encode_boolean(&bool_a, value->a);  //encode the Structure.a as a boolean
  serialize_boolean(serialized, &bool_a); //write serialized boolean to out bytes, length of boolean is 1
  *bytes = 1; //have written 1 byte so far
  encode_int_to_fixed_int_scale(&int8_b, value->b); //encode Structure.b as a fixed int
  uint64_t len = 0;
  serialize_fixed_int(&serialized[1], &len, &int8_b); //serialize the fixed int and write it to out bytes
  *bytes += len;  //increment *bytes (length of serialized out bytes) by length of our fixed int
}
```

#### Deserialization Function:
```c
/* read serialized scale @bytes of length @len, write a Structure to @structure_out */
void struct_Structure_deserialize(void *structure_out, uint8_t *bytes, size_t len) {
  struct Structure *value = (struct Structure*)structure_out; //Cast this void* to our Structure
  scale_boolean bool_a; //a scale_boolean
  scale_fixed_int *int8_b = &value->b;  //assign this fixed int to Structure.b
  deserialize_boolean(&bool_a, bytes);  //Deserialize the boolean at bytes[0]
  value->a = decode_boolean(&bool_a);  //assign this boolean value to Structure.a
  deserialize_fixed_int(&value->b, &bytes[1], 1, true); //Deserialize fixed int starting at bytes[1]. It's signed and 1 byte long (i8). Populate Structure.b
}
```

#### Using `scale_structure` Helper:
```c
//User Defined Struct
struct Structure my_struct;
my_struct.a = false;
my_struct.b = 42;
my_struct.scale_encoder.serialize = &struct_Structure_serialize;  //Assign our serialization function as function pointer
my_struct.scale_encoder.deserialize = &struct_Structure_deserialize;  //Assign our deserialization function as function pointer
```

#### Serializing Into Bytes:
```c
uint8_t serialized[32] = { 0 };
size_t len = 0;
my_struct.scale_encoder.serialize(serialized, &len, &my_struct);
```

#### Deserializing From Bytes:
```c
struct Structure deserialized;
my_struct.scale_encoder.deserialize(&deserialized, serialized, len);
```

---

`scale_structure` helps especially when nesting structures. For example:

```c
struct Structure {
  bool a;
  int8_t b;
  scale_structure scale_encoder;
};

struct MyStruct {
  uint32_t a;
  struct Structure b;
  scale_structure scale_encoder;
};

struct Structure structure = { .a = false, .b = 1, .scale_encoder.serialize = &struct_Structure_serialize, .scale_encoder.deserialize = &struct_Structure_deserialize};

//Make sure struct_MyStruct_serialize calls Structure->scale_encoder.serialize(), and same for deserialize
struct MyStruct b = { .a = 1, .b = structure, .scale_encoder.serialize = &struct_MyStruct_serialize, .scale_encoder.deserialize = &struct_MyStruct_deserialize};

uint8_t serialized[32] = { 0 };
size_t len = 0;
b.scale_encoder.serialize(serialized, &len, &b);
```

You can see an example of this in `src/test/structure.c`