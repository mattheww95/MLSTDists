BIN=MLSTDists
TEST_DATA=test.tab
CC=gcc
ARGS=-std=gnu99 -mtune native -Ofast


bin/$(BIN): src/*.c
	$(CC) $(ARGS) -o $@ $^


.PHONY: test_mat
test_mat:
	python ./scripts/make_mat.py > test/$(TEST_DATA)