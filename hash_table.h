#ifndef HASH_TABLE_H_INCLUDED
#define HASH_TABLE_H_INCLUDED

#include "graph_models.h" // 1. kiþinin tanýmladýðý Node yapýsýný kullanýyoruz

#define HASH_TABLE_SIZE 100

// Karma Tablo Girdisi (Separate Chaining için Baðlý Liste Düðümü)
typedef struct HashEntry {
    int key;                  // Arama anahtarý (Düðüm ID'si)
    Node* node;               // 1. kiþinin Node yapýsýna iþaretçi
    struct HashEntry* next;   // Çarpýþma durumunda bir sonraki girdi
} HashEntry;

// Karma Tablo (Hash Table) Tanýmý
typedef struct HashTable {
    int size;           // Tablo kapasitesi
    HashEntry** table;  // HashEntry iþaretçilerini tutan dizi
} HashTable;

// Fonksiyon Prototipleri
int hash_function(int key, int size);
HashTable* create_hash_table(int size);
void insert_to_hash(HashTable* ht, Node* newNode);
Node* get_from_hash(HashTable* ht, int searchId);
void free_hash_table(HashTable* ht);

#endif // HASH_TABLE_H_INCLUDED
