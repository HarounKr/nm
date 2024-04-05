#include "ft_nm.h"

// int handle_file_errors(int fd, struct stat buf) {
    
//     return 0;
// }
int x;

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

void insertion_sort(symbol_data *array, int n) {
    int i, j;
    symbol_data key;

    for (i = 1; i < n; i++) {
        key = array[i];
        j = i - 1;

        while (j >= 0 && ft_strncmp(array[j].symbol_name, key.symbol_name, ft_strlen(key.symbol_name)) > 0) {
            array[j + 1] = array[j];
            j = j - 1;
        }
        array[j + 1] = key;
    }
}

int is_section(char *section_name, char **sections) {

    for (int i = 0; sections[i] != NULL; i++) {
        if (!ft_strncmp(section_name, sections[i], ft_strlen(sections[i])))
            return 0;
    }
    fflush(stdout);
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
    Elf64_Shdr *sections_hdr;
    Elf64_Shdr *symtab_hdr;
    Elf64_Shdr *strtab_hdr;
    Elf64_Sym *symtab;
    uint16_t e_shstrndx;
    int symbols_offset;
    char *strtab;
    char *shstrtab;
    char *name;
    file_hdr = (Elf64_Ehdr *) file_data;

    if (file_hdr->e_ident[EI_MAG0] != ELFMAG0 || file_hdr->e_ident[EI_MAG1] != ELFMAG1 ||
            file_hdr->e_ident[EI_MAG2] != ELFMAG2 || file_hdr->e_ident[EI_MAG3] != ELFMAG3) {
        printf("Not an ELF file!\n");
        return 1;
    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        ft_putstr_fd("ELF 32\n", 2);
    } else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        sections_hdr = (Elf64_Shdr *) (file_data + file_hdr->e_shoff);
        e_shstrndx = file_hdr->e_shstrndx;
        shstrtab = get_strtab(file_data, sections_hdr[e_shstrndx].sh_size, sections_hdr[e_shstrndx].sh_offset);

        for (int i = 0; i < file_hdr->e_shnum; i++) {
            name = &shstrtab[sections_hdr[i].sh_name];
            if (!ft_strncmp(name, ".symtab", ft_strlen(".symtab")))
                symtab_hdr = &sections_hdr[i];
            else if (!ft_strncmp(name, ".strtab", ft_strlen(".strtab")))
                strtab_hdr = &sections_hdr[i];
        }
        symtab = (Elf64_Sym *)(file_data + symtab_hdr->sh_offset);
        symbols_offset = symtab_hdr->sh_size / sizeof(Elf64_Sym);
        strtab = get_strtab(file_data, strtab_hdr->sh_size, strtab_hdr->sh_offset);
        for (int i = 0; i < symbols_offset; i++) {
            name = &strtab[symtab[i].st_name];
            if (ft_strlen(name) != STT_NOTYPE && ELF64_ST_TYPE(symtab[i].st_info) != STT_FILE) {
                // printf("st_info  : %-10s   | ", get_elf_symbol_type(ELF64_ST_TYPE(symtab[i].st_info)));
                // printf("Type     : %-10c |", get_final_symbol_type(ELF64_ST_BIND(symtab[i].st_info), ELF64_ST_BIND(symtab[i].st_info), &shstrtab[sections_hdr[symtab[i].st_shndx].sh_name])); 
                // printf("st_bind  : %-10d |", ELF64_ST_BIND(symtab[i].st_info)); 
                // printf("st_shndx : %-10d |", symtab[i].st_shndx); 
                // printf("section  : %-10s | ", &shstrtab[sections_hdr[symtab[i].st_shndx].sh_name]);
                if (symtab[i].st_shndx) {
                    printf("%s ",formatted_address(symtab[i].st_value));
                }
                else
                    printf("                   ");
                printf("%c ", get_final_symbol_type(ELF64_ST_BIND(symtab[i].st_info), ELF64_ST_BIND(symtab[i].st_info), &shstrtab[sections_hdr[symtab[i].st_shndx].sh_name]));
                printf("%s\n", name);
            }
        }
    }
    return 0;
}

int main(int ac, char **av) {
    if (ac == 2) {
        struct stat buf;
        uint8_t *file_data;

        int fd = open(av[1], O_RDONLY, S_IRUSR);
        x = 5;
        printf("%d\n", x);
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
    
        file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        analyze_file(file_data);
        close(fd);
    } 
    else {
        ft_putstr_fd("Usage: ", 2);
        ft_putstr_fd(av[0], 2);
        ft_putstr_fd("<file>\n", 2);
    }
    return 0;
}