#include "ft_nm.h"

t_flags flags;
char *text_sections[4]  = { ".text", ".init", 
                            ".fini", NULL
                        };
char *data_sections[6] = {  ".data", ".init_array", 
                            ".fini_array", ".dynamic",
                            ".got", NULL,
                        };
char *ro_sections[5] = {    ".rodata", ".eh_frame", 
                            ".eh_frame_hdr", ".note.ABI-tag", NULL,
                        };

char *formatted_address(uint64_t address) {
    char *formatted_address = malloc(sizeof(char) * 19); // "0000000000004010" + '\0'
    if (!formatted_address)
        return NULL;

    for (int i = 0; i < 19; i++) {
        formatted_address[i] = '0';
    }
    formatted_address[18] = '\0';

    int i = 17;
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

    strtab = (char *) malloc(strtab_size);
    ft_memcpy(strtab, (char *)(file_data + strtab_offset), strtab_size);

    return strtab;
}

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

int is_section(char *section_name, char **sections) {

    for (int i = 0; sections[i] != NULL; i++) {
        if (!ft_strncmp(section_name, sections[i], ft_strlen(sections[i])))
            return 0;
    }
    return 1;
}

char get_final_symbol_type(unsigned int type, unsigned int bind, char *section_name) {
    char final_type = '?';
    
    if (bind == STB_WEAK) {
        if (!ft_strlen(section_name))
            final_type = 'w';
        else
            final_type = 'W';
    } else if (bind == STB_GLOBAL && !ft_strlen(section_name))
        final_type = 'U';
    else {
        if (!is_section(section_name, text_sections)) {
            if (bind == STB_LOCAL)
                final_type = 't';
            else if (bind == STB_GLOBAL)
                final_type = 'T';
        } else if (!ft_strncmp(section_name, ".bss", ft_strlen(".bss"))) {
            if (bind == STB_LOCAL)
                final_type = 'b';
            else if (bind == STB_GLOBAL)
                final_type = 'B'; 
        } else if (type == STT_COMMON && bind == STB_GLOBAL)
            final_type = 'C';
        else if (!is_section(section_name, data_sections)) {
            if (bind == STB_LOCAL)
                final_type = 'd';
            else if (bind == STB_GLOBAL)
                final_type = 'D';
        }
        else if (!is_section(section_name, ro_sections)) {
            if (bind == STB_LOCAL)
                final_type = 'r';
            else
                final_type = 'R';            
        } 
        // U a faire, V v 
    }
    return final_type;
}

int analyze_file(uint8_t *file_data) {
    Elf64_Ehdr *file_hdr;
 
    file_hdr = (Elf64_Ehdr *) file_data;

    if (file_hdr->e_ident[EI_MAG0] != ELFMAG0 || file_hdr->e_ident[EI_MAG1] != ELFMAG1 ||
            file_hdr->e_ident[EI_MAG2] != ELFMAG2 || file_hdr->e_ident[EI_MAG3] != ELFMAG3) {
        printf("Not an ELF file!\n");
        return 1;
    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        ft_putstr_fd("ELF 32\n", 2);
    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        handle_elf_64(file_hdr, file_data);
    }
    return 0;
}

t_flags flags_init(int ac, char **av) {
    t_flags flags;

    flags.a = false, flags.g = false, flags.u = false; flags.r = false, flags.p = false;
    for (int i = 1; i < ac - 1; i++) {
        if (!ft_strncmp(av[i], "-a", 2))
            flags.a = true;
        if (!ft_strncmp(av[i], "-g", 2))
            flags.g = true;
        if (!ft_strncmp(av[i], "-u", 2))
            flags.u = true;
        if (!ft_strncmp(av[i], "-r", 2))
            flags.r = true;
        if (!ft_strncmp(av[i], "-p", 2))
            flags.p = true;
    }
    if (flags.p && flags.r)
        flags.r = false;
    if (flags.u && flags.g)
        flags.g = false;
    return flags;
}

int main(int ac, char **av) {
    if (ac >= 2) {
        struct stat buf;
        uint8_t *file_data;

        int fd = open(av[ac - 1], O_RDONLY, S_IRUSR);
        if (fd == -1) {
            ft_putstr_fd("Failed to open file\n", 2);
            close(fd);
            return 1;
        }
        if (fstat(fd, &buf) != 0) {
            ft_putstr_fd("couldn't get file size.\n", 2);
            close(fd);
            return 1;
        }
        if (S_ISDIR(buf.st_mode)) {
            ft_putstr_fd("Argument must be file.\n", 2);
            close(fd);
            return 1;
        }
        flags = flags_init(ac, av);
        // printf("a: %d g: %d u: %d r: %d p: %d", flags.a, flags.g, flags.u, flags.r, flags.p);
        file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        analyze_file(file_data);
        close(fd);
    } 
    else {
        ft_putstr_fd("Usage: ./ft_nm [option(s)] [file(s)]\n List symbols in [file(s)]\n The options are:\n", 2);
        ft_putstr_fd("  -a,       Display debugger-only symbols\n", 1);
        ft_putstr_fd("  -g,       Display only external symbols\n", 1);
        ft_putstr_fd("  -p,       Do not sort the symbols\n", 1);
        ft_putstr_fd("  -r,       Reverse the sense of the sort\n", 1);
        ft_putstr_fd("  -u,       Display only undefined symbols\n", 1);
    }
    return 0;
}