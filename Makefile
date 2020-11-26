OBJS	= src/chip8.o
SOURCE	= src/chip8.c
HEADER	= 
OUT	= chip8
CC	 = gcc
FLAGS	 = -g3 -c -Wall
LFLAGS	 = -Wall

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

src/chip8: src/chip8.c
	$(CC) $(FLAGS) src/chip8.c 


clean:
	rm -f $(OBJS) $(OUT)

debug: $(OUT)
	valgrind $(OUT)

valgrind: $(OUT)
	valgrind $(OUT)

valgrind_leakcheck: $(OUT)
	valgrind --leak-check=full $(OUT)

valgrind_extreme: $(OUT)
	valgrind --leak-check=full --show-leak-kinds=all --leak-resolution=high --track-origins=yes --vgdb=yes $(OUT)