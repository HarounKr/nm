#include "ft_nm.h"

t_options options;
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

int define_elf_type(uint8_t *file_data) {
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

int parse_args(int ac, char **av) {
    options.files_name = malloc(sizeof(char *) * ac);
    options.files_size = 0;
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
            options.files_name[options.files_size] = ft_strdup(av[i]);
            options.files_size++;
        }
    }
    if (options.p && options.r)
        options.r = false;
    if (options.u && options.g)
        options.g = false;
    // for (int i = 0; i < f_index; i++) {
    //     printf("%s\n", options.files_name[i]);
    // }
    // printf("a: %d g: %d u: %d r: %d p: %d", options.a, options.g, options.u, options.r, options.p);
    // printf("\n");
    return 0;
}

int main(int ac, char **av) {
    int ret_error = 0;

    if (ac >= 2) {
        struct stat buf;
        uint8_t *file_data;
        if (parse_args(ac, av)) {
            help_output();
            return 1;
        }
        printf("%d\n", options.files_size);
        for (int i = 0; i < options.files_size; i++) {
            int fd = open(options.files_name[i], O_RDONLY, S_IRUSR);
            if (fd == -1) {
               print_error(options.files_name[i], "No such file\n\n");
               ret_error = 1;
            }
            else if (fstat(fd, &buf) != 0) {
                print_error(options.files_name[i], "couldn't get file size\n\n");
                close(fd);
                ret_error = 1;
            }
            else if (S_ISDIR(buf.st_mode)) {
                print_error(options.files_name[i],"is a directory\n\n");
                close(fd);
                ret_error = 1;
            }
            else {
                file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (file_data == MAP_FAILED) {
                    print_error(options.files_name[i],"mapped memory failed\n\n");
                    close(fd);
                    ret_error = 1;
                }
                define_elf_type(file_data);
                munmap(file_data, buf.st_size);
                close(fd);
            }
        }
    } 
    else {
        help_output();
        ret_error = 1;   
    }
    return ret_error;
}