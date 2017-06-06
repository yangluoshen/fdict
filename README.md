# fdict

C实现的字典（hash table)

## Usage

    make
or

    cd test/
    make
    ./strkey
    ./full > log 2>&1

## Description

* 如何使用fdict可参考test/的测试用例
* match 和 calc 需要使用者自己实现（或者使用散列函数备选方案，见下文）
  * match: **typedef int (*hash_match_func)(void* ptr, fdict_key_t key);** (ptr为目标结构指针)
  * calc(散列函数）: **typedef size_t (*hash_calc_func)(fdict* d, fdict_key_t key);**

* 散列函数备选方案
  * **hash_calc_int** : key为unsigned int 的散列函数
  * **hash_calc_str0** 和 **hash_calc_str1** : key 为字符串的散列函数

## Interesting

* HASHNUM_DIV

  [Testcase](./test/full.c)
  <p>HASHNUM_DIV的作用是为空间和时间的平衡做一些努力。
  我们知道，装填因子(元素个数与散列表大小的比值)r越小，引起冲突的机会越小，但是代价是空闲的槽就越多，这样内存使用率就相对低了。
  以key为整数时为例，HASHNUM_DIV保证了，只要key小于 hash_size，每条chain冲突数不大于HASHNUM_DIV。只要HASHNUM_DIV是个
  相对较小的常量，这样的冲突是可以接受的。
  当然前提是散列需要比较均匀，这点由hash_size保证， get_better_hashnum 能保证返回一个比较好的hash_size，一般为素数。</p>

