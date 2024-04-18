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
    char *address;
    char type;
    char *name;
} t_symbol_data;

typedef struct s_flags{
    int a; // Afficher tous les symboles, même ceux spécifiques au débogueur
    int g; // N'afficher que les symboles globaux.
    int u; // N'afficher que les symboles non définis (ceux externes à chaque fichier objet).
    int r; // Renverser l'ordre de tri (numérique ou alphabétique) ; commencer par le dernier.
    int p; // Ne pas trier les symboles, uniquement les afficher dans leur ordre de rencontre.
} t_flags;

typedef struct s_elf_64 {
    char *name;
    char *strtab;
    char *shstrtab;
    int symbols_offset;
    Elf64_Ehdr *file_hdr; // Stock l'adresse des headers du fichier
    Elf64_Shdr *sections_hdr; // Stock l'adresse des headers des sections
    Elf64_Shdr *symtab_hdr; // Stock l'adresse du header symtab
    Elf64_Shdr *strtab_hdr; // Stock l'adresse du header strtab
    Elf64_Sym *symtab; // Stock l'adresse du debut de la section strtab
    uint16_t e_shstrndx; // Index de la section de noms de section shstrtab
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
