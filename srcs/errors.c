#include "../inc/ft_nm.h"

int print_error(char *file_name ,char *err, char *err_type, bool is_quote) {
    
    ft_putstr_fd("ft_nm: ", 2);
    if (err_type && !ft_strncmp(err_type, "Warning", ft_strlen(err_type)))
        ft_putstr_fd("Warning: ", 2);
    if (is_quote)
        ft_putchar_fd(39, 2);
    ft_putstr_fd(file_name, 2);
    if (is_quote)
        ft_putchar_fd(39, 2);
    ft_putstr_fd(err, 2);
    
    return 1;
}

int handle_elf_errors(Elf64_Ehdr *file_hdr, uint8_t *file_data, char *filename, long int st_size) {

    Elf64_Ehdr *hdr64;
    Elf32_Ehdr* hdr32;
    Elf64_Off shoff;
    Elf64_Off phoff;
    uint16_t shnum;
    uint16_t type;
    unsigned long sizeof_hdr = 0;
    
    if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        hdr64 = (Elf64_Ehdr*) file_data;
        elf_64.is_bigendian = 0;
        shoff = convert_endian64(hdr64->e_shoff, file_hdr->e_ident[EI_DATA]);
        phoff = convert_endian64(hdr64->e_phoff, file_hdr->e_ident[EI_DATA]);
        shnum = convert_endian16(hdr64->e_shnum, file_hdr->e_ident[EI_DATA]);
        type = convert_endian16(hdr64->e_type, file_hdr->e_ident[EI_DATA]);
        elf_64.e_shstrndx = convert_endian16(hdr64->e_shstrndx, file_hdr->e_ident[EI_DATA]);
        elf_64.e_shoff = shoff;
        elf_64.e_shnum = shnum;
        sizeof_hdr = sizeof(Elf64_Ehdr);

    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        hdr32 = (Elf32_Ehdr*) file_data;
        elf_32.is_bigendian = 0;
        shoff = convert_endian32(hdr32->e_shoff, file_hdr->e_ident[EI_DATA]);
        phoff = convert_endian32(hdr32->e_phoff, file_hdr->e_ident[EI_DATA]);
        shnum = convert_endian16(hdr32->e_shnum, file_hdr->e_ident[EI_DATA]);
        type = convert_endian16(hdr32->e_type, file_hdr->e_ident[EI_DATA]);
        elf_32.e_shstrndx = convert_endian16(hdr32->e_shstrndx, file_hdr->e_ident[EI_DATA]);
        elf_32.e_shoff = shoff;
        elf_32.e_shnum = shnum;
        sizeof_hdr = sizeof(Elf32_Ehdr);
    }
    if ((phoff == 0 && type != ET_REL) || shoff == 0 || shnum == 0 || shoff < sizeof_hdr || shoff >= (Elf64_Off) st_size)
            return print_error(filename, ": file format not recognized\n", NULL, false);
    else if (file_hdr->e_ident[EI_DATA] != ELFDATA2LSB && file_hdr->e_ident[EI_DATA] != ELFDATA2MSB)
        return print_error(filename, ": file format not recognized\n", NULL, false);
    else if (file_hdr->e_ident[EI_VERSION] != EV_CURRENT)
        return print_error(filename, ": file format not recognized\n", NULL, false);

    return 0;
}