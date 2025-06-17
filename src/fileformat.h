#ifndef FILEFORMAT_H
#define FILEFORMAT_H

#include <cstdint>

namespace CZipFormat {

    // unique 4-byte identifier for all czip files
    constexpr char MAGIC[4] = {'C', 'Z', 'P', '1'};

    // file format version for future upgrades
    constexpr uint8_t VERSION = 1;

    // type-safe enum for all processing operations
    enum class OperationCode : uint8_t {
        COMPRESS_ZLIB    = 0x01,
        ENCRYPT_SIMPLE   = 0x02,
        ENCRYPT_AES      = 0x03,
        CONCEAL_AURA     = 0x10,
        CONCEAL_LSB      = 0x11
    };

    // represents the fixed-size portion of the file header
    #pragma pack(push, 1) // ensure compiler does not add padding bytes
    struct FileHeader {
        char      magic[4];
        uint8_t   version;
        uint8_t   op_count;
    };
    #pragma pack(pop)

} // namespace CZipFormat

#endif // FILEFORMAT_H
