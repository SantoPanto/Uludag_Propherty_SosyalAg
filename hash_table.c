<<<<<<< HEAD
#include <stdio.h>
=======
>>>>>>> main
#include <stdlib.h>
#include "hash_table.h"

int hash_function(int key, int size) {
<<<<<<< HEAD
    return key % size;
}

HashTable* create_hash_table(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    ht->size = size;
    ht->table = (HashEntry**)malloc(sizeof(HashEntry*) * size);

    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

void insert_to_hash(HashTable* ht, Node* newNode) {
    if (ht == NULL || newNode == NULL) return;

    // 1. Yeni eklenecek düğümün hangi indekse (zincire) gideceğini bul
    int hashIndex = hash_function(newNode->id, ht->size);

    // --- EKLENEN MÜKERRER KAYIT KONTROLÜ ---
    // 2. Bellekte yer ayırmadan ÖNCE bu zinciri dolaş ve ID'yi ara
    HashEntry* current = ht->table[hashIndex];
    while (current != NULL) {
        if (current->key == newNode->id) {
            // Düğüm zaten hash tablosunda var! Hiçbir şey yapmadan çık.
            // (İstersen buraya printf ile "Uyarı: Düğüm zaten var" yazabilirsin)
            return; 
        }
        current = current->next;
    }
    // ---------------------------------------

    // 3. Eğer buraya kadar geldiysek düğüm tabloda YOKTUR.
    // Artık güvenle yeni HashEntry için bellekte yer (malloc) ayırabiliriz.
    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    newEntry->key = newNode->id;
    newEntry->node = newNode; // Ortak Node yapısı
    newEntry->next = NULL;

    // 4. Yeni elemanı zincirin başına ekle (Insert at Head)
    if (ht->table[hashIndex] == NULL) {
        ht->table[hashIndex] = newEntry;
    } else {
        newEntry->next = ht->table[hashIndex];
        ht->table[hashIndex] = newEntry;
    }
}

Node* get_from_hash(HashTable* ht, int searchId) {
    if (ht == NULL) return NULL;

    int hashIndex = hash_function(searchId, ht->size);
    HashEntry* entry = ht->table[hashIndex];

    while (entry != NULL) {
        if (entry->key == searchId) {
            return entry->node;
        }
        entry = entry->next;
    }
=======
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

>>>>>>> main
    return NULL;
}

void free_hash_table(HashTable* ht) {
<<<<<<< HEAD
    if (ht == NULL) return;
    for (int i = 0; i < ht->size; i++) {
        HashEntry* entry = ht->table[i];
        while (entry != NULL) {
            HashEntry* temp = entry;
            entry = entry->next;
            // Sadece Hash kapsüllerini siliyoruz, asil dügümü free_graph silecek
            free(temp);
        }
    }
    free(ht->table);
=======
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

>>>>>>> main
    free(ht);
}