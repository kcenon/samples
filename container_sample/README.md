# Container Sample

## Overview

This sample demonstrates the container_system features including:

- Type-safe data container with multiple value types
- XML serialization and deserialization
- JSON serialization and deserialization
- Container manipulation (add, remove, merge)
- Support for various data types (bool, int, long, float, double, string, nested containers)

## Building

```bash
cd /path/to/samples
mkdir -p build && cd build
cmake .. -DSAMPLES_USE_LOCAL_SYSTEMS=ON
make container_sample
```

## Running

```bash
cd build
./bin/container_sample
```

## Expected Output

```
Container sample starting...
Using default configuration (log_level=Information, style=ConsoleOnly)
Logger started: container_sample
[INFO] [0.004875ms] data serialize:
[double_value] = 1.234567890123456789
[float_value] = 1.234567
[true_value] = true
[false_value] = false

[INFO] [0.013291ms] data xml:
<container>
  <item key="double_value">1.234567890123456789</item>
  <item key="float_value">1.234567</item>
  <item key="true_value">true</item>
  <item key="false_value">false</item>
</container>

[INFO] [0.0215ms] data json:
{
  "double_value": "1.234567890123456789",
  "float_value": "1.234567",
  "true_value": "true",
  "false_value": "false"
}

[INFO] data serialize:
[container_value] = nested container example
[ulong_value] = 18446744073709551615
[llong_value] = 9223372036854775807
[true_value] = true
[long_value] = 9223372036854775807
[double_value] = 1.234567890123456789
...
Logger stopped: container_sample
```

## Features Demonstrated

### 1. Creating and Populating Containers

```cpp
value_container container;
container.add(std::make_shared<value>("false_value", value_types::bool_value, "false"));
container.add(std::make_shared<value>("true_value", value_types::bool_value, "true"));
container.add(std::make_shared<value>("float_value", value_types::float_value, "1.234567"));
container.add(std::make_shared<value>("double_value", value_types::double_value, "1.234567890123456789"));
```

### 2. Serialization Formats

```cpp
// Serialize to human-readable format
std::string serialized = container.serialize();

// Convert to XML
std::string xml = container.to_xml();

// Convert to JSON
std::string json = container.to_json();
```

### 3. Deserialization

```cpp
// From XML
value_container from_xml;
from_xml.from_xml(xml_string);

// From JSON
value_container from_json;
from_json.from_json(json_string);
```

### 4. Container Manipulation

```cpp
// Copy constructor
value_container copy(original);

// Add more values
copy.add(std::make_shared<value>("new_key", value_types::string_value, "new_value"));

// Remove values
copy.remove("old_key");

// Get value
auto val = copy.get_value("key_name");
if (val) {
    std::cout << val->data() << std::endl;
}
```

## Supported Value Types

- `bool_value` - Boolean values (true/false)
- `int_value` - 32-bit integers
- `long_value` - 64-bit integers
- `ulong_value` - Unsigned 64-bit integers
- `llong_value` - Long long integers
- `ullong_value` - Unsigned long long integers
- `float_value` - Single precision floating point
- `double_value` - Double precision floating point
- `string_value` - String values
- `container_value` - Nested containers

## Key Concepts

- **Type Safety**: Each value knows its type and can be safely cast
- **Serialization**: Containers can be converted to/from XML, JSON, and custom formats
- **Nested Structures**: Containers can hold other containers for complex data structures
- **Performance**: Efficient value lookup with internal indexing
- **Copy-on-Write**: Efficient copying with shared internal state

## Use Cases

- **Configuration Management**: Store and serialize application settings
- **Data Exchange**: Convert between different serialization formats
- **Message Passing**: Package data for inter-process communication
- **State Persistence**: Save and restore application state

## Architecture

```
value_container
    ├── value (polymorphic base)
    │   ├── bool_value
    │   ├── numeric_values (int, long, float, double, etc.)
    │   ├── string_value
    │   └── container_value (recursive)
    │
    └── Serialization
        ├── to_xml() / from_xml()
        ├── to_json() / from_json()
        └── serialize() (custom format)
```

## Related Samples

- [Logging Sample](../logging_sample/README.md) - Async logging system
- [Threads Sample](../threads_sample/README.md) - Thread pool and job scheduling
- [Echo Server](../echo_server/README.md) - Network server using containers for messages
