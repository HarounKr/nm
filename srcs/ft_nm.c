#include "ft_nm.h"

t_options options;
t_elf_64 elf_64;
t_elf_32 elf_32;

char *text_sections[4]  = { ".text", ".init", ".fini", NULL };
char *data_sections[6] = {  ".data", ".fini_array", ".init_array", ".dynamic", ".got", NULL,};
char *ro_sections[5] = { ".rodata", ".eh_frame",  ".eh_frame_hdr",".note.ABI-tag", NULL,};
char *weak_sections[4] = { ".rodata", ".data", ".bss", NULL };

const char *get_elf_symbol_type(unsigned int type) {
    switch (type) {
        case STT_NOTYPE:   return "NOTYPE";
        case STT_OBJECT:   return "OBJECT";
        case STT_FUNC:     return "FUNC";
        case STT_SECTION:  return "SECTION";
        case STT_FILE:     return "FILE";
        case STT_COMMON:   return "COMMON";
        case STT_TLS:      return "TLS";
        default:           return "<unknown>";
    }
}

char *formatted_address(uint64_t address) {
    int i = 15;
    char *formatted_address = ft_calloc(17, sizeof(char)); // "0000000000004010" + '\0'

    if (!formatted_address)
        return NULL;
    for (int i = 0; i < 16; i++) {
        formatted_address[i] = '0';
    }
    formatted_address[16] = '\0';
    while (address > 0 && i >= 0) {
        int digit = address % 16;
        formatted_address[i] = digit < 10 ? '0' + digit : 'a' + digit - 10;
        address /= 16;
        --i;
    }
    return formatted_address;
}

char *get_strtab(uint8_t *file_data, uint64_t strtab_size, Elf64_Off strtab_offset) {
    char *strtab;

    strtab = ft_calloc(strtab_size, sizeof(char *));
    ft_memcpy(strtab, (char *)(file_data + strtab_offset), strtab_size);

    return strtab;
}

int is_section(char *section_name, char **sections) {
    for (int i = 0; sections[i] != NULL; i++) {
        // printf("sections[i] : %s\n", sections[i]);
        if (!ft_strncmp(section_name, sections[i], ft_strlen(section_name)))
            return 0;
    }
    return 1;
}

char get_final_symbol_type(unsigned int type, unsigned int bind, unsigned int size, char *section_name) {
    // printf("type  name : %-10s | ", get_elf_symbol_type(type));
    // printf("type  : %-10d | ", type);
    // printf("bind  : %-10d | ", bind);
    // printf("section  : %-30s |", section_name);
    // (void)size;
    if (bind == STB_WEAK) {
        if (!ft_strlen(section_name))
            return 'w';
        else if (is_section(section_name, weak_sections) || size == 0)
            return 'W';
        else
            return 'V';
    } else if (bind == STB_GLOBAL && !ft_strlen(section_name))
        return 'U';
    else {
        if (!is_section(section_name, text_sections)) {
            if (bind == STB_LOCAL)
                return 't';
            else if (bind == STB_GLOBAL)
                return 'T';
        } else if (!ft_strncmp(section_name, ".bss", ft_strlen(".bss"))) {
            if (bind == STB_LOCAL)
                return 'b';
            else if (bind == STB_GLOBAL)
                return 'B'; 
        } else if (type == STT_COMMON && bind == STB_GLOBAL)
            return 'C';
        else if (!is_section(section_name, data_sections)) {
            if (bind == STB_LOCAL)
                return 'd';
            else if (bind == STB_GLOBAL)
                return 'D';
        }
        else if (!is_section(section_name, ro_sections)) {
            if (bind == STB_LOCAL)
                return 'r';
            else
                return 'R';            
        } 
    }
    return '?';
}

uint64_t convert_endian64(uint64_t value, unsigned char data_encoding) {
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
    return value; // Assume little endian by default
}


uint16_t convert_endian16(uint16_t value, unsigned char data_encoding) {
    if (data_encoding == ELFDATA2MSB) {
        return (value >> 8) | (value << 8);
    }
    return value; // Assume little endian by default
}

static int handle_elf_errors(Elf64_Ehdr *file_hdr, uint8_t *file_data, char *filename, long int st_size) {
    Elf64_Ehdr *hdr64;
    Elf32_Ehdr* hdr32;
    Elf64_Off shoff;
    Elf64_Off phoff;
    uint16_t shnum;
    unsigned long sizeof_hdr = 0;
    
    if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        hdr64 = (Elf64_Ehdr*) file_data;
        shoff = convert_endian64(hdr64->e_shoff, file_hdr->e_ident[EI_DATA]);
        phoff = convert_endian64(hdr64->e_phoff, file_hdr->e_ident[EI_DATA]);
        elf_64.e_shstrndx = convert_endian16(hdr64->e_shstrndx, file_hdr->e_ident[EI_DATA]);
        shnum = hdr64->e_shnum;
        elf_64.e_shoff = shoff;
        elf_64.e_shnum = shnum;
        sizeof_hdr = sizeof(Elf64_Ehdr);
        
        printf("phoff : %ld\n", phoff);
        printf("shoff : %ld\n",shoff);
        printf("e_shnum : %d\n", shnum);
        printf("st_size : %ld\n", st_size);
    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        hdr32 = (Elf32_Ehdr*) file_data;
        shoff = convert_endian32(hdr32->e_shoff, file_hdr->e_ident[EI_DATA]);
        phoff = convert_endian32(hdr32->e_phoff, file_hdr->e_ident[EI_DATA]);
        shnum = convert_endian16(hdr32->e_shnum, file_hdr->e_ident[EI_DATA]);
        elf_32.e_shstrndx = convert_endian16(hdr32->e_shstrndx, file_hdr->e_ident[EI_DATA]);
        elf_32.e_shoff = shoff;
        elf_32.e_shnum = shnum;
        sizeof_hdr = sizeof(Elf32_Ehdr);

        printf("phoff : %ld\n", phoff);
        printf("shoff : %ld\n",shoff);
        printf("e_shnum : %d\n", shnum);
        printf("st_size : %ld\n", st_size);
        printf("e_shstrndx : %d\n", elf_32.e_shstrndx);
    }
    if (phoff == 0 || shoff == 0 || shnum == 0 || shoff < sizeof_hdr || shoff >= (Elf64_Off) st_size)
            return print_error(filename, ": file format not recognized 2\n", NULL, false);
    else if (file_hdr->e_ident[EI_DATA] != ELFDATA2LSB && file_hdr->e_ident[EI_DATA] != ELFDATA2MSB)
        return print_error(filename, ": file format not recognized 3\n", NULL, false);
    else if (file_hdr->e_ident[EI_VERSION] != EV_CURRENT)
        return print_error(filename, ": file format not recognized 4\n", NULL, false);

    return 0;
}

