all: int-int str-int

int-int: int.int.c ../dict.c ../dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I../

str-int: str.int.c ../dict.c ../dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I../

clean:
	rm -f int-int str-int log core val.log
