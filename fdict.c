/*  fdict -- A C hash table implemented with seperate chaining.
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

#include "fdict.h"
#include <malloc.h>

#define MIN_HASHNUM (100)
#define HASHNUM_DIV (17)

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

fdict* fdict_create(unsigned int hash_size, hash_match_func match, hash_calc_func calc)
{
    hash_size = get_better_hashnum(hash_size);
    if (0 == hash_size) return NULL;
    if (!match || !calc) return NULL;
    
    fdict* d = (fdict*) malloc (sizeof(fdict));
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

void fdict_release(fdict* d)
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
int fdict_insert(fdict* d, fdict_key_t key, void* val)
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

void* fdict_find(fdict* d, fdict_key_t key)
{
    if (!d) return NULL;
    fnode* n;
    index_t hash_slot = d->calc(d, key);
    if (-1 == hash_slot || hash_slot >= d->hash_size)
        return NULL;
    n = flist_find(d->hash_list[hash_slot], key);
    return n ? n->val : NULL;
}
int fdict_remove(fdict* d, fdict_key_t key) { if (!d) return FDICT_FAILED; fnode* n = fdict_find(d, key); if (!n) return FDICT_NOTEXIST; 
    index_t hash_slot = d->calc(d, key);
    if (-1==hash_slot || hash_slot>=d->hash_size)
        return FDICT_FAILED;

    flist_delete(d->hash_list[hash_slot], key);
    return FDICT_SUCCESS;
}

/* key为整数的散列函数
 * 一个简单的散列函数
 * 注意: 要留意原key的类型是否与index_t是一致的。
 * 若不一致，不应该使用此散列函数，否则可能会导致错误，
 * 尤其是key的类型字节长为比index_t小时（如short,char,unsigned char）
 */
index_t hash_calc_int(fdict* d, fdict_key_t key)
{
    if (!d || !key) return -1;
    index_t hash = *((index_t*)key) % d->hash_size;
    return hash;
}

/* key 为字符串的散列函数
 * 计算简单，但是不适合key特别长的情况
 * 因为key太长的话，此散列函数会花很多时间计算
 */
index_t hash_calc_str0(fdict* d, fdict_key_t key)
{
    index_t hash = 0;
    char* k = (char*)key;
    while(*k != 0){
        hash = (hash << 5) + *k++;
    }
    return (index_t) (hash % d->hash_size);
}

index_t hash_calc_str1(fdict* d, fdict_key_t key)
{
    if (!d || !key) return -1;
    
    char* name = (char*) key;
    unsigned int hash = 0;
    while(*name){
        hash = (hash << 4) + *name;
        hash ^= (hash & 0xF0000000) >> 24;
        name++;
    }
    hash &= 0x0FFFFFFF;
    return (index_t) (hash % d->hash_size);
}

#ifdef FDICT_MAIN
#include <assert.h>
#include <stdio.h>

typedef struct {
    int value;
    int id;
}node;

node* generate(int id, int val)
{
    node* n = (node*) malloc(sizeof(node));
    if (!n) return NULL;
    n->value = val;
    n->id = id;
    return n;
}

int match_func(void* ptr, fdict_key_t key)
{
    if (!ptr || !key) return 0;
    node* n = (node*) ptr;
    return n->id == *(int*)key;
}

#define GENERATE_DICT(d, s) \
    do {\
        (d) = fdict_create((s), match_func, hash_calc_int);\
        assert(d);\
    }while(0)

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

void print_dict(fdict* d)
{
    if (!d) return;
    puts("");
    index_t i; 
    for (i = 0; i < d->hash_size; ++i){
        printf("[%2u]:", i);
        print_list(d->hash_list[i]);
    }
    puts("");
}

void testcase1()
{
    fdict* d;
    GENERATE_DICT(d,10);
    
    node* node1 = generate(1, 1);
    
    assert(FDICT_SUCCESS==fdict_insert(d, &node1->id, node1));
    print_dict(d);
    assert(FDICT_SUCCESS!=fdict_insert(d, &node1->id, node1));
    int key = 1; 
    node* s = fdict_find(d, &key);
    assert(s);
    printf ("testcase2: [%d]-[%d] found\n", s->id, s->value); 
    fdict_release(d);
    free (node1);
}

void testcase2()
{
    puts("testcase3");
    fdict* d;
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

int main()
{
    testcase1();
    testcase2();

    return 0;
}
#endif
