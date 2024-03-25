#include "ft_nm.h"

// int handle_file_errors(int fd, struct stat buf) {
    
//     return 0;
// }

int analyze_file(uint8_t *file_data) {
    
    Elf64_Ehdr *hdr; 
    hdr = (Elf64_Ehdr *) file_data;

    if (hdr->e_ident[EI_MAG0] != ELFMAG0 || hdr->e_ident[EI_MAG1] != ELFMAG1 ||
            hdr->e_ident[EI_MAG2] != ELFMAG2 || hdr->e_ident[EI_MAG3] != ELFMAG3) {
        printf("Not an ELF file!\n");
        return 1;
    } else if (hdr->e_ident[EI_CLASS] == ELFCLASS32) {
        ft_putstr_fd("ELF 32\n", 2);
    } else if (hdr->e_ident[EI_CLASS] == ELFCLASS64) {
        Elf64_Ehdr *elf_header = (Elf64_Shdr *) file_data;
        Elf64_Shdr *section_headers = (Elf64_Shdr *)((char *)elf_header + elf_header->e_shoff);

        Elf64_Shdr *strtab_section = NULL;

        for (int i = 0; i < elf_header->e_shnum; i++) {
            if (section_headers[i].sh_type == SHT_STRTAB) {
                strtab_section = &section_headers[i];
                return ; 
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