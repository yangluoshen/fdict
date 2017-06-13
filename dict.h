#ifndef __DICT_H
#define __DICT_H

#include <stdint.h>

typedef struct dict_entry{
    void* key;
    union {
        void* v;
        uint64_t u64;
        int64_t s64;
        double d;
    }val;
    struct dict_entry* next;
}dict_entry;

typedef struct dict_option{
    uint64_t (*hash_func)(uint64_t hash_size, const void* key);
    int (*key_comp)(const void* key1, const void* key2);
    void* (*key_dup)(const void* key);
    void (*key_destructor)(void* key);
    void* (*val_dup)(const void* val);
    void (*val_destructor)(void* val);
}dict_option;

typedef struct dict{
    uint64_t size;
    dict_entry** ht;
    dict_option* op;
    uint64_t used;
}dict;

dict* dict_create(const dict_option* op);
void dict_release(dict* d);
dict_entry* dict_add(dict* d, void* key, void* val);
int dict_delete(dict* d, void* key);
void* fdict_find(dict* d, void* key);

#endif
