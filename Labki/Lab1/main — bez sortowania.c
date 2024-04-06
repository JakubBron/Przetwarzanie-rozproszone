#include <stdio.h>
#include <stdlib.h>
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

struct hashmap_t* hashmap_create(hash_t(*hash_function)(data_t), int (*compare_function)(hash_t, hash_t)) {
    struct hashmap_t* hashmap = (struct hashmap_t*)malloc(sizeof(struct hashmap_t));
    hashmap->head = NULL;
    hashmap->hash_function = hash_function;
    hashmap->compare_function = compare_function;
    return hashmap;
}

void hashmap_add(struct hashmap_t* hashmap, data_t data)
{
    hash_t hash = hashmap->hash_function(data);
    struct hashmap_node_t* node = hashmap->head;
    while (node != NULL) /*Checking if node with such key (hash) already exists, append data there*/
    {
        if (node->hash == hash)  /*node with this data existed, add to DLL (datanode_t)*/
        {
            /*add new element to the end of doubly linked list*/
            struct datanode_t* datanode = node->head;
            while (datanode->next != NULL)
            {
                datanode = datanode->next;
            }
            struct datanode_t* new_datanode = (struct datanode_t*)malloc(sizeof(struct datanode_t));
            new_datanode->data = data;
            new_datanode->next = NULL;
            new_datanode->prev = datanode;
            datanode->next = new_datanode;
            return;
        }
        node = node->next;
    }
    /*node with this hash did not exist, create new node and add data to it*/
    struct hashmap_node_t* new_node = (struct hashmap_node_t*)malloc(sizeof(struct hashmap_node_t));
    new_node->hash = hash;
    new_node->head = (struct datanode_t*)malloc(sizeof(struct datanode_t));
    new_node->head->data = data;
    new_node->head->next = NULL;
    new_node->head->prev = NULL;
    new_node->next = NULL;
    new_node->prev = NULL;
    /*append new_node to the end of doubly linked list*/
    node = hashmap->head;  // Question: is 'node' pointing to the last element of the DLL after while loop above?
    if (hashmap->head == NULL)
    {
        hashmap->head = new_node;
    }
    else
    {
        while (node->next != NULL)
        {
            node = node->next;
        }
        node->next = new_node;
        new_node->prev = node;
    }
}

data_t hashmap_get(struct hashmap_t* hashmap, hash_t hash, bool remove)
{
    struct hashmap_node_t* node = hashmap->head;
    while (node != NULL) {
        if (node->hash == hash)
        {
            struct datanode_t* datanode = node->head;
            data_t toReturn = datanode->data;
            if (remove)
            {
                /*remove last element from doubly linked list*/
                if (datanode->next == NULL)
                {
                    if (datanode->prev != NULL)
                    {
                        datanode->prev->next = NULL;
                    }
                    else
                    {
                        node->head = NULL;
                    }
                    free(datanode);
                }
                else
                {
                    while (datanode->next != NULL)
                    {
                        datanode = datanode->next;
                    }
                    datanode->prev->next = NULL;
                    free(datanode);
                }

                if(node->head == NULL) /*No values with this hash is stored, remove node*/
                {
                    /*remove hashmap node from hashmap*/
                    struct hashmap_node_t* temp = node;
                    if (node->prev != NULL)
                    {
                        node->prev->next = node->next;
                    }
                    else
                    {
                        hashmap->head = node->next;
                    }
                    if (node->next != NULL)
                    {
                        node->next->prev = node->prev;
                    }
                    else 
                    {
                        if (node->prev != NULL)
                        {
                            node->prev->next = NULL;
                        }
                    }
                    free(temp);
                }
            }
            return toReturn;

        }
        node = node->next;
    }
    return NULL;
}

void hashmap_free(struct hashmap_t* hashmap)
{
    struct hashmap_node_t* node = hashmap->head;
    while (node != NULL)
    {
        struct datanode_t* datanode = node->head;
        while (datanode != NULL)
        {
            struct datanode_t* temp = datanode;
            datanode = datanode->next;
            free(temp);
        }
        struct hashmap_node_t* temp = node;
        node = node->next;
        free(temp);
    }
    free(hashmap);
}

void hashmap_print(struct hashmap_t* hashmap)
{
    struct hashmap_node_t* node = hashmap->head;
    int iter = 0;
    while (node != NULL)
    {
        printf("[%d] hash: %d\n", iter, node->hash);
        struct datanode_t* datanode = node->head;
        while (datanode != NULL)
        {
            printf("\tdata: %d\n", (int)datanode->data);
            datanode = datanode->next;
        }
        node = node->next;
        iter++;
    }
}


/* Client implements his/hers own hash_function and compare_function.*/
/* We don't assume type, client choses. If map contains X datatype,
then hash_function must hash X datatype*/
hash_t hash_function(data_t data)
{
    hash_t hash = 2137;
    int d = (int)data;
    while (d > 0)
    {
        hash = hash * 37 + d % 10;
        d = d / 10;
    }
    return hash;
}

int compare_function(hash_t a, hash_t b)
{
    return a - b;
}

int main()
{
    printf("hash_function test: \n");
    printf("hash_function(30) = %llu\n", (hash_t)hash_function(30));
    printf("hash_function(100) = %llu\n", (hash_t)hash_function(100));

    struct hashmap_t* myhashmap = hashmap_create(hash_function, compare_function);
    for (int i = 1; i < 10; i++)
    {
        hashmap_add(myhashmap, (data_t)(i * 10));
        if (i % 3 == 0 || i % 5 == 0)
        {
            hashmap_add(myhashmap, (data_t)(i * 10));
        }
    }
    hashmap_print(myhashmap);
    printf("\n myhashmap[30] = %d\n", (int)hashmap_get(myhashmap, hash_function(30), false));
    printf("\n myhashmap[90] & remove = %d\n", (int)hashmap_get(myhashmap, hash_function(90), true));
    hashmap_print(myhashmap);
    printf("\n myhashmap[40] & remove = %d\n", (int)hashmap_get(myhashmap, hash_function(40), true));
    hashmap_print(myhashmap);
    hashmap_add(myhashmap, (data_t)100);
    printf("\n");
    hashmap_print(myhashmap);
    printf("\n myhashmap[10] & remove = %d\n", (int)hashmap_get(myhashmap, hash_function(10), true));
    hashmap_print(myhashmap);
    hashmap_add(myhashmap, (data_t)10);
    printf("\n");
    hashmap_print(myhashmap);

    hashmap_free(myhashmap);
    return 0;
}