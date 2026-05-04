#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

#include "graph_models.h" // 1. kişinin tanımladığı Node yapısını kullanıyoruz


// Karma Tablo Girdisi (Separate Chaining için Bağlı Liste Düğümü)
typedef struct HashEntry {
    int key;                  // Arama anahtarı (Düğüm ID'si)
    Node* node;               // 1. kişinin Node yapısına işaretçi
    struct HashEntry* next;   // Çarpışma durumunda bir sonraki girdi
} HashEntry;

// Karma Tablo (Hash Table) Tanımı
typedef struct HashTable {
    int size;           // Tablo kapasitesi
    HashEntry** table;  // HashEntry işaretçilerini tutan dizi
} HashTable;

// Fonksiyon Prototipleri
int hash_function(int key, int size);
HashTable* create_hash_table(int size);
void insert_to_hash(HashTable* ht, Node* newNode);
Node* get_from_hash(HashTable* ht, int searchId);
void free_hash_table(HashTable* ht);

#endif // HASH_TABLE_H_INCLUDED