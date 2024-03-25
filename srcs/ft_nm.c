#include "ft_nm.h"

int handle_file_errors(int fd, struct stat buf) {
    if (fd == -1) {
        ft_putstr_fd("Failed to open file\n", 2);
        return 1;
    }
    if (fstat(fd, &buf) != 0) {
        ft_putstr_fd("couldn't get file size.\n", 2);
        return 1;
    }
    if (S_ISDIR(buf.st_mode)) {
        ft_putstr_fd("Argument must be file.\n", 2);
        return 1;
    }
    return 0;
}

int main(int ac, char **av) {
    if (ac == 2) {
        struct stat buf;
        int fd = open(av[1], O_RDONLY, S_IRUSR);
        if (handle_file_errors(fd, buf) == 1)
            return 1;
        close(fd);
    } 
    else {
        ft_putstr_fd("Usage: ", 2);
        ft_putstr_fd(av[0], 2);
        ft_putstr_fd(" <file>\n", 2);
    }
    return 0;
}