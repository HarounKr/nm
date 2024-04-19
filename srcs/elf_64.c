#include "ft_nm.h"

void fill_symdata(t_symbol_data *sym_data, t_elf_64 elf_64) {
    int sym_size = 0;
    for (int i = 0; i < elf_64.symbols_offset; i++) {
        unsigned int type = ELF64_ST_BIND(elf_64.symtab[i].st_info);
        unsigned int bind = ELF64_ST_BIND(elf_64.symtab[i].st_info);
        elf_64.name = &elf_64.strtab[elf_64.symtab[i].st_name];
        if (ft_strlen(elf_64.name) != STT_NOTYPE && ELF64_ST_TYPE(elf_64.symtab[i].st_info) != STT_FILE) {
            if (elf_64.symtab[i].st_shndx == SHN_UNDEF && flags.u) {
                // printf("%s\n", elf_64.name);
            }
            if (elf_64.symtab[i].st_shndx)
                sym_data[sym_size].address = formatted_address(elf_64.symtab[i].st_value);
            else {
                sym_data[sym_size].address = ft_strdup("                  ");
                sym_data[sym_size].is_undefined = 1;
            }
            // &elf_64.shstrtab[elf_64.sections_hdr[elf_64.symtab[i].st_shndx].sh_name] ==> trouve la section dans laquelle se trouve le symbole
            // st_shndx contient l'index de la section dans laquelle se trouve le symbole dans sections_hdr, une fois la section trouver, on peut avoir son nom
            // dans shstrtab grace a sh_name qui est l index du nom de la table des sections
            sym_data[sym_size].type = get_final_symbol_type(type, bind, &elf_64.shstrtab[elf_64.sections_hdr[elf_64.symtab[i].st_shndx].sh_name]);

            sym_data[sym_size].name = ft_strdup(elf_64.name);
            sym_size++;
        }
    }
    handle_flags(sym_data, sym_size);
}


void handle_elf_64(Elf64_Ehdr *file_hdr, u_int8_t *file_data) {
    t_elf_64 elf_64;

    elf_64.sections_hdr = (Elf64_Shdr *) (file_data + file_hdr->e_shoff);
    elf_64.e_shstrndx = file_hdr->e_shstrndx;
    elf_64.shstrtab = get_strtab(file_data, elf_64.sections_hdr[elf_64.e_shstrndx].sh_size, elf_64.sections_hdr[elf_64.e_shstrndx].sh_offset);

    // recupere les addresses des headers symtab et strtab
    for (int i = 0; i < file_hdr->e_shnum; i++) {
        elf_64.name = &elf_64.shstrtab[elf_64.sections_hdr[i].sh_name];
        if (!ft_strncmp(elf_64.name, ".symtab", ft_strlen(".symtab")))
            elf_64.symtab_hdr = &elf_64.sections_hdr[i];
        else if (!ft_strncmp(elf_64.name, ".strtab", ft_strlen(".strtab")))
            elf_64.strtab_hdr = &elf_64.sections_hdr[i];
    }
    // recupere la section symtab
    elf_64.symtab = (Elf64_Sym *)(file_data + elf_64.symtab_hdr->sh_offset);
    // calcul de l offset entre chaque symbole
    elf_64.symbols_offset = elf_64.symtab_hdr->sh_size / sizeof(Elf64_Sym);
    elf_64.strtab = get_strtab(file_data, elf_64.strtab_hdr->sh_size, elf_64.strtab_hdr->sh_offset);
    t_symbol_data *sym_data = malloc(sizeof(t_symbol_data) * elf_64.symbols_offset);
    if (sym_data == NULL)
        return ;
    sym_data_init(sym_data, elf_64.symbols_offset);
    fill_symdata(sym_data, elf_64);
}