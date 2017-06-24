#define _BSD_SOURCE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>

#include "dict.h"

typedef struct namelist{
    char* name;
    struct namelist* next;
}namelist;

const long max_nums = 1000000;
namelist** nh;

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

dict_option murmur_op = {
    hash_calc_murmur,
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

char* gen_name()
{
    const int len = 1024;
    char name[len];
    int i;
    for (i=0; i < len-1; ++i){
        int r = rand();
        name[i] = (r%2) ? ('a'+r%26) : ('A'+r%26);
    }
    name[len-1] = 0;

    return strdup(name);
}

void print_name(void* key, void* val)
{
    printf(" {%8s, %2ld} ", (char*)key, (long)val);
}

void test_add(dict* d)
{
    struct timeval st, et;
    gettimeofday (&st, NULL);
    namelist* nl = *nh;
    long i;
    for (i = 0; i < max_nums; ++i){
        dict_add(d, nl->name, (void*)i);
        nl = nl->next;
    }
    gettimeofday (&et, NULL);
    printf("add  time usage: %8ldms\n", (et.tv_sec-st.tv_sec)*1000+(et.tv_usec/1000-st.tv_usec/1000));

}

void test_find(dict* d)
{
    struct timeval find_st, find_et;
    gettimeofday(&find_st, NULL);
    namelist* nn = *nh;
    int count;
    for (count=0; nn; count++){
        if ((count % 97) == 0)
            assert(NULL != dict_find(d, nn->name));

        nn = nn->next;
    }
    gettimeofday(&find_et, NULL);
    printf("find time usage: %8ldms\n", (find_et.tv_sec-find_st.tv_sec)*1000+(find_et.tv_usec/1000-find_st.tv_usec/1000));

}

void test_delete(dict* d)
{
    struct timeval free_st, free_et;
    gettimeofday(&free_st, NULL);
    dict_release(d);
    gettimeofday(&free_et, NULL);
    printf("free time usage: %8ldms\n", (free_et.tv_sec-free_st.tv_sec)*1000+(free_et.tv_usec/1000-free_st.tv_usec/1000));
}

namelist** init_namelist()
{
    namelist** nl = (namelist**) malloc(sizeof(namelist*));
    assert(NULL != nl);
    *nl = NULL;

    long i;
    for (i = 0; i<max_nums; ++i){
        char* n = gen_name();
        namelist* nn = (namelist*)malloc(sizeof(namelist));
        assert(NULL!=nn);
        nn->name = n;
        nn->next = *nl;
        *nl = nn;
    }

    return nl;
}

void free_namelist(namelist** nl)
{
    while (*nl){
        namelist* t =  *nl;
        *nl = t->next;
        free(t->name);
        free(t);
    }
    free(nl);
}

void testcase0()
{
    printf("difference between two kinds of hash calculate function\n");
    dict* strd = dict_create(&str_op);
    assert(NULL != strd);
    dict* murd = dict_create(&murmur_op);
    assert(NULL != murd);

    nh = init_namelist();

    printf("> common string hash calculate\n");
    test_add(strd);
    test_find(strd);
    test_delete(strd);

    printf("\n> use murmurhash \n");
    test_add(murd);
    test_find(murd);
    test_delete(murd);

    //print_dict(d, print_name);
    free_namelist(nh);
}

int main()
{
    testcase0();

    return 0;
}

