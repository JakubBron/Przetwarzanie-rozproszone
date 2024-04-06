#include <stdbool.h>

typedef void* data_t;
typedef unsigned long long int hash_t;

struct datanode_t {
    struct datanode_t* next;
    struct datanode_t* prev;
    data_t data;
};

struct hashmap_node_t {
    hash_t hash;
    struct datanode_t* head;
    struct hashmap_node_t* next;
    struct hashmap_node_t* prev;

};

struct hashmap_t {
    struct hashmap_node_t* head;
    hash_t(*hash_function)(data_t);
    int (*compare_function)(hash_t, hash_t);
};


struct hashmap_t* hashmap_create(hash_t(*hash_function)(data_t), int (*compare_function)(hash_t, hash_t)) ;
void hashmap_add(struct hashmap_t* hashmap, data_t data);
data_t hashmap_get(struct hashmap_t* hashmap, hash_t hash, bool remove);
void hashmap_free(struct hashmap_t* hashmap);
void hashmap_print(struct hashmap_t* hashmap, void (*print_function)(data_t));