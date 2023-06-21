BIN=MLSTDists
TEST_DATA=test.tab


bin/$(BIN): src/*.c
	gcc -Ofast -mtune=native -march=native -o $@ $^


.PHONY: test_mat
test_mat:
	python ./scripts/make_mat.py > test/$(TEST_DATA)