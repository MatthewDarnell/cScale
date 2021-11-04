## Booleans in cScale

[Booleans](https://docs.substrate.io/v3/advanced/scale-codec/#boolean) represent bool values true and false. Encoded values are 1 byte wide.

---

### Example:

##### Encoding:
```c
#include <stdbool.h>
//...
scale_boolean boolean;
encode_boolean(&boolean, true);
```

#### Serializing Into Bytes:
```c
uint8_t serialized = 0;
serialize_boolean(&serialized, &boolean);
```

#### Deserializing From Bytes:
```c
deserialize_boolean(&boolean, &serialized);
```

#### Serializing Into Hex String:
```c
char* hex = decode_boolean_to_hex(&boolean);
free(fixed_hex);
```

#### Deserializing From Hex String:
```c
encode_boolean_from_hex(&boolean, hex);
```