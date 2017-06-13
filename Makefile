
dict: dict.c dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -DFDICT_MAIN


clean:
	rm -f dict core
