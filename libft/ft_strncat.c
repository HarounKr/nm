#include "libft.h"

char *ft_strncat(char *dst, const char *src, size_t n) {
    char *ptr = dst;
    
    while (*ptr != '\0') {
        ptr++;
    }
    while (n > 0 && *src != '\0') {
        *ptr++ = *src++;
        n--;
    }
    
    *ptr = '\0';
    
    return dst;
}