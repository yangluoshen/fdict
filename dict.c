/*  dict -- A C hash table implemented with seperate chaining.
 *
 *  Copyright (c) 2017, yangluo shen <yangluoshen at gmail dot com>

 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:

 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.

 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#define _BSD_SOURCE
#include "dict.h"
#include <malloc.h>
#include <string.h>

#define MIN_HASHNUM (100)
#define HASHNUM_DIV (17)

#define dict_free_key(d, entry) \
    if ((d)->op->key_destructor) \
        (d)->op->key_destructor((entry)->key)

#define dict_free_val(d, entry) \
    if ((d)->op->val_destructor) \
        (d)->op->val_destructor((entry)->val.v)

#define dict_hash(d, key)  ((d)->op->hash_func((d)->size, key))

#define dict_key_compare(d, key1, key2) \
    ((d)->op->key_comp ? (d)->op->key_comp(key1, key2) : \
        (key1) == (key2)) 

#define dict_set_key(d, entry, _key) \
    do{(entry)->key = (d)->op->key_dup ? (d)->op->key_dup(_key) : _key;}while(0)

#define dict_set_val(d, entry, _val) \
    do{(entry)->val.v = (d)->op->val_dup ? (d)->op->val_dup(_val) : _val;}while(0)

uint64_t prime_pool[] = {13,31,61,127,251,509,1021,2039,4093,8191,16381,32749,65521,131071,262139,524287,1048573,2097143,4194301,8388593,16777213,33554393,67108859,134217689,268435399,536870909,1073741789,2147483647};

#if 0
/*@return : 0 means num is not a prime
 *          1 means num is a prime
 */
int is_prime(index_t num)
{
    index_t i;
    index_t t = num / 2;
    for (i = 2; i < t; ++i){
        if (0 == (num % i))
            return 0;
    }
    return 1;
}

index_t get_better_hashnum(index_t hash_size)
{
    if (hash_size < MIN_HASHNUM)
        return hash_size;
    index_t t = hash_size / HASHNUM_DIV;
    index_t i = t;
    for (i = t; i < hash_size; ++i){
        if (is_prime(i))
            return i;
    }
    if (0 == (i % 2)) i += 1;

    return i;
}
#endif

int  _dict_init(dict* d, const dict_option* op)
{
    d->op = (dict_option*) malloc(sizeof(dict_option));
    if (!d->op) return -1;

    d->ht = (dict_entry**) malloc(d->size * sizeof(dict_entry*));
    if (!d->ht) {
        free (d->op);
        return -1;
    }
    
    d->op->hash_func = op->hash_func;
    d->op->key_dup = op->key_dup;
    d->op->val_dup = op->val_dup;
    d->op->key_comp = op->key_comp;
    d->op->key_destructor = op->key_destructor;
    d->op->val_destructor = op->val_destructor;

    d->used = 0;
    return 0;
}

dict* dict_create(const dict_option* op)
{
    if (!op) return NULL;
    dict* d = (dict*) malloc(sizeof(dict));
    if (!d) return NULL;

    if (!op->hash_func) return NULL;

    d->size = prime_pool[0];
    
    if (-1 == _dict_init(d, op)){
        free (d);
        return NULL;
    }

    return d;
}

void dict_release(dict* d)
{
    if (!d) return;

    int i;
    for (i=0; i<d->size; ++i){
        if (d->ht[i] == NULL) continue;

        dict_entry** indirect = &(d->ht[i]);
        while (*indirect) {
            dict_entry* he = *indirect;

            dict_free_key(d, he);
            dict_free_val(d, he);
            
            free (he);
            d->used --;

            // next loop
            indirect = &((*indirect)->next);
        }
    }
    
    free (d->ht);
    free (d->op);
}

dict_entry* dict_add(dict* d, void* key, void* val)
{
    if (!d || !key) return NULL;
    
    uint64_t hash = dict_hash(d, key);
    
    // invalid hash will return NULL
    if (hash >= d->size) return NULL;

    dict_entry* he = d->ht[hash];
    while (he){
        // if already exist, return NULL
        if (dict_key_compare(d, key, he->key))
            return NULL;

        he = he->next;
    }

    he = (dict_entry*) malloc(sizeof(dict_entry));
    if (!he) return NULL;

    // push head
    he->next = d->ht[hash];
    d->ht[hash] = he;

    d->used ++;
    dict_set_key(d, he, key);
    dict_set_val(d, he, val);

    return he;
}

