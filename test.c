int small_data_1;
int small_data_2 = 2;
// Essayez avec différents types et tailles de données
char small_array[4] = {0, 1, 2, 3};
int large_data[1024] = {0}; // Exemple de plus grande donnée

int main() {
    return small_data_1 + small_data_2 + small_array[0] + large_data[0];
}


typedef union {
    Elf32_Ehdr* elf32;
    Elf64_Ehdr* elf64;
} Elf_Ehdr_Ptr;

typedef struct {
    int is_64;       // 0 pour ELF32, non-0 pour ELF64
    Elf_Ehdr_Ptr hdr;
} Generic_Elf_Header;


// Fonction pour initialiser l'en-tête générique
Generic_Elf_Header init_generic_elf_header(void* ehdr, int is_64) {
    Generic_Elf_Header ghdr;
    ghdr.is_64 = is_64;
    if (is_64) {
        ghdr.hdr.elf64 = (Elf64_Ehdr*)ehdr;
    } else {
        ghdr.hdr.elf32 = (Elf32_Ehdr*)ehdr;
    }
    return ghdr;
}

// Fonction exemple pour afficher l'adresse de départ du programme
void print_entry_point(Generic_Elf_Header ghdr) {
    if (ghdr.is_64) {
        printf("Entry point: %p\n", (void*)ghdr.hdr.elf64->e_entry);
    } else {
        printf("Entry point: %p\n", (void*)(uintptr_t)ghdr.hdr.elf32->e_entry);  // Cast nécessaire pour la taille
    }
}


#ifdef ELF64
    typedef Elf64_Ehdr Generic_Ehdr;
    typedef Elf64_Shdr Generic_Shdr;
    typedef Elf64_Sym Generic_Sym;
    #define GET_SECTIONS_HDR(file_data, file_hdr) (Generic_Shdr *)((char *)(file_data) + (file_hdr)->e_shoff)
    #define GET_SYM(file_data, sym_hdr) (Generic_Sym *)((char *)(file_data) + (sym_hdr)->sh_offset)
#else
    typedef Elf32_Ehdr Generic_Ehdr;
    typedef Elf32_Shdr Generic_Shdr;
    typedef Elf32_Sym Generic_Sym;
    #define GET_SECTIONS_HDR(file_data, file_hdr) (Generic_Shdr *)((char *)(file_data) + (file_hdr)->e_shoff)
    #define GET_SYM(file_data, sym_hdr) (Generic_Sym *)((char *)(file_data) + (sym_hdr)->sh_offset)
#endif

inline Generic_Shdr *get_sections_hdr(void *file_data, Generic_Ehdr *file_hdr) {
    return (Generic_Shdr *)((char *)file_data + file_hdr->e_shoff);
}

inline Generic_Sym *get_sym(void *file_data, Generic_Shdr *sym_hdr) {
    return (Generic_Sym *)((char *)file_data + sym_hdr->sh_offset);
}
