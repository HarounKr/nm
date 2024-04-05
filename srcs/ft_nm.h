#pragma once

#include "../libft/libft.h"
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <elf.h>

typedef struct {
    char *adress;
    char type;
    char *symbol_name;
} symbol_data;

char *text_sections[4]  = { ".text", ".init", 
                            ".fini", NULL
                        };
char *data_sections[6] = {  ".data", ".init_array", 
                            ".fini_array", ".dynamic",
                            ".got", NULL,
                        };
char *ro_sections[5] = {    ".rodata", ".eh_frame", 
                            ".eh_frame_hdr", ".note.ABI-tag", NULL,
                        };

int handle_file_errors(int fd, struct stat buf);
