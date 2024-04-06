#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashmap.h"

/* Client implements his/hers own hash_function and compare_function.*/
/* We don't assume type, client choses. If map contains X datatype,
then hash_function must hash X datatype*/
hash_t hash_function(data_t data)
{
    hash_t hash = 257;
    long long *d_ptr = data;
    long long d = *d_ptr;
    while (d > 0)
    {
        hash = hash * 37 + d % 10;
        d = d / 10;
    }
    return hash;
}

int compare_function_asc(hash_t a, hash_t b)
{
    int c = a - b;
    return c;
}

int compare_function_desc(hash_t a, hash_t b)
{
    int c = a-b;
    c *= (-1);
    return c;
}

void print_function(data_t data)
{
    long long *d_ptr = data;
    long long d = *d_ptr;
    printf("%lld", d);
}

int main()
{
    struct hashmap_t* myhashmap = hashmap_create(hash_function, compare_function_asc);
    if(myhashmap == NULL)
    {
        printf("Fatal error: malloc failed.\n");
        return 1;
    }
    long long myData[] = {80, 20, 10, 40, 60, 50, 70, 100, 90, 20, 10, 0, 60, 0, 60, 70, 90, 90, 90};
    for (int i = 0; i < sizeof(myData)/sizeof(myData[0]); i++)
    {
        hashmap_add(myhashmap, (data_t)(myData+i));    
    }
    hashmap_print(myhashmap, print_function);

    printf("\n myhashmap[40] = %lld\n", (long long)hashmap_get(myhashmap, hash_function(&myData[3]), false));
    printf("\n myhashmap[90] & remove = %lld\n", (long long)hashmap_get(myhashmap, hash_function(&myData[8]), true));
    hashmap_print(myhashmap, print_function);
    printf("\n myhashmap[60] & remove = %lld\n", (long long)hashmap_get(myhashmap, hash_function(&myData[4]), true));
    hashmap_print(myhashmap, print_function);
    hashmap_add(myhashmap, &myData[11]);
    printf("\n");
    hashmap_print(myhashmap, print_function);
    hashmap_free(myhashmap);
    return 0;
}