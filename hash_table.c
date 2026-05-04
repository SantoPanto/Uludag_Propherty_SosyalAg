#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int hash_function(int key, int size) {
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
    return NULL;
}

void free_hash_table(HashTable* ht) {
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
    free(ht);
}