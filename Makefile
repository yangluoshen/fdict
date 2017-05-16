
dict: fdict.c flist.c fdict.h flist.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -DFDICT_MAIN

list: flist.c flist.h
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -DLIST_MAIN

clean:
	rm -f dict list core
