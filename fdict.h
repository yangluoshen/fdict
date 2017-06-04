#ifndef __DICT_H
#define __DICT_H

#include "flist.h"

typedef void* fdict_key_t;
typedef unsigned int index_t;

typedef struct fdict{
    index_t hash_size;
    flist** hash_list;
    int (*match)(void*, fdict_key_t);
    index_t (*calc)(struct fdict*, fdict_key_t);
}fdict;

typedef int (*hash_match_func)(void* ptr, fdict_key_t key);
typedef index_t (*hash_calc_func)(fdict* d, fdict_key_t key);

fdict* fdict_create(unsigned int hash_size, hash_match_func match, hash_calc_func calc);
void fdict_release(fdict* d);
int fdict_insert(fdict* d, fdict_key_t key, void* val);
int fdict_remove(fdict* d, fdict_key_t key);
void* fdict_find(fdict* d, fdict_key_t key);

index_t hash_calc_int(fdict* d, fdict_key_t key);
index_t hash_calc_str0(fdict* d, fdict_key_t key);
index_t hash_calc_str1(fdict* d, fdict_key_t key);

enum E_FDICT_RETCODE{
    FDICT_SUCCESS = 0,
    FDICT_FAILED,
    FDICT_EXIST,
    FDICT_NOTEXIST
};


#endif
