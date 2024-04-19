#include "ft_nm.h"

void print_symbol(char *address, char type, char *name) {
    ft_putstr_fd(ft_strjoin(address, " "), 1);
    ft_putchar_fd(type, 1);
    ft_putstr_fd(ft_strjoin(" ", name), 1);
    ft_putchar_fd('\n', 1);
}

void handle_flags(t_symbol_data *sym_data, int sym_size) {
    if (!flags.p) {
        if (flags.r)
            insertion_sort(sym_data, sym_size, 1);
        else
            insertion_sort(sym_data, sym_size, 0);
    }
    for (int i = 0; i < sym_size; i++) {
        if (flags.u) {
            if (sym_data[i].is_undefined) {
                print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
            }
        } 
        else if (flags.g) {
            if (!is_upper(sym_data[i].type) || sym_data[i].is_undefined)
                print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
        }
        else {
            print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
        }
    }
}

void sym_data_init(t_symbol_data *sym_data, int size) {
    for (int i = 0; i < size; i++) {
        sym_data[i].address = NULL;
        sym_data[i].type = 0;
        sym_data[i].name = NULL;
        sym_data[i].is_undefined = false;
        sym_data[i].is_global = false;
    }
}