#include "ft_nm.h"

void print_symbol(char *address, char type, char *name) {
    ft_putstr_fd(address, 1);
    ft_putchar_fd(' ', 1);
    ft_putchar_fd(type, 1);
    ft_putchar_fd(' ', 1);
    ft_putstr_fd(name, 1);
    ft_putchar_fd('\n', 1);
}

void handle_output(t_symbol_data *sym_data, int sym_size) {
    if (!options.p) {
        if (options.r)
            insertion_sort(sym_data, sym_size, 1);
        else
            insertion_sort(sym_data, sym_size, 0);
    }
    if (options.files_nb > 1) {
        ft_putchar_fd('\n', 1);
        ft_putstr_fd(options.file_name, 1);
        ft_putstr_fd(":\n", 1);
    }
    for (int i = 0; i < sym_size; i++) {
        if (options.u) {
            if (sym_data[i].is_undefined) {
                print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
            }
        }
        else if (options.g) {
            if (!is_upper(sym_data[i].type) || sym_data[i].is_undefined)
                print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
        }
        else {
            print_symbol(sym_data[i].address, sym_data[i].type, sym_data[i].name);
        }
    }
}

void help_output() {
    ft_putstr_fd("Usage: ./ft_nm [option(s)] [file(s)]\n List symbols in [file(s)]\n The options are:\n", 2);
    ft_putstr_fd("  -a,       Display debugger-only symbols\n", 1);
    ft_putstr_fd("  -g,       Display only external symbols\n", 1);
    ft_putstr_fd("  -p,       Do not sort the symbols\n", 1);
    ft_putstr_fd("  -r,       Reverse the sense of the sort\n", 1);
    ft_putstr_fd("  -u,       Display only undefined symbols\n", 1);
}

void print_error(char *file_name ,char *err, char *err_type) {
    ft_putstr_fd("ft_nm: ", 2);
    if (err_type && !ft_strncmp(err_type, "Warning", ft_strlen(err_type)))
        ft_putstr_fd("Warning: ", 2);
    ft_putchar_fd(39, 2);
    ft_putstr_fd(file_name, 2);
    ft_putchar_fd(39, 2);
    ft_putstr_fd(err, 2);
}