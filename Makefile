BIN=MLSTDists



bin/$(BIN): src/*.c
	gcc -Ofast -mtune=native -march=native -o $@ $^