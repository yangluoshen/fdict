dict: dict.c dict.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -DFDICT_MAIN

valgrind:
	valgrind --log-file=val.log --track-origins=yes --leak-check=full ./dict

clean:
	rm -f dict core val.log
