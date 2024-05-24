#include "ft_nm.h"

static void fill_symdata(t_symbol_data *sym_data, t_elf_32 elf_32) {
    char *name;
    int sym_size = 0;
    (void)name;

    printf("symbols_offset : %d\n", elf_32.symbols_offset);
    for (int i = 0; i < elf_32.symbols_offset; i++) {
        unsigned int type = ELF32_ST_TYPE(elf_32.symtab[i].st_info);
        unsigned int bind = ELF32_ST_BIND(elf_32.symtab[i].st_info);
        uint32_t size = convert_endian32(elf_32.symtab[i].st_size, elf_32.is_bigendian);
        uint32_t st_name = convert_endian32(elf_32.symtab[i].st_name, elf_32.is_bigendian);
        uint16_t st_shndx = convert_endian16(elf_32.symtab[i].st_shndx, elf_32.is_bigendian);
        uint32_t st_value = convert_endian32(elf_32.symtab[i].st_value, elf_32.is_bigendian);
        uint32_t sh_name = convert_endian32(elf_32.sections_hdr[st_shndx].sh_name, elf_32.is_bigendian);
        (void) st_name;
        printf("st_type  : %-10d |\n", type);
        printf("st_bind  : %-10d |\n", bind);
        printf("st_size  : %-10d |\n", size);
        printf("sh_name  : %-10d |\n", sh_name);
        printf("st_shndx : %-10d |\n", st_shndx);
        printf("st_value : %-10d |\n", st_value);

        name = &elf_32.strtab[st_name];
        if (ft_strlen(name) != STT_NOTYPE && type != STT_FILE) {
            if (st_shndx)
                sym_data[sym_size].address = formatted_address(st_value);
            else {
                sym_data[sym_size].address = ft_strdup("                ");
                sym_data[sym_size].is_undefined = 1;
            }

            sym_data[sym_size].type = get_final_symbol_type(type, bind, size, &elf_32.shstrtab[sh_name]);
            // printf("sym type : %-10c |\n", sym_data[sym_size].type);
            if (!is_upper(sym_data[i].type))
                sym_data[i].is_external = true;
            sym_data[sym_size].name = ft_strdup(name);
            sym_size++;
        }
    }
    handle_output(sym_data, sym_size);
    for (int i = 0; i < elf_32.symbols_offset; i++) {
        free(sym_data[i].address);
        sym_data[i].address = NULL;
        free(sym_data[i].name);
        sym_data[i].name = NULL;
        sym_data[i].type = 0;
        sym_data[i].is_external = false;
        sym_data[i].is_undefined = false;
    }
    free(sym_data);
    sym_data = NULL;
}

int handle_elf_32(Elf32_Ehdr *file_hdr, u_int8_t *file_data, t_elf_32 elf_32) {
    char *name;
    bool is_symtab = false;
    bool is_strtab = false;
    Elf32_Shdr *symtab_hdr; // Stock l'adresse du header symtab
    Elf32_Shdr *strtab_hdr; // Stock l'adresse du header strtab
    (void)file_hdr;

    elf_32.sections_hdr = (Elf32_Shdr *) (file_data + elf_32.e_shoff);
    uint32_t shstrtab_size = convert_endian32(elf_32.sections_hdr[elf_32.e_shstrndx].sh_size, elf_32.is_bigendian);
    uint32_t shstrtab_offset = convert_endian32(elf_32.sections_hdr[elf_32.e_shstrndx].sh_offset, elf_32.is_bigendian);
    printf(" shstrtab_offset %d\n", shstrtab_offset);
    printf(" shstrtab_size %d\n", shstrtab_size);

    elf_32.shstrtab = get_strtab(file_data, shstrtab_size, shstrtab_offset);
    // recupere les addresses des headers symtab et strtab
    for (int i = 0; i < elf_32.e_shnum; i++) {
        uint32_t sh_name = convert_endian32(elf_32.sections_hdr[i].sh_name, elf_32.is_bigendian); // Index name
        name = &elf_32.shstrtab[sh_name];
        if (!ft_strncmp(name, ".symtab", ft_strlen(".symtab"))) {
            is_symtab = true;
            symtab_hdr = &elf_32.sections_hdr[i];
        }
        else if (!ft_strncmp(name, ".strtab", ft_strlen(".strtab"))) {
            is_strtab = true;
            strtab_hdr = &elf_32.sections_hdr[i];
        }
    }
    if (!is_symtab || !is_strtab) {
        free(elf_32.shstrtab);
        return print_error(options.file_name, ": no symbols\n", NULL, false);
    }
    // recupere la section symtab
    uint32_t sh_offset = convert_endian32(symtab_hdr->sh_offset, elf_32.is_bigendian);
    uint32_t symtab_size = convert_endian32(symtab_hdr->sh_size, elf_32.is_bigendian);
    uint32_t strtab_size = convert_endian32(strtab_hdr->sh_size, elf_32.is_bigendian);
    uint32_t strtab_offset = convert_endian32(strtab_hdr->sh_offset, elf_32.is_bigendian);
    elf_32.symtab = (Elf32_Sym *)(file_data + sh_offset);
    // calcul de l offset entre chaque symbole
    elf_32.symbols_offset = symtab_size / sizeof(Elf32_Sym);
    elf_32.strtab = get_strtab(file_data, strtab_size, strtab_offset);
    t_symbol_data *sym_data = ft_calloc(elf_32.symbols_offset, sizeof(t_symbol_data));
    if (sym_data == NULL)
        return 1;
    sym_data_init(sym_data, elf_32.symbols_offset);
    fill_symdata(sym_data, elf_32);
    free(elf_32.strtab);
    free(elf_32.shstrtab);
    
    return 0;
}


// printf("st_info  : %-10s | ", get_elf_symbol_type(ELF32_ST_TYPE(elf_32.symtab[i].st_info)));
        // printf("Type     : %-10c |", get_final_symbol_type(ELF32_ST_BIND(elf_32.symtab[i].st_info), ELF32_ST_BIND(elf_32.symtab[i].st_info), &elf_32.shstrtab[elf_32.sections_hdr[elf_32.symtab[i].st_shndx].sh_name]));
        // printf("st_bind  : %-10d |", ELF32_ST_BIND(elf_32.symtab[i].st_info)); 
        // printf("section  : %-10s | ", &elf_32.shstrtab[elf_32.sections_hdr[elf_32.symtab[i].st_shndx].sh_name]);


 // &elf_32.shstrtab[elf_32.sections_hdr[elf_32.symtab[i].st_shndx].sh_name] ==> trouve la section dans laquelle se trouve le symbole
            // st_shndx contient l'index de la section dans laquelle se trouve le symbole dans sections_hdr, une fois la section trouver, on peut avoir son nom
            // dans shstrtab grace a sh_name qui est l index du nom de la table des sections
            // printf("sym Name : %-40s |", elf_32.name);
            // printf("st_shndx : %-10d |", elf_32.symtab[i].st_shndx);
            // printf("st_size : %-10d |", size);