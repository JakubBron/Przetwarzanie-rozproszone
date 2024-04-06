#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashmap.h"


struct hashmap_t* hashmap_create(hash_t(*hash_function)(data_t), int (*compare_function)(hash_t, hash_t)) 
{
    struct hashmap_t* hashmap = (struct hashmap_t*)malloc(sizeof(struct hashmap_t));
    if(hashmap == NULL) 
    {
        return NULL;
    }
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
        if (hashmap->compare_function(node->hash, hash) == 0)  /*node with this data existed, add to DLL (datanode_t)*/
        {
            /*add new element to the end of doubly linked list*/
            struct datanode_t* datanode = node->head;
            while (datanode->next != NULL)
            {
                datanode = datanode->next;
            }
            struct datanode_t* new_datanode = (struct datanode_t*)malloc(sizeof(struct datanode_t));
            if(new_datanode == NULL)
            {   
                printf("[hashmap_add] Fatal error: malloc failed.\n");
                return;
            }
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
    if(new_node == NULL)
    {   
        printf("[hashmap_add] Fatal error: malloc failed.\n");
        return;
    }
    new_node->hash = hash;
    new_node->head = (struct datanode_t*)malloc(sizeof(struct datanode_t));
    new_node->head->data = data;
    new_node->head->next = NULL;
    new_node->head->prev = NULL;
    new_node->next = NULL;
    new_node->prev = NULL;
    /*append new_node to correct place in doubly linked list*/
    node = hashmap->head;
    if (hashmap->head == NULL)
    {
        hashmap->head = new_node;
    }
    else
    {
        /*While current hash is bigger, move forward*/
        while (node->next != NULL && hashmap->compare_function(node->hash, hash) < 0)
        {
            node = node->next;
        }

        if (hashmap->compare_function(node->hash, hash) > 0)
        {
            new_node->next = node;
            new_node->prev = node->prev;
            if (node->prev != NULL)
            {
                node->prev->next = new_node;
            }
            else
            {
                hashmap->head = new_node;
            }
            node->prev = new_node;
        }
        else
        {
            new_node->prev = node;
            node->next = new_node;
        }
    }
}

data_t hashmap_get(struct hashmap_t* hashmap, hash_t hash, bool remove)
{
    struct hashmap_node_t* node = hashmap->head;
    while (node != NULL) {
        if (hashmap->compare_function(node->hash, hash) == 0)  /*node with this data existed, add to DLL (datanode_t)*/
        {
            struct datanode_t* datanode = node->head;
            data_t toReturn = datanode->data;
            if (remove)
            {
                if (datanode->next == NULL) /*remove last element from doubly linked list*/
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

void hashmap_print(struct hashmap_t* hashmap, void (*print_function)(data_t) )
{
    struct hashmap_node_t* node = hashmap->head;
    int iter = 0;
    while (node != NULL)
    {
        printf("[%d] hash: %llu\n", iter, node->hash);
        struct datanode_t* datanode = node->head;
        while (datanode != NULL)
        {

            printf("\tdata: ");
            print_function(datanode->data);
            printf("\n");
            datanode = datanode->next;
        }
        node = node->next;
        iter++;
    }
    printf("\n");
}
