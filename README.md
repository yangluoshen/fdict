# fdict

C hash table implemented with seperate chaining.

## Usage

    make
    ./dict

## Description

* 如何使用fdict可参考test/的测试用例

* 散列函数备选方案
  * **hash_calc_int** : key为unsigned int 的散列函数
  * **hash_calc_str0** 和 **hash_calc_str1** : key 为字符串的散列函数

## Interesting

* rehash
  当装填因子(FILL_FACTOR)大于50%时，将会进行rehash（再散列）。
   
