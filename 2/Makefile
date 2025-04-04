CC = gcc
CFLAGS = -pedantic -std=c99 -ggdb3 -lm

SRCS = main.c copy_file.c finder.c main_fun.c
OBJS = $(SRCS:.c=.o)
HEADERS = sp2_com.h main_fun.h

all: $(HEADERS) $(SRCS)
	$(CC) main_fun.c main.c $(CFLAGS) -o main.elf
	$(CC) copy_file.c $(CFLAGS) -o copyifier.elf
	$(CC) finder.c $(CFLAGS) -o finder.elf
	chmod +x copyifier.elf
	chmod +x finder.elf



