CC=gcc
CFLAGS=-Wall -Werror -Wextra

SRC= tests/arrays.c tests/env.c tests/hello.c tests/inline.c tests/pointers.c tests/strings.c tests/vars.c \
    tests/dynamic.c tests/filesops.c tests/hidden.c tests/maths.c tests/static.c tests/structs.c tests/weak.c

# Dossiers de destination pour les fichiers objets et exécutables
OBJ32=$(SRC:tests/%.c=obj32/%.o)
OBJ64=$(SRC:tests/%.c=obj64/%.o)
EXE32=$(SRC:tests/%.c=elf32/%)
EXE64=$(SRC:tests/%.c=elf64/%)

# all: objdirs build32 build64
all: objdirs build64

objdirs:
	mkdir -p obj32 obj64 elf32 elf64

# build32: $(OBJ32) $(EXE32)
build64: $(OBJ64) $(EXE64)

# obj32/%.o: tests/%.c
# 	$(CC) $(CFLAGS) -m32 -c $< -o $@

obj64/%.o: tests/%.c
	$(CC) $(CFLAGS) -m64 -c $< -o $@

# elf32/%: obj32/%.o
# 	$(CC) $(CFLAGS) -m32 $< -o $@ -lm

elf64/%: obj64/%.o
	$(CC) $(CFLAGS) -m64 $< -o $@ -lm

clean:
	rm -rf obj32 obj64 elf32 elf64

re: clean all
