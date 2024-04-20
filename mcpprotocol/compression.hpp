#ifndef MCPPROTOCOL_COMPRESSION_HPP
#define MCPPROTOCOL_COMPRESSION_HPP

namespace mcp {
    inline std::vector<std::byte> TEMP_reverse(std::span<const std::byte> compressed_data) {
        // Promote span to vector using constructor
        std::vector<std::byte> data(compressed_data.begin(), compressed_data.end());

        // Reverse the vector
        std::reverse(data.begin(), data.end());

        return data;
    }

    inline std::vector<std::byte> decompress(std::span<const std::byte> compressed_data, uint32_t size_hint) {
        return TEMP_reverse(compressed_data);
        throw std::runtime_error("TODO: decompression"); // TODO: decompression
    }

    inline std::vector<std::byte> compress(std::span<const std::byte> uncompressed_data) {
        return TEMP_reverse(uncompressed_data);
        throw std::runtime_error("TODO: decompression"); // TODO: decompression
    }
}
#endif //MCPPROTOCOL_COMPRESSION_HPP
