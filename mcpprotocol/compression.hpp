#ifndef MCPPROTOCOL_COMPRESSION_HPP
#define MCPPROTOCOL_COMPRESSION_HPP

namespace mcp {
    inline std::vector<std::byte> decompress(std::span<const std::byte> compressed_data, uint32_t size_hint) {
        throw std::runtime_error("TODO: decompression"); // TODO: decompression
    }
}
#endif //MCPPROTOCOL_COMPRESSION_HPP
