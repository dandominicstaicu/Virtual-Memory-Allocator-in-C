CC=gcc
CFLAGS=-Wall -Wextra -pedantic
DEBUG=-g -ggdb -O0 -march=native

ELF=*

.PHONY: build, clean

build: $(ELF)

$(ELF): $(ELF).c
	$(CC) $(CFLAGS) $^ -o $@

run: $(ELF)
	./$< < test.in
