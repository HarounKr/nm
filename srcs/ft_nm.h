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

char *data_sections[3] = {  ".data", ".init_array", 
                            ".fini_array", 
                        };
char *ro_sections[4] = {    ".rodata", ".eh_frame", 
                            ".eh_frame_hdr", ".note.ABI-tag",
                        };

int handle_file_errors(int fd, struct stat buf);
