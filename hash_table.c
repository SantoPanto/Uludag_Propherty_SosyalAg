#include <stdlib.h>
#include "hash_table.h"

int hash_function(int key, int size) {
    if (size <= 0) {
        return 0;
    }

    int index = key % size;
    if (index < 0) {
        index += size;
    }
    return index;
}

HashTable* create_hash_table(int size) {
    if (size <= 0) {
        size = 16;
    }

    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }

    ht->size = size;
    ht->count = 0;
    ht->table = (HashEntry**)calloc((size_t)size, sizeof(HashEntry*));

    if (ht->table == NULL) {
        free(ht);
        return NULL;
    }

    return ht;
}

static void insert_entry(HashTable* ht, Node* newNode) {
    int index = hash_function(newNode->id, ht->size);

    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    if (entry == NULL) {
        return;
    }

    entry->key = newNode->id;
    entry->node = newNode;
    entry->next = ht->table[index];
    ht->table[index] = entry;
    ht->count++;
}

void rehash_table(HashTable* ht) {
    if (ht == NULL || ht->table == NULL) {
        return;
    }

    int old_size = ht->size;
    HashEntry** old_table = ht->table;
    int new_size = old_size > 0 ? old_size * 2 : 16;

    HashEntry** new_table = (HashEntry**)calloc((size_t)new_size, sizeof(HashEntry*));
    if (new_table == NULL) {
        return;
    }

    ht->table = new_table;
    ht->size = new_size;
    ht->count = 0;

    for (int i = 0; i < old_size; i++) {
        HashEntry* current = old_table[i];
        while (current != NULL) {
            HashEntry* next = current->next;
            int index = hash_function(current->key, ht->size);
            current->next = ht->table[index];
            ht->table[index] = current;
            ht->count++;
            current = next;
        }
    }

    free(old_table);
}

void insert_to_hash(HashTable* ht, Node* newNode) {
    if (ht == NULL || newNode == NULL) {
        return;
    }

    if ((ht->count + 1) * 4 > ht->size * 3) {
        rehash_table(ht);
    }

    insert_entry(ht, newNode);
}

Node* get_from_hash(HashTable* ht, int searchId) {
    if (ht == NULL || ht->table == NULL || ht->size <= 0) {
        return NULL;
    }

    int index = hash_function(searchId, ht->size);
    HashEntry* current = ht->table[index];

    while (current != NULL) {
        if (current->key == searchId) {
            return current->node;
        }
        current = current->next;
    }

    return NULL;
}

void free_hash_table(HashTable* ht) {
    if (ht == NULL) {
        return;
    }

    if (ht->table != NULL) {
        for (int i = 0; i < ht->size; i++) {
            HashEntry* current = ht->table[i];
            while (current != NULL) {
                HashEntry* next = current->next;
                free(current);
                current = next;
            }
        }
        free(ht->table);
    }

    free(ht);
}