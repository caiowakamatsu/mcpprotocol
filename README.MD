# mcpprotocol
What's the name? mc cpp protocol, but written with funny word play.

This serves as a quick, pain-free, easy way to get started with using the Minecraft protocol in your own project.

## Installation
This library depends on CMake, have a modern version and you'll (probably) be fine.

```cmake
include(FetchContent)

FetchContent_Declare(
        mcpprotocol
        GIT_REPOSITORY https://github.com/caiowakamatsu/mcpprotocol.git
        GIT_TAG        main
)

FetchContent_MakeAvailable(mcpprotocol)

target_link_libraries(your_target PRIVATE mcpprotocol)
```

## Usage
Please bear with this library, it's still in it's early stages so documentation is sparse. 
Instead of starting out with the (little) documentation, I'll explain the motivation behind design decisions.

All packets are defined by a struct with static methods for serialization / deserialization, 
this means there are no actual packet structs that store information (They're all arguments). 
```cpp
static std::vector<std::byte> serialize(std::int64_t keep_alive_id);
static void handle(auto base_handle, std::span<const std::byte> source);
```

Now you might be wondering, how about the types like a chunk, nbt, uuid, etc...? Well that's a great question - they don't exist!
This library is designed to allow users to BYOT (bring your own type) for these types. The following are the types you will need to provide
```
mcp::uuid
mcp::text_component
mcp::nbt
mcp::position
mcp::command_node
mcp::slot
mcp::chunk_data
mcp::particle_data
mcp::chunk_light_data
mcp::entity_metadata
```

Defining a user-type is as simple as creating a struct like this.
```cpp
struct uuid_converter {
    using type_source = mcp::uuid;
    using type_target = my_uuid;

    [[nodiscard]] static type_target from(mcp::reader &reader) {
        auto uuid = my_uuid();
        uuid.low = reader.read<std::uint64_t>();
        uuid.high = reader.read<std::uint64_t>();
        return uuid;
    }

    static void to(my_uuid source, mcp::writer &writer) {
        writer.write(source.low);
        writer.write(source.high);
    }
};
```

There is a basic example in the examples folder, it's a program that connects to localhost and does the handshake sequence.
