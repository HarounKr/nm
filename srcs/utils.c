#include "ft_nm.h"

int compare_strings(const char *a, const char *b) {
    int i = 0, j = 0;
    while (a[i] != '\0' && !ft_isalpha(a[i])) 
        i++;  // Aller au premier caractère alphabétique
    while (b[j] != '\0' && !ft_isalpha(b[j])) 
        j++;  // Aller au premier caractère alphabétique

    while (a[i] != '\0' && b[j] != '\0') {
        char charA = ft_tolower(a[i]);
        char charB = ft_tolower(b[j]);

        if (charA != charB)
            return charA - charB;

        i++;
        j++;
        while (a[i] != '\0' && !ft_isalpha(a[i])) 
            i++;
        while (b[j] != '\0' && !ft_isalpha(b[j]))
            j++;
    }
    return a[i] - b[j];  // Compare the rest if one string ends
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