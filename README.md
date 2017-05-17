# fdict
C实现的字典（hash table)

## Description
  - 如何使用fdict可参考test/的测试用例
  - match 和 calc 需要使用者自己实现 
    - match: `typedef int (*hash_match_func)(void* ptr, fdict_key_t key)`;<br>
        (ptr为目标结构指针) <br>
    - calc: `typedef size_t (*hash_calc_func)(fdict* d, fdict_key_t key);`


