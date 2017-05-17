all: strkey full

strkey: strkey.c ../fdict.c ../flist.c 
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I ../

full: full.c ../fdict.c ../flist.c
	gcc -g $^ -o $@ -Wall -std=c99 -pedantic -I ../


clean:
	rm -f strkey full
