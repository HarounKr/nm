#include "ft_nm.h"

// int handle_file_errors(int fd, struct stat buf) {
    
//     return 0;
// }

char *get_strtab(uint8_t *file_data, uint64_t strtab_size, Elf64_Off strtab_offset) {
    char *strtab;

    strtab = (char *) malloc(strtab_size);
    ft_memcpy(strtab, (char *)(file_data + strtab_offset), strtab_size);

    return strtab;
}

int analyze_file(uint8_t *file_data) {
    
    Elf64_Ehdr *file_hdr;
    Elf64_Shdr *sections_hdr;
    Elf64_Shdr *symtab_hdr;
    Elf64_Shdr *strtab_hdr;
    uint16_t e_shstrndx;
    int symbols_offset; 
    char *strtab;
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
        strtab = get_strtab(file_data, sections_hdr[e_shstrndx].sh_size, sections_hdr[e_shstrndx].sh_offset);
        
        for (int i = 0; i < file_hdr->e_shnum; i++) {
            name = &strtab[sections_hdr[i].sh_name];
            if (!ft_strncmp(name, ".symtab", ft_strlen(".symtab")))
                symtab_hdr = &sections_hdr[i];
            else if (!ft_strncmp(name, ".strtab", ft_strlen(".strtab")))
                strtab_hdr = &sections_hdr[i];
        }
        Elf64_Sym *symtab = (Elf64_Sym *)(file_data + symtab_hdr->sh_offset);
        symbols_offset = symtab_hdr->sh_size / sizeof(Elf64_Sym);
        free(strtab);
        strtab = get_strtab(file_data, strtab_hdr->sh_size, strtab_hdr->sh_offset);
        for (int i = 0; i < symbols_offset; i++) {
            // unsigned int bind = ELF64_ST_BIND(symtab[i].st_info);
            unsigned int type = ELF64_ST_TYPE(symtab[i].st_info);
            printf("bind : %d | type : %d\n", symtab[i].st_info, type);
            name = &strtab[symtab[i].st_name];
            if (!ft_strlen(name) == STT_NOTYPE && symtab[i].st_info != STT_FILE) {
                printf("%d | ", symtab[i].st_info);
                printf("%s\n", name);
            }
        }
        // printf("symtab : %ld\n", symtab_hdr->sh_size / 24);
        // printf("symtab : %d\n", symtab_hdr->sh_info);
        // printf("strtab size : %ld\n", strtab_hdr->sh_size);
        // printf("strtab : %s\n", &strtab[12352]);
    }
    return 0;
}

int main(int ac, char **av) {
    if (ac == 2) {
        struct stat buf;
        uint8_t *file_data;

        int fd = open(av[1], O_RDONLY, S_IRUSR);
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