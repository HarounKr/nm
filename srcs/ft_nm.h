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
#include <stdbool.h>
#include <string.h>

typedef struct s_symbol_data {
    char *address;
    char type;
    char *name;
    bool   is_undefined;
    bool   is_external;  
} t_symbol_data;

typedef struct s_options{
    char **files_name;
    char *file_name;
    int  files_nb;
    bool a; // Afficher tous les symboles, même ceux spécifiques au débogueur
    bool g; // N'afficher que les symboles externes (globaux + indefinis).
    bool u; // N'afficher que les symboles non définis (ceux externes à chaque fichier objet).
    bool r; // Renverser l'ordre de tri (numérique ou alphabétique) ; commencer par le dernier.
    bool p; // Ne pas trier les symboles, uniquement les afficher dans leur ordre de rencontre.
} t_options;

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

extern t_options options;
extern char *text_sections[5];
extern char *data_sections[5];
extern char *ro_sections[5];

int     handle_file_errors(int fd, struct stat buf);
void    insertion_sort(t_symbol_data *array, int n, int reverse);
void    handle_output(t_symbol_data *sym_data, int sym_size);
void    handle_elf_64(Elf64_Ehdr *file_hdr, u_int8_t *file_data);
char    *formatted_address(uint64_t address);
char    get_final_symbol_type(unsigned int type, unsigned int bind, char *section_name);
char    *get_strtab(uint8_t *file_data, uint64_t strtab_size, Elf64_Off strtab_offset);
int     is_upper(char c);
void    sym_data_init(t_symbol_data *sym_data, int size);
void    help_output();
void    print_error(char *file_name ,char *err);