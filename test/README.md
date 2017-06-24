# fdict测试用例

## Usage

        make
        ./int-int > log
        time ./str-int > log
        make clean

## Display

	rengar@rengar-Y430P:/opt/github/mygit/fdict/test$ ./str-int 
	difference between two kinds of hash calculate function
	> common string hash calculate
	add  time usage:    16324ms
	find time usage:      253ms
	free time usage:     1323ms

	> use murmurhash 
	add  time usage:     9124ms
	find time usage:      131ms
	free time usage:      978ms


