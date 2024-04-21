#ifndef MCPPROTOCOL_COMPRESSION_HPP
#define MCPPROTOCOL_COMPRESSION_HPP

#include <zlib-ng.h>

namespace mcp {

    [[nodiscard]] inline std::vector<std::byte> decompress(std::span<const std::byte> compressed_data, std::size_t size_hint) {
        auto buffer = std::vector<std::byte>(size_hint);

        const auto errcode = zng_uncompress(reinterpret_cast<Bytef *>(buffer.data()), &size_hint,
                                 reinterpret_cast<const Bytef *>(compressed_data.data()), compressed_data.size());

        if (errcode != Z_OK) {
            switch (errcode) {
                case Z_MEM_ERROR:
                    throw std::runtime_error("out of memory for decompression, kill me now");
                case Z_BUF_ERROR:
                    throw std::runtime_error("received bad size for decompression (buffer too small)");
                case Z_DATA_ERROR:
                    throw std::runtime_error("received bad data for decompression (data corrupted or incomplete)");
                default:
                    throw std::runtime_error("zlib threw an undocumented error (unreachable hopefully)");
            }
        }
        if (buffer.size() != size_hint) {
            throw std::runtime_error("received bad size for decompression (buffer too large)");
        }

        return buffer;
    }

    [[nodiscard]] inline std::vector<std::byte> compress(std::span<const std::byte> uncompressed_data) {
        auto length = zng_compressBound(uncompressed_data.size());

        auto buffer = std::vector<std::byte>(length);

        const auto errcode = zng_compress2(reinterpret_cast<Bytef *>(buffer.data()), &length,
                                reinterpret_cast<const Bytef *>(uncompressed_data.data()),
                                uncompressed_data.size_bytes(),
                                Z_DEFAULT_COMPRESSION);

        if (errcode != Z_OK) {
            switch (errcode) {
                case Z_MEM_ERROR:
                    throw std::runtime_error("out of memory for compression, kill me now");
                case Z_BUF_ERROR:
                    throw std::runtime_error("zlib::compressBound lied to us (not enough space in buffer)");
                case Z_STREAM_ERROR:
                    throw std::runtime_error("zlib rejected its own default compression level (invalid compression level)");
                default:
                    throw std::runtime_error("zlib threw an undocumented error (unreachable hopefully)");
            }
        }

        buffer.resize(length);
        return buffer;
    }
}
#endif //MCPPROTOCOL_COMPRESSION_HPP
