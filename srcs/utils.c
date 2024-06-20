#include "../inc/ft_nm.h"

void sym_data_init(t_symbol_data *sym_data, int size) {
    for (int i = 0; i < size; i++) {
        sym_data[i].address = NULL;
        sym_data[i].type = 0;
        sym_data[i].name = NULL;
        sym_data[i].is_undefined = false;
        sym_data[i].is_external = false;
    }
}

int compare_strings(const char *a, const char *b) {
    int i = 0, j = 0;
    int underscore_count_a = 0, underscore_count_b = 0;

    // Compter les underscores au début de chaque chaîne
    while (a[i] == '_') {
        underscore_count_a++;
        i++;
    }
    while (b[j] == '_') {
        underscore_count_b++;
        j++;
    }
    // Aller au premier caractère alphabétique
    while (a[i] && !ft_isalpha(a[i])) 
        i++;
    while (b[j] && !ft_isalpha(b[j]))
        j++;

    // Comparaison alphabétique des chaînes en ignorant les caractères non alphabétiques
    while (a[i] && b[j]) {
        if (ft_tolower(a[i]) != ft_tolower(b[j]))
            return ft_tolower(a[i]) - ft_tolower(b[j]);

        i++;
        j++;
        while (a[i] && !ft_isalpha(a[i])) 
            i++;
        while (b[j] && !ft_isalpha(b[j]))
            j++;
    }

    // Si les chaînes sont identiques, comparer les derniers caractères
    if (a[i] == b[j])
        return underscore_count_b - underscore_count_a; // Plus d'underscores => priorité plus élevée

    return a[i] - b[j];
}

void insertion_sort(t_symbol_data *array, int n, int reverse) {
    int i, j;
    t_symbol_data key;
    int compare;

    for (i = 1; i < n; i++) {
        key = array[i];
        j = i - 1;

        while (j >= 0) {
            compare = compare_strings(array[j].name, key.name);
            if (reverse ? compare < 0 : compare > 0) {
                array[j + 1] = array[j];
                j = j - 1;
            }
            else
                break ;
        }    
        array[j + 1] = key;
    }
}

int is_upper(char c) {
    if  (c >= 65 && c <= 90)
        return 0;
    return 1;  
}