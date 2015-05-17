CC=gcc
EXE=fft.out
FLAGS=-O3

$(EXE): main.c fft.h
	@echo "$(CC) $< -o $@"
	@$(CC) $< -o $@

