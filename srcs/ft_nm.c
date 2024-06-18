#include "ft_nm.h"

t_options options;
t_elf_64 elf_64;
t_elf_32 elf_32;

char *text_sections[4]  = { ".text", ".init", ".fini", NULL };
char *data_sections[6] = {  ".data", ".fini_array", ".init_array", ".dynamic", ".got", NULL,};
char *ro_sections[5] = { ".rodata", ".eh_frame",  ".eh_frame_hdr",".note.ABI-tag", NULL,};
char *weak_sections[4] = { ".rodata", ".data", ".bss", NULL };

char *formatted_address(uint64_t address, int index) {
    int i = index;
    char *formatted_address = ft_calloc(index + 2, sizeof(char)); // "0000000000004010" + '\0'

    if (!formatted_address)
        return NULL;
    for (int i = 0; i < index + 1; i++) {
        formatted_address[i] = '0';
    }
    formatted_address[index + 1] = '\0';
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

int define_elf_type(uint8_t *file_data, char *filename, long int st_size) {
    
    Elf64_Ehdr *file_hdr;
    file_hdr = (Elf64_Ehdr *) file_data;

    if (file_hdr->e_ident[EI_MAG0] != ELFMAG0 || file_hdr->e_ident[EI_MAG1] != ELFMAG1 ||
            file_hdr->e_ident[EI_MAG2] != ELFMAG2 || file_hdr->e_ident[EI_MAG3] != ELFMAG3)
        return print_error(filename, ": file format not recognized 1\n", NULL, false);
    else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        // printf("ca va dans le 32\n");
        if (handle_elf_errors(file_hdr, file_data, filename, st_size))
            return 1;
        Elf32_Ehdr *file_hdr_32 = (Elf32_Ehdr *) file_data;
        return handle_elf_32(file_hdr_32, file_data, elf_32);
    } 
    else if (file_hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        // printf("ca va dans le 64\n");
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

int handle_archive_file(uint8_t *file_data, long st_size) {
    int offset = 8;

    while (offset < st_size) {
        s_archive_file_hdr *header = (s_archive_file_hdr *)(file_data + offset);
        
        options.file_name = ft_strdup(header->file_identifier);
        char *slash_pos = ft_strchr(options.file_name, '/');
        if (slash_pos) 
            *slash_pos = '\0';
        int file_size = ft_atoi(header->file_size);
        if (ft_strlen(options.file_name)) {
            uint8_t *file_contents = (uint8_t *)(header + 1);
            define_elf_type(file_contents, options.file_name, file_size);
        }
        offset += sizeof(s_archive_file_hdr) + file_size;
    }
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
            // printf("NAME : %s\n", av[i + 1]);
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
                // printf("file size : %ld\n", buf.st_size);
                file_data = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
                if (file_data == MAP_FAILED) {
                    close(fd);
                    return print_error(options.files_name[i],"mapped memory failed\n", NULL, true);
                }
                if (ft_strncmp((char *) file_data, "!<arch>\n", 8) == 0)
                    handle_archive_file(file_data, buf.st_size);    
                else {
                    options.file_name = ft_strdup(options.files_name[i]);
                    define_elf_type(file_data, options.files_name[i], buf.st_size);
                }
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