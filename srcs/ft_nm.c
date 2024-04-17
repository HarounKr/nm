#include "ft_nm.h"

// int handle_file_errors(int fd, struct stat buf) {
    
//     return 0;
// }

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

int compare_strings(const char *a, const char *b) {
    int i = 0, j = 0;
    while (a[i] != '\0' && !ft_isalpha(a[i])) 
        i++;  // Aller au premier caractère alphabétique
    while (b[j] != '\0' && !ft_isalpha(b[j])) 
        j++;  // Aller au premier caractère alphabétique

    while (a[i] != '\0' && b[j] != '\0') {
        char charA = ft_tolower(a[i]);
        char charB = ft_tolower(b[j]);

        if (charA != charB)
            return charA - charB;

        i++;
        j++;
        while (a[i] != '\0' && !ft_isalpha(a[i])) 
            i++;
        while (b[j] != '\0' && !ft_isalpha(b[j]))
            j++;
    }
    return a[i] - b[j];  // Compare the rest if one string ends
}

void insertion_sort(t_symbol_data *array, int n) {
    int i, j;
    t_symbol_data key;

    for (i = 1; i < n; i++) {
        key = array[i];
        j = i - 1;

        // Use compare_strings to compare entire strings properly
        while (j >= 0 && compare_strings(array[j].name, key.name) > 0) {
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

void fill_symdata(t_symbol_data *sym_data, t_elf_64 elf_64) {
    int sym_size = 0;
    for (int i = 0; i < elf_64.symbols_offset; i++) {
        elf_64.name = &elf_64.strtab[elf_64.symtab[i].st_name];
        if (ft_strlen(elf_64.name) != STT_NOTYPE && ELF64_ST_TYPE(elf_64.symtab[i].st_info) != STT_FILE) {
            if (elf_64.symtab[i].st_shndx == SHN_UNDEF) {
                // printf("%s\n", name);
            }
            if (elf_64.symtab[i].st_shndx) {
                sym_data[sym_size].adress = formatted_address(elf_64.symtab[i].st_value);
            }
            else
                sym_data[sym_size].adress = ft_strdup("                  ");
            sym_data[sym_size].type = get_final_symbol_type(ELF64_ST_BIND(elf_64.symtab[i].st_info), ELF64_ST_BIND(elf_64.symtab[i].st_info), 
                &elf_64.shstrtab[elf_64.sections_hdr[elf_64.symtab[i].st_shndx].sh_name]);
            sym_data[sym_size].name = ft_strdup(elf_64.name);
            sym_size++;
        }
    }
    insertion_sort(sym_data, sym_size);
    for (int i = 0; i < sym_size; i++) {
        printf("%s ", sym_data[i].adress);
        printf("%c ", sym_data[i].type);
        printf("%s \n", sym_data[i].name);

    }
}

void handle_elf_64(Elf64_Ehdr *file_hdr, u_int8_t *file_data) {
    t_elf_64 elf_64;

    elf_64.sections_hdr = (Elf64_Shdr *) (file_data + file_hdr->e_shoff);
    elf_64.e_shstrndx = file_hdr->e_shstrndx;
    elf_64.shstrtab = get_strtab(file_data, elf_64.sections_hdr[elf_64.e_shstrndx].sh_size, elf_64.sections_hdr[elf_64.e_shstrndx].sh_offset);

    // recupere les adresses des headers symtab et strtab
    for (int i = 0; i < file_hdr->e_shnum; i++) {
        elf_64.name = &elf_64.shstrtab[elf_64.sections_hdr[i].sh_name];
        if (!ft_strncmp(elf_64.name, ".symtab", ft_strlen(".symtab")))
            elf_64.symtab_hdr = &elf_64.sections_hdr[i];
        else if (!ft_strncmp(elf_64.name, ".strtab", ft_strlen(".strtab")))
            elf_64.strtab_hdr = &elf_64.sections_hdr[i];
    }
    // recupere symtab
    elf_64.symtab = (Elf64_Sym *)(file_data + elf_64.symtab_hdr->sh_offset);
    // calcul de l offset entre chaque symbole
    elf_64.symbols_offset = elf_64.symtab_hdr->sh_size / sizeof(Elf64_Sym);
    elf_64.strtab = get_strtab(file_data, elf_64.strtab_hdr->sh_size, elf_64.strtab_hdr->sh_offset);
    t_symbol_data *sym_data = malloc(sizeof(t_symbol_data) * elf_64.symbols_offset);
    if (sym_data == NULL)
        return ;
    fill_symdata(sym_data, elf_64);
    
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

t_flags init_flags(int ac, char **av) {
    t_flags flags;

    flags.a = 0, flags.g = 0, flags.u = 0; flags.r = 0, flags.p = 0;
    for (int i = 1; i < ac - 1; i++) {
        if (!ft_strncmp(av[i], "-a", 2))
            flags.a = 1;
        if (!ft_strncmp(av[i], "-g", 2))
            flags.g = 1;
        if (!ft_strncmp(av[i], "-u", 2))
            flags.u = 1;
        if (!ft_strncmp(av[i], "-r", 2))
            flags.r = 1;
        if (!ft_strncmp(av[i], "-p", 2))
            flags.p = 1;
    }
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
        flags = init_flags(ac, av);
        // printf("a: %d g: %d u: %d r: %d p: %d", flags.a, flags.g, flags.u, flags.r, flags.p);
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