int define_elf_type(uint8_t *file_data, char *filename, long int st_size) {
    
    Elf64_Ehdr *file_hdr;
    file_hdr = (Elf64_Ehdr *) file_data;

    if (file_hdr->e_ident[EI_MAG0] != ELFMAG0 || file_hdr->e_ident[EI_MAG1] != ELFMAG1 ||
            file_hdr->e_ident[EI_MAG2] != ELFMAG2 || file_hdr->e_ident[EI_MAG3] != ELFMAG3)
        return print_error(filename, ": file format not recognized 1\n", NULL, false);
    else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        printf("ca va dans le 32\n");
        if (handle_elf_errors(file_hdr, file_data, filename, st_size))
            return 1;
        Elf32_Ehdr *file_hdr_32 = (Elf32_Ehdr *) file_data;
        return handle_elf_32(file_hdr_32, file_data, elf_32);
    } 
    else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        if (handle_elf_errors(file_hdr, file_data, filename, st_size))
            return 1;
        return handle_elf_64(file_hdr, file_data, elf_64);
    } else
        return print_error(filename, ": file format not recognized 5\n", NULL, false);
    return 0;
}

int parse_args(int ac, char **av) {
    options.files_name =  ft_calloc(ac, sizeof(char *));
    options.files_nb = 0;
    options.a = false, options.g = false, options.u = false; options.r = false, options.p = false;

    for (int i = 1; i < ac; i++) {
        if (av[i][0] == '-') {
            for (size_t j = 1; j < ft_strlen(av[i]); j++) {
                if (av[i][j] == 'a')
                    options.a = true;
                else if (av[i][j] == 'g')
                    options.g = true;
                else if (av[i][j] == 'u')
                    options.u = true;
                else if (av[i][j] == 'r')
                    options.r = true;
                else if (av[i][j] == 'p')
                    options.p = true;
                else {
                    free(options.files_name);
                    return 1;
                }
            }
        } else {
            options.files_name[options.files_nb] = ft_strdup(av[i]);
            options.files_nb++;
        }
    }
    if (options.p && options.r)
        options.r = false;
    if (options.u && options.g)
        options.g = false;
    return 0;
}

int main(int ac, char **av) {
    if (ac >= 2) {
        struct stat buf;
        uint8_t *file_data;
        if (parse_args(ac, av)) {
            help_output();
            return 1;
        }
        for (int i = 0; i < options.files_nb; i++) {
            int fd = open(options.files_name[i], O_RDONLY, S_IRUSR);
            if (fd == -1)
               return print_error(options.files_name[i], " No such file\n", NULL, true);
            else if (fstat(fd, &buf) != 0) {
                close(fd);
                return print_error(options.files_name[i], " couldn't get file size\n", NULL, true);
            }
            else if (S_ISDIR(buf.st_mode)) {
                close(fd);
                return print_error(options.files_name[i], " is a directory\n", "Warning", true);
            }
            else {
                file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (file_data == MAP_FAILED) {
                    close(fd);
                    return print_error(options.files_name[i],"mapped memory failed\n", NULL, true);
                }
                options.file_name = ft_strdup(options.files_name[i]);
                define_elf_type(file_data, options.files_name[i], buf.st_size);
                munmap(file_data, buf.st_size);
                free(options.file_name);
                options.file_name = NULL;
                close(fd);
            }
        }
        for (int i = 0; i <= options.files_nb; i++) {
            free(options.files_name[i]);
            options.files_name[i] = NULL;
        }
        free(options.files_name);
        options.files_name = NULL;
    } 
    else {
        help_output();
        return 1;   
    }
    return 0;
}
 
    // printf("%ld\n", file_hdr->e_phoff);
    // printf("%ld\n", file_hdr->e_shoff);
    // printf("%d\n\n", file_hdr->e_shnum);

    //  printf("%d\n", file_hdr_32->e_phoff);
    // printf("%d\n", file_hdr_32->e_shoff);
    // printf("%d\n", file_hdr_32->e_shnum);
    // // printf("%d\n", file_hdr->e_type);
    // // printf("%s\n", file_hdr->e_ident);
    // printf("size: %ld\n",st_size);
    // printf("Taille de l'entête ELF32 : %lu octets\n", sizeof(Elf32_Ehdr));
    // printf("Taille de l'entête ELF64 : %lu octets\n", sizeof(Elf64_Ehdr));