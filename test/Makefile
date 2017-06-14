all: rehash str-int

rehash: rehash.c ../dict.c ../dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I../

str-int: str.int.c ../dict.c ../dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I../

clean:
	rm -f rehash str-int log core
