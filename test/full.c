
#include "fdict.h"
#include <assert.h>
#include <malloc.h>

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

size_t calc_func(fdict* d, fdict_key_t key)
{
    if (!d || !key) return -1;
    int hash = *((int*)key) % d->hash_size;
    return (size_t)hash;
}
int match_func(void* ptr, fdict_key_t key)
{
    if (!ptr || !key) return 0;
    node* n = (node*) ptr;
    return (n->id == *(int*)key) ? 1 : 0;
}

#define GENERATE_DICT(d, s) \
    do {\
        (d) = fdict_create((s), match_func, calc_func);\
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
    size_t i; 
    for (i = 0; i < d->hash_size; ++i){
        printf("[%2lu]:", i);
        print_list(d->hash_list[i]);
    }
    puts("");
}

void testcase()
{
    puts("testcase4");
    fdict* d;
    GENERATE_DICT(d, 106);
    const int MAX_NODE_NUM = 300;
    node** nlist = (node**) malloc(MAX_NODE_NUM*sizeof(node*));
    if (!nlist) assert(NULL);
    int i;
    for (i=0; i < MAX_NODE_NUM; ++i){
        nlist[i] = generate(i,i);
    }
    
    for (i=0; i < MAX_NODE_NUM; ++i){
        fdict_insert(d, &(nlist[i]->id), nlist[i]);
    }
    print_dict(d);

    int key = 107; 
    node* s = fdict_find(d, &key);
    assert(s);
    printf ("[%d]-[%d] found\n", s->id, s->value); 

    for (i=0; i < MAX_NODE_NUM; ++i){
        fdict_insert(d, &(nlist[i]->id), nlist[i]);
    }
    print_dict(d);

    for (i=0; i<MAX_NODE_NUM; ++i){
        fdict_remove(d, &(nlist[i]->id));
    }
    print_dict(d);

    // release resource
    fdict_release(d);
    for (i=0; i<MAX_NODE_NUM;++i){
        free(nlist[i]);
        nlist[i] = NULL;
    }
    free(nlist);
    nlist = NULL;
}


int main(int argc, char* argv[])
{
    testcase();
    return 0;
}
