import json


def type_lookup(str_rep):
    names = {
        "string": "std::string",
        "uuid": "mcp::uuid",
        "i64": "std::int64_t"
    }
    return names[str_rep]


def is_convertible_type(str_rep):
    if str_rep == "uuid":
        return True

    return False


def load_packets(file):
    with open(file, 'r') as loaded:
        return json.load(loaded)


def write_field_parameter(type_rep, name):
    type_name = type_lookup(type_rep)
    if is_convertible_type(type_rep):
        return f"detail::get_type_t<{type_name}, Converters...>::type_target uuid"
    else:
        return f"{type_name} {name}"


def write_packet_header(name, target, pid):
    data = "template <auto Handler>\n"
    data += f"struct {name}_{target} : public detail::packet_base<{'0x{:02x}'.format(pid)}, Handler> {{\n"

    return data


def write_serialize_field(type_rep, name):
    if is_convertible_type(type_rep):
        return f"detail::get_type_t<{type_lookup(type_rep)}, Converters...>::to(uuid, writer);"
    else:
        return f"writer.write({name});"


def write_serialize_function(fields):
    data = "\ttemplate <typename ...Converters>\n"
    data += "\tstatic std::vector<std::byte> serialize(\n"
    names = ["auto *state"]
    for field in fields:
        for name in field:
            names.append(write_field_parameter(field[name], name))
    joined = ",\n\t\t\t".join(names)
    data += f"\t\t\t{joined}) {{\n"
    data += "\t\tauto buffer = std::vector<std::byte>();\n"
    data += "\t\tauto writer = mcp::writer(buffer);\n\n"

    for field in fields:
        for name in field:
            data += f"\t\t{write_serialize_field(field[name], name)}\n"
    data += "\n\t\treturn buffer;\n\t}\n"
    return data


def write_deserialize_field(type_rep, name):
    type_name = type_lookup(type_rep)
    if is_convertible_type(type_rep):
        return f"const auto {name} = detail::get_type_t<{type_name}, Converters...>::from(reader);"
    else:
        return f"const auto {name} = reader.read<{type_name}>();"


def write_deserialize_function(packet_name, target, fields):
    data = "\ttemplate <typename ...Converters>\n"
    data += "\tstatic void handle(auto base_handle, auto *state, std::span<const std::byte> source) {\n"
    data += "\t\tauto reader = mcp::reader(source);\n\n"

    names = []
    for field in fields:
        for name in field:
            names.append(name)
            data += f"\t\t{write_deserialize_field(field[name], name)}\n"

    data += f"\n\t\t{packet_name}_{target}::handler(base_handle, {', '.join(names)});\n"
    data += "\t}\n"
    return data


def write_packet(name, target, pid, fields):
    data = write_packet_header(name, target, pid)
    data += write_serialize_function(fields)
    data += '\n'
    data += write_deserialize_function(name, target, fields)
    data += '};'
    return data


def create_packet(name, target, data):
    pid = data[0]
    fields = data[1:]
    return write_packet(name, target, pid, fields)


def main():
    packets = load_packets("packets.json")
    for mode in packets:
        for packet_name in packets[mode]["client"]:
            print(create_packet(packet_name, "c", packets[mode]["client"][packet_name]))
        for packet_name in packets[mode]["server"]:
            print(create_packet(packet_name, "s", packets[mode]["server"][packet_name]))


if __name__ == '__main__':
    main()
