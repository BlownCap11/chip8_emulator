Flags := -O3 -Wall -Wextra

Chip8: ./Src/*.c
	gcc $(Flags) ./Src/*.c -o Chip8 -lSDL2