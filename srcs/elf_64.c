#include "../inc/ft_nm.h"

static int fill_symdata(t_symbol_data *sym_data, t_elf_64 elf_64) {
    char *name;
    int sym_size = 0;
    // printf("symbols_offset : %d\n", elf_64.symbols_offset);
    for (int i = 0; i < elf_64.symbols_offset; i++) {
        unsigned int type = ELF64_ST_TYPE(elf_64.symtab[i].st_info);
        // printf("st_type  : %-10d |\n", type);
        unsigned int bind = ELF64_ST_BIND(elf_64.symtab[i].st_info);
        // printf("st_bind  : %-10d |\n", bind);
        uint64_t size = convert_endian64(elf_64.symtab[i].st_size, elf_64.is_bigendian);
        // printf("st_size  : %-10ld |\n", size);
        uint64_t st_name = convert_endian64(elf_64.symtab[i].st_name, elf_64.is_bigendian);
        // printf("st_name  : %-10ld |\n", st_name);
      
        name = &elf_64.strtab[st_name];
        if (ft_strlen(name) != STT_NOTYPE && type != STT_FILE) {
            // printf("Name: %-10s | ", name);
            uint16_t st_shndx = convert_endian16(elf_64.symtab[i].st_shndx, elf_64.is_bigendian);
            // printf("st_shndx : %-10d |\n", st_shndx);
            uint64_t st_value = convert_endian64(elf_64.symtab[i].st_value, elf_64.is_bigendian);
            // printf("st_value : %-10ld |\n", st_value);
            if (st_shndx)
                sym_data[sym_size].address = formatted_address(st_value, 15);
            else {
                sym_data[sym_size].address = ft_strdup("                ");
                sym_data[sym_size].is_undefined = 1;
            }
            uint64_t sh_name = convert_endian64(elf_64.sections_hdr[st_shndx].sh_name, elf_64.is_bigendian);
            sym_data[sym_size].type = get_final_symbol_type(type, bind, size, &elf_64.shstrtab[sh_name]);
            // printf("sym type : %-10c |\n", sym_data[sym_size].type);
            if (!is_upper(sym_data[i].type))
                sym_data[i].is_external = true;
            sym_data[sym_size].name = ft_strdup(name);
            sym_size++;
        }
    }
    return sym_size;
}

int handle_elf_64(Elf64_Ehdr *file_hdr, u_int8_t *file_data, t_elf_64 elf_64) {
    char *name;
    int sym_size = 0;
    bool is_symtab = false;
    bool is_strtab = false;
    Elf64_Shdr *symtab_hdr; // Stock l'adresse du header symtab
    Elf64_Shdr *strtab_hdr; // Stock l'adresse du header strtab
    (void)file_hdr;

    elf_64.sections_hdr = (Elf64_Shdr *) (file_data + elf_64.e_shoff);
    uint64_t shstrtab_size = convert_endian64(elf_64.sections_hdr[elf_64.e_shstrndx].sh_size, elf_64.is_bigendian);
    uint64_t shstrtab_offset = convert_endian64(elf_64.sections_hdr[elf_64.e_shstrndx].sh_offset, elf_64.is_bigendian);
    // printf(" shstrtab_offset %ld\n", shstrtab_offset);
    // printf(" shstrtab_size %ld\n", shstrtab_size);

    elf_64.shstrtab = get_strtab(file_data, shstrtab_size, shstrtab_offset);
    // recupere les addresses des headers symtab et strtab
    for (int i = 0; i < elf_64.e_shnum; i++) {
        uint64_t sh_name = convert_endian32(elf_64.sections_hdr[i].sh_name, elf_64.is_bigendian); // Index name
        name = &elf_64.shstrtab[sh_name];
        if (!ft_strncmp(name, ".symtab", ft_strlen(".symtab"))) {
            is_symtab = true;
            symtab_hdr = &elf_64.sections_hdr[i];
        }
        else if (!ft_strncmp(name, ".strtab", ft_strlen(".strtab"))) {
            is_strtab = true;
            strtab_hdr = &elf_64.sections_hdr[i];
        }
    }
    print_filename();
    if (!is_symtab || !is_strtab) {
        free(elf_64.shstrtab);
        return print_error(options.file_name, ": no symbols\n", NULL, false);
    }
    // recupere la section symtab
    uint64_t sh_offset = convert_endian64(symtab_hdr->sh_offset, elf_64.is_bigendian);
    uint64_t symtab_size = convert_endian64(symtab_hdr->sh_size, elf_64.is_bigendian);
    uint64_t strtab_size = convert_endian64(strtab_hdr->sh_size, elf_64.is_bigendian);
    uint64_t strtab_offset = convert_endian64(strtab_hdr->sh_offset, elf_64.is_bigendian);
    elf_64.symtab = (Elf64_Sym *)(file_data + sh_offset);
    // calcul de l offset entre chaque symbole
    elf_64.symbols_offset = symtab_size / sizeof(Elf64_Sym);

    elf_64.strtab = get_strtab(file_data, strtab_size, strtab_offset);
    t_symbol_data *sym_data = ft_calloc(elf_64.symbols_offset, sizeof(t_symbol_data));
    if (sym_data == NULL)
        return 1;
    sym_data_init(sym_data, elf_64.symbols_offset);
    if ((sym_size = fill_symdata(sym_data, elf_64))) {
        handle_output(sym_data, sym_size);
        for (int i = 0; i < elf_64.symbols_offset; i++) {
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
    free(elf_64.strtab);
    free(elf_64.shstrtab);
    
    return 0;
}