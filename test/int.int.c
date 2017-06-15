#include <stdio.h>
#include <assert.h>

#include "dict.h"

dict_option int_op = {
    hash_calc_int,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

void print_int(void* key, void* val)
{
    (void)key;
    printf(" {%3ld, %3ld} ", (long)key, (long)val);
}

void testcase0()
{
    dict* d = dict_create(&int_op);
    assert(NULL != d);

    long i, max=2000;
    for (i=0; i < max; i++){
        assert(NULL!=dict_add(d, (void*)i, (void*)(max-i)));
    }

    print_dict(d, print_int);

    dict_release(d);
}

int main()
{
    testcase0();

    return 0;
}
