#define _BSD_SOURCE

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/time.h>

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

char* gen_name()
{
    const int len = 7;
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

typedef struct namelist{
    char* name;
    struct namelist* next;
}namelist;

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
    dict* d = dict_create(&str_op);
    assert(NULL != d);

    namelist** nh = (namelist**) malloc(sizeof(namelist*));
    assert(NULL != nh);
    *nh = NULL;


    struct timeval add_st, add_et, find_st, find_et, free_st, free_et;
    gettimeofday(&add_st, NULL);
    long i;
    for (i = 0; i<10000000; ++i){
        char* n = gen_name();
        dict_add(d, n, (void*)i);
        
        namelist* nn = (namelist*)malloc(sizeof(namelist));
        assert(NULL!=nn);
        nn->name = n;
        nn->next = *nh;
        *nh = nn;
    }
    gettimeofday(&add_et, NULL);
    printf("add  time usage: %8ldms\n", (add_et.tv_sec-add_st.tv_sec)*1000+(add_et.tv_usec/1000-add_st.tv_usec/1000));

    //print_dict(d, print_name);
    //
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

    gettimeofday(&free_st, NULL);
    free_namelist(nh);
    dict_release(d);
    gettimeofday(&free_et, NULL);
    printf("free time usage: %8ldms\n", (free_et.tv_sec-free_st.tv_sec)*1000+(free_et.tv_usec/1000-free_st.tv_usec/1000));
}


void testcase2()
{

    namelist** nh = (namelist**) malloc(sizeof(namelist*));
    assert(NULL != nh);
    *nh = NULL;

    int i;
    for (i=0; i< 10; ++i){
        char* n = gen_name();
        printf ("%d:%7s\n",i, n);

        namelist* nn = (namelist*)malloc(sizeof(namelist));
        assert(NULL!=nn);
        nn->name = n;
        nn->next = *nh;
        *nh = nn;
    }

    namelist* nl = *nh;
    while (nl){
        printf("%s\n", nl->name);
        nl = nl->next;
    }

    free_namelist(nh);
}

int main()
{
    testcase0();
    //testcase2();

    return 0;
}

