#include "ft_nm.h"

uint64_t convert_endian64(uint64_t value, unsigned char data_encoding) {
    // printf("elf_64.is_bigendian %d\n", data_encoding);
    if (data_encoding == ELFDATA2MSB) {
        elf_64.is_bigendian = ELFDATA2MSB;
        return ((value & 0xFF00000000000000ULL) >> 56) |
               ((value & 0x00FF000000000000ULL) >> 40) |
               ((value & 0x0000FF0000000000ULL) >> 24) |
               ((value & 0x000000FF00000000ULL) >> 8)  |
               ((value & 0x00000000FF000000ULL) << 8)  |
               ((value & 0x0000000000FF0000ULL) << 24) |
               ((value & 0x000000000000FF00ULL) << 40) |
               ((value & 0x00000000000000FFULL) << 56);
    }
    return value; // Assume little endian by default
}

uint32_t convert_endian32(uint32_t value, unsigned char data_encoding) {
    if (data_encoding == ELFDATA2MSB) {
        elf_32.is_bigendian = ELFDATA2MSB;
        return ((value & 0xFF000000) >> 24) |
               ((value & 0x00FF0000) >> 8)  |
               ((value & 0x0000FF00) << 8)  |
               ((value & 0x000000FF) << 24);
    }
    // printf("elf_32.is_bigendian %d\n",elf_32.is_bigendian);
    return value; // Assume little endian by default
}


uint16_t convert_endian16(uint16_t value, unsigned char data_encoding) {
    if (data_encoding == ELFDATA2MSB) {
        return (value >> 8) | (value << 8);
    }
    return value; // Assume little endian by default
}