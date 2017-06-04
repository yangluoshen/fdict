
#include "fdict.h"
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdio.h>

#define MAX_NAME_LEN (8)
typedef struct {
    char name[MAX_NAME_LEN];
    int age;
    int sex;
}animal;


int animal_hash_match(void* ani, fdict_key_t key)
{
    if (!ani || !key) return 0;
    return 0 == strcmp(((animal*)ani)->name, (char*)key);
}

animal* generate_ani(char* name, int age, int sex)
{
    animal* ani = (animal*) malloc(sizeof(animal));
    if (!ani) return NULL;
    strcpy(ani->name, name);
    ani->age = age;
    ani->sex = sex;
    return ani;
}

void print_ani_list(flist* l)
{
    fnode* n = l->next;
    while(n){
        animal* v = (animal*)n->val;
        if (v){
            printf("[%8s,%2d,%2d]", v->name,v->age,v->sex);
        }
        n = n->next;
    }
    puts("");
}

void print_ani_dict(fdict* d)
{
    if (!d) return;
    puts("");
    index_t i; 
    for (i = 0; i < d->hash_size; ++i){
        printf("[%2u]:", i);
        print_ani_list(d->hash_list[i]);
    }
    puts("");
}

void testcase()
{
    fdict* d = fdict_create(3, animal_hash_match, hash_calc_str0);
    assert(d);
    
    animal* ani0 = generate_ani("dog", 3, 1);
    animal* ani1 = generate_ani("cat", 2, 1);
    animal* ani2 = generate_ani("cattle", 5, 0);
    animal* ani3 = generate_ani("rabbit", 4, 0);
    
    fdict_insert(d, ani0->name, ani0);
    fdict_insert(d, ani1->name, ani1);
    fdict_insert(d, ani2->name, ani2);
    fdict_insert(d, ani3->name, ani3);
    print_ani_dict(d);

    animal* s = fdict_find(d, "dog");
    assert(s);
    printf ("[%s,%d,%d] found\n", s->name, s->age, s->sex); 

    fdict_release(d);
    free(ani0); free(ani1); free(ani2); free(ani3);
}

int main(int argc, char* argv[])
{
    testcase();
    return 0;
}

