#include "ft_nm.h"

// int handle_file_errors(int fd, struct stat buf) {
    
//     return 0;
// }

char *get_shstrtab(uint8_t *file_data, uint64_t shstrtab_size, Elf64_Off shstrtab_offset) {
    char *shstrtab;

    shstrtab = (char *) malloc(shstrtab_size);
    ft_memcpy(shstrtab, (char *)(file_data + shstrtab_offset), shstrtab_size);

    return shstrtab;
}

int analyze_file(uint8_t *file_data) {
    
    Elf64_Ehdr *file_hdr;
    Elf64_Shdr *sections_hdr;
    Elf64_Shdr *symtab_hdr;
    Elf64_Shdr *strtab_hdr;
    uint16_t e_shstrndx;
    char *shstrtab;
    char *sh_name;
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
        shstrtab = get_shstrtab(file_data, sections_hdr[e_shstrndx].sh_size, sections_hdr[e_shstrndx].sh_offset);
        
        for (int i = 0; i < file_hdr->e_shnum; i++) {
            sh_name = &shstrtab[sections_hdr[i].sh_name];
            if (!ft_strncmp(sh_name, ".symtab", ft_strlen(".symtab"))) {
                printf("%ld\n", sections_hdr[i].sh_offset);
                symtab_hdr = &sections_hdr[i];
            }
            else if (!ft_strncmp(sh_name, ".strtab", ft_strlen(".strtab"))) {
                printf("%ld\n", sections_hdr[i].sh_offset);
                strtab_hdr = &sections_hdr[i];
            }
            else {
                printf("No symtab\n");
            }
        }
        printf("symtab : %ld\n", symtab_hdr->sh_size);
        printf("strtab : %ld\n", strtab_hdr->sh_size);
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