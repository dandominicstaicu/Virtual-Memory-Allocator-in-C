#Copyright 2023 311CA Dan-Dominic Staicu <dando.ds11@gmail.com>

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic
DEBUG=-g -ggdb -O0 -march=native
FILES=$(wildcard *.c)

TARGET = vma

# all:
# 	make clean; make build; make run

build: $(TARGET)

vma: main.c
	$(CC) $(CFLAGS) $(FILES) -g -o vma

pack:
	zip -FSr 311CA_StaicuDan-Dominic_Tema1.zip README.md Makefile *.c *.h

clean:
	rm -rf $(TARGET)

run_vma:
	./vma

.PHONY: pack clean