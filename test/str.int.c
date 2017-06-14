#define _BSD_SOURCE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

#include "dict.h"

int str_comp (const void* key1, const void* key2)
{
    return strcmp((const char*) key1, (const char*) key2) == 0;
}

void* name_dup(const void* s)
{
    return strdup((const char*)s);
}

dict_option str_op = {
    hash_calc_str1,
    str_comp,
    name_dup,
    free,
    NULL,
    NULL
};

char* names[] = {
    "Fizz", "yangluo", "rengar", "Dive", "Allen", "Eric",
    "Maria", "Mike"
};

void print_name(void* key, void* val)
{
    printf(" {%8s, %2ld} ", (char*)key, (long)val);
}

void testcase0()
{
    dict* d = dict_create(&str_op);
    assert(NULL != d);

    long i;
    for (i = 0; i<sizeof(names)/sizeof(names[0]); ++i){
        assert(NULL!=dict_add(d, names[i], (void*)i));
    }

    print_dict(d, print_name);

    dict_release(d);
}

int main()
{
    testcase0();

    return 0;
}
