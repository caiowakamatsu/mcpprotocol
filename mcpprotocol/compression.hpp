#ifndef MCPPROTOCOL_COMPRESSION_HPP
#define MCPPROTOCOL_COMPRESSION_HPP

#include <zlib-ng.h>

namespace mcp {
    inline std::vector<std::byte> TEMP_reverse(std::span<const std::byte> compressed_data) {
        // Promote span to vector using constructor
        std::vector<std::byte> data(compressed_data.begin(), compressed_data.end());

        // Reverse the vector
        std::reverse(data.begin(), data.end());

        return data;
    }

    inline std::vector<std::byte> decompress(std::span<const std::byte> compressed_data, uint32_t size_hint) {
        zlibng_version();
        uLongf /*mut*/ length = size_hint;
        auto *buffer = new std::byte[length];

        int errcode = zng_uncompress(reinterpret_cast<Bytef *>(buffer), &/*mut*/ length,
                                 reinterpret_cast<const Bytef *>(compressed_data.data()), compressed_data.size());

        if (errcode != Z_OK) {
            delete[] buffer;
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
        if (length != size_hint) {
            delete[] buffer;
            throw std::runtime_error("received bad size for decompression (buffer too large)");
        }

        auto result = std::vector<std::byte>(buffer, buffer + length);

        delete[] buffer;

        return result;
    }

    inline std::vector<std::byte> compress(std::span<const std::byte> uncompressed_data) {
        uLong /*mut*/ length = zng_compressBound(uncompressed_data.size());
        auto *buffer = new std::byte[length];

        int errcode = zng_compress2(reinterpret_cast<Bytef *>(buffer), &/*mut*/ length,
                                reinterpret_cast<const Bytef *>(uncompressed_data.data()),
                                uncompressed_data.size_bytes(),
                                Z_DEFAULT_COMPRESSION);

        if (errcode != Z_OK) {
            delete[] buffer;
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

        auto result = std::vector<std::byte>(buffer, buffer + length);

        delete[] buffer;

        return result;
    }
}
#endif //MCPPROTOCOL_COMPRESSION_HPP