dict_entry* dict_find(dict* d, void* key)
{
    if (!d || !key) return NULL;

    uint64_t hash = dict_hash(d, key);
    if (hash >= d->size) return NULL;

    dict_entry* he = d->ht[hash];
    while (he){
        if (dict_key_compare(d, key, he->key))
            return he;

        he = he->next;
    }

    return NULL;
}

int dict_delete(dict* d, void* key)
{
    if (!d || !key) return -1;

    uint64_t hash = dict_hash(d, key);
    if (hash > d->size) return -1;

    dict_entry** in_he = &(d->ht[hash]);
    while(*in_he){
        if (dict_key_compare(d, key, (*in_he)->key)){
            dict_entry* he = *in_he;
            dict_free_key(d, he);
            dict_free_val(d, he);

            *in_he = (*in_he)->next;
            free(he);
            d->used --;
            return 0;
        }

        in_he = &((*in_he)->next);
    }

    return -1;
}


#if 0
dict* fdict_create(unsigned int hash_size, hash_match_func match, hash_calc_func calc)
{
    hash_size = get_better_hashnum(hash_size);
    if (0 == hash_size) return NULL;
    if (!match || !calc) return NULL;
    
    dict* d = (dict*) malloc (sizeof(dict));
    if (!d) return NULL;
    d->hash_size = hash_size;
    d->calc = calc;
    d->match = match;

    d->hash_list = (flist**) malloc(hash_size * sizeof(flist*));
    if (!d->hash_list) {
        free (d);
        return NULL;
    }

    index_t i = 0;
    for (; i < hash_size; ++i){
        d->hash_list[i] = flist_create(d->match);
        // if create failed, release the resource alloced
        if (!d->hash_list[i]){
            index_t j;
            for (j=0; j < i; ++j){
                free(d->hash_list[j]);
                d->hash_list[j] = NULL;
            }
            free(d->hash_list);
            free(d);
            return NULL;
        }
    }
    
    return d;
}

void fdict_release(dict* d)
{
    if (!d) return;
    index_t i = 0;
    for (; i < d->hash_size; ++i){
        flist_release(d->hash_list[i]);
    }

    free(d->hash_list);
    free (d);
}

/*@return: 0 success
 *         1 failed
 *         2 exist
 *@val: val is a customed pointer
 */
int fdict_insert(dict* d, fdict_key_t key, void* val)
{
    if (!d) return FDICT_FAILED;
    if (fdict_find(d, key)) return FDICT_EXIST;

    index_t hash_slot = d->calc(d, key);
    if (-1==hash_slot || hash_slot>=d->hash_size) 
        return FDICT_FAILED;

    if (!flist_push(d->hash_list[hash_slot], val))
        return FDICT_FAILED;

    return FDICT_SUCCESS;
}

void* fdict_find(dict* d, fdict_key_t key)
{
    if (!d) return NULL;
    fnode* n;
    index_t hash_slot = d->calc(d, key);
    if (-1 == hash_slot || hash_slot >= d->hash_size)
        return NULL;
    n = flist_find(d->hash_list[hash_slot], key);
    return n ? n->val : NULL;
}
int fdict_remove(dict* d, fdict_key_t key) { if (!d) return FDICT_FAILED; fnode* n = fdict_find(d, key); if (!n) return FDICT_NOTEXIST; 
    index_t hash_slot = d->calc(d, key);
    if (-1==hash_slot || hash_slot>=d->hash_size)
        return FDICT_FAILED;

    flist_delete(d->hash_list[hash_slot], key);
    return FDICT_SUCCESS;
}

#endif

/* key为整数的散列函数
 * 一个简单的散列函数
 * 注意: 要留意原key的类型是否与uint64_t是一致的。
 * 若不一致，不应该使用此散列函数，否则可能会导致错误，
 * 尤其是key的类型字节长为比uint64_t小时（如short,char,unsigned char）
 */
uint64_t hash_calc_int(uint64_t hash_size, void* key)
{
    if (!key) return -1;
    uint64_t hash = *((uint64_t*)key) % hash_size;
    return hash;
}

