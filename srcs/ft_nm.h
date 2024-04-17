#pragma once

#include "../libft/libft.h"
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <elf.h>

typedef struct s_symbol_data {
    char *adress;
    char type;
    char *name;
} t_symbol_data;

typedef struct s_flags{
    int a;
    int g;
    int u;
    int r;
    int p;
} t_flags;

typedef struct s_elf_64 {
    char *name;
    char *strtab;
    char *shstrtab;
    int symbols_offset;
    Elf64_Ehdr *file_hdr;
    Elf64_Shdr *sections_hdr;
    Elf64_Shdr *symtab_hdr;
    Elf64_Shdr *strtab_hdr;
    Elf64_Sym *symtab;
    uint16_t e_shstrndx;
} t_elf_64;

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

int handle_file_errors(int fd, struct stat buf);
