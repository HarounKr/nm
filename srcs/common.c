#include "../inc/ft_nm.h"

char *formatted_address(uint64_t address, int index) {
    int i = index;
    char *formatted_address = ft_calloc(index + 2, sizeof(char)); // "0000000000004010" + '\0'

    if (!formatted_address)
        return NULL;
    for (int i = 0; i < index + 1; i++) {
        formatted_address[i] = '0';
    }
    formatted_address[index + 1] = '\0';
    while (address > 0 && i >= 0) {
        int digit = address % 16;
        formatted_address[i] = digit < 10 ? '0' + digit : 'a' + digit - 10;
        address /= 16;
        --i;
    }
    return formatted_address;
}

char *get_strtab(uint8_t *file_data, uint64_t strtab_size, Elf64_Off strtab_offset) {
    char *strtab;

    strtab = ft_calloc(strtab_size, sizeof(char *));
    ft_memcpy(strtab, (char *)(file_data + strtab_offset), strtab_size);

    return strtab;
}

int is_section(char *section_name, char **sections) {
    for (int i = 0; sections[i] != NULL; i++) {
        if (!ft_strcmp(section_name, sections[i]))
            return 0;
    }
    return 1;
}

char get_final_symbol_type(unsigned int type, unsigned int bind, unsigned int size, char *section_name) {
    if (bind == STB_WEAK) {
        if (!ft_strlen(section_name))
            return 'w';
        else if (is_section(section_name, weak_sections) || size == 0)
            return 'W';
        else
            return 'V';
    } else if (bind == STB_GLOBAL && !ft_strlen(section_name))
        return 'U';
    else {
        if (!is_section(section_name, text_sections)) {
            if (bind == STB_LOCAL)
                return 't';
            else if (bind == STB_GLOBAL)
                return 'T';
        } else if (!ft_strcmp(section_name, ".bss")) {
            if (bind == STB_LOCAL)
                return 'b';
            else if (bind == STB_GLOBAL)
                return 'B'; 
        } else if (type == STT_COMMON && bind == STB_GLOBAL)
            return 'C';
        else if (!is_section(section_name, data_sections)) {
            if (bind == STB_LOCAL)
                return 'd';
            else if (bind == STB_GLOBAL)
                return 'D';
        }
        else if (!is_section(section_name, ro_sections)) {
            if (bind == STB_LOCAL)
                return 'r';
            else
                return 'R';            
        } 
    }
    return '?';
}