/* key 为字符串的散列函数
 * 计算简单，但是不适合key特别长的情况
 * 因为key太长的话，此散列函数会花很多时间计算
 */
uint64_t hash_calc_str0(uint64_t hash_size, const void* key)
{
    uint64_t hash = 0;
    char* k = (char*)key;
    while(*k != 0){
        hash = (hash << 5) + *k++;
    }
    return (uint64_t) (hash % hash_size);
}

uint64_t hash_calc_str1(uint64_t hash_size, void* key)
{
    if (!key) return -1;
    
    char* name = (char*) key;
    unsigned int hash = 0;
    while(*name){
        hash = (hash << 4) + *name;
        hash ^= (hash & 0xF0000000) >> 24;
        name++;
    }
    hash &= 0x0FFFFFFF;
    return (uint64_t) (hash % hash_size);
}

#ifdef FDICT_MAIN
#include <assert.h>
#include <stdio.h>

typedef struct {
    char* name;
    int price;
}fruit;

fruit* gen_fruit(const char* name, int price)
{
    fruit* f = (fruit*) malloc(sizeof(fruit));
    if (!f) return NULL;

    f->name = strdup(name);
    f->price = price;
    return f;
}

void* fruit_dup (const void* val)
{
    const fruit* f = (const fruit*) val;
    return gen_fruit(f->name, f->price); 
}

void fruit_free(void* val)
{
    if (!val) return;
    free (((fruit*)val)->name);
    free(val);
    val = NULL;
}

void* name_dup(const void* key)
{
    return strdup((const char*)key);
}

int fruit_comp(const void* key1, const void* key2)
{
    return strcasecmp((const char*)key1, (const char*)key2) == 0;
}

dict_option fruit_op = {
    hash_calc_str0,
    fruit_comp,
    name_dup,
    free,
    fruit_dup,
    fruit_free
};

/*
void print_list(flist* l)
{
    fnode* n = l->next;
    while(n){
        node* v = n->val;
        if (v){
            printf(" %4d", v->value);
        }
        n = n->next;
    }
    puts("");
}

void print_dict(dict* d)
{
    if (!d) return;
    puts("");
    uint64_t i; 
    for (i = 0; i < d->hash_size; ++i){
        printf("[%2u]:", i);
        print_list(d->hash_list[i]);
    }
    puts("");
}
*/

fruit fruit_pool[] = {
    {"apple", 5999},
    {"banana", 5},
    {"pear", 4},
    {"straberry", 10},
    {"watermelon", 3},
    {"peach", 7}
};

void testcase1()
{
    dict* d = dict_create(&fruit_op);
    assert(d != NULL);
    
    assert(NULL!=dict_add(d, fruit_pool[0].name, &fruit_pool[0]));
    dict_entry* he = dict_find(d, "apple");
    assert(he && (5999 == ((fruit*)he->val.v)->price));
    assert(0 == dict_delete(d, "apple"));
    assert(!dict_find(d, "apple"));
}

#if 0
void testcase2()
{
    puts("testcase3");
    dict* d;
    GENERATE_DICT(d,10);
    
    node* node1 = generate(1, 1);
    node* node2 = generate(2, 2);
    node* node3 = generate(3, 3);
    node* node5 = generate(5, 5);
    node* node15 = generate(15, 15);

    assert(FDICT_SUCCESS==fdict_insert(d, &node1->id, node1));
    assert(FDICT_SUCCESS==fdict_insert(d, &node2->id, node2));
    assert(FDICT_SUCCESS==fdict_insert(d, &node3->id, node3));
    assert(FDICT_SUCCESS==fdict_insert(d, &node5->id, node5));
    assert(FDICT_SUCCESS==fdict_insert(d, &node15->id, node15));
    assert(FDICT_SUCCESS!=fdict_insert(d, &node3->id, node3));
    print_dict(d);
    assert(FDICT_SUCCESS==fdict_remove(d, &node5->id));
    assert(FDICT_SUCCESS==fdict_remove(d, &node15->id));
    assert(FDICT_SUCCESS!=fdict_remove(d, &node15->id));
    assert(FDICT_SUCCESS==fdict_remove(d, &node3->id));
    print_dict(d);

    fdict_release(d);
    free(node1); free(node2); free(node3); free(node5); free(node15);
}
#endif

int main()
{
    testcase1();
    //testcase2();

    return 0;
}
#endif
