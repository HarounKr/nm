#include "../inc/ft_nm.h"

static int fill_symdata(t_symbol_data *sym_data, t_elf_32 elf_32) {

    char *name;
    int sym_size = 0;

    for (int i = 0; i < elf_32.symbols_offset; i++) {
        unsigned int type = ELF32_ST_TYPE(elf_32.symtab[i].st_info);
        unsigned int bind = ELF32_ST_BIND(elf_32.symtab[i].st_info);
        uint32_t size = convert_endian32(elf_32.symtab[i].st_size, elf_32.is_bigendian);
        uint32_t st_name = convert_endian32(elf_32.symtab[i].st_name, elf_32.is_bigendian);
    
        name = &elf_32.strtab[st_name];
        if (ft_strlen(name) != STT_NOTYPE && type != STT_FILE) {
            uint16_t st_shndx = convert_endian16(elf_32.symtab[i].st_shndx, elf_32.is_bigendian);
            uint32_t st_value = convert_endian32(elf_32.symtab[i].st_value, elf_32.is_bigendian);

            if (st_shndx)
                sym_data[sym_size].address = formatted_address(st_value, 7);
            else {
                sym_data[sym_size].address = ft_strdup("        ");
                sym_data[sym_size].is_undefined = 1;
            }
            if (st_shndx == SHN_ABS && bind == STB_LOCAL) {
                sym_data[sym_size].type = 'a';
            } else if (st_shndx == SHN_ABS && bind == STB_GLOBAL) {
                sym_data[sym_size].type = 'A';
            }
            else {
                uint32_t sh_name = convert_endian32(elf_32.sections_hdr[st_shndx].sh_name, elf_32.is_bigendian);
                sym_data[sym_size].type = get_final_symbol_type(type, bind, size, &elf_32.shstrtab[sh_name]);
            }
            sym_data[sym_size].name = ft_strdup(name);
            sym_size++;
        }
    }
    return sym_size;
}

int handle_elf_32(u_int8_t *file_data, t_elf_32 elf_32) {

    char *name;
    bool is_symtab = false;
    bool is_strtab = false;
    int sym_size = 0;
    Elf32_Shdr *symtab_hdr; // Stock l'adresse du header symtab
    Elf32_Shdr *strtab_hdr; // Stock l'adresse du header strtab

    elf_32.sections_hdr = (Elf32_Shdr *) (file_data + elf_32.e_shoff);
    uint32_t shstrtab_size = convert_endian32(elf_32.sections_hdr[elf_32.e_shstrndx].sh_size, elf_32.is_bigendian);
    uint32_t shstrtab_offset = convert_endian32(elf_32.sections_hdr[elf_32.e_shstrndx].sh_offset, elf_32.is_bigendian);
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
    print_filename();
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
    if ((sym_size = fill_symdata(sym_data, elf_32))) {
        handle_output(sym_data, sym_size);
        for (int i = 0; i < elf_32.symbols_offset; i++) {
            free(sym_data[i].address);
            sym_data[i].address = NULL;
            free(sym_data[i].name);
            sym_data[i].name = NULL;
            sym_data[i].type = 0;
            sym_data[i].is_undefined = false;
        }
        free(sym_data);
        sym_data = NULL;
    }
    free(elf_32.strtab);
    free(elf_32.shstrtab);
    return 0;
}