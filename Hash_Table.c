#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 100 // Küçük ve orta ölçekli graflar için başlangıç boyutu

// 1. Düğüm Türleri (Heterojen Yapı İçin)
typedef enum {
    NODE_USER,
    NODE_PHOTO,
    NODE_EVENT
} NodeType;

// 2. Graf Düğümü (Vertex) Tanımı
typedef struct GraphNode {
    int id;             // Benzersiz ID
    NodeType type;      // Tür bilgisi
    void* properties;   // Düğüme özel özellikler (İleride ilgili struct'lara cast edilecek)
    // Not: Komşuluk listesi kenarları (edges) bu yapının içinde veya ayrı bir graf yapısında tutulabilir.
} GraphNode;

// 3. Karma Tablo Girdisi (Separate Chaining için Bağlı Liste Düğümü)
typedef struct HashEntry {
    int key;                  // Arama anahtarı (Düğüm ID'si)
    GraphNode* node;          // Graf düğümüne işaretçi
    struct HashEntry* next;   // Çarpışma durumunda bir sonraki girdi
} HashEntry;

// 4. Karma Tablo (Hash Table) Tanımı
typedef struct HashTable {
    int size;           // Tablo kapasitesi
    HashEntry** table;  // HashEntry işaretçilerini tutan dizi
} HashTable;

// --- FONKSİYONLAR ---

// Basit Modulo Hash Fonksiyonu
int hashFunction(int key, int size) {
    return key % size;
}

// Karma Tabloyu İlk Değerlerle Oluşturma (Initialization)
HashTable* createHashTable(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    ht->size = size;
    ht->table = (HashEntry**)malloc(sizeof(HashEntry*) * size);
    
    for (int i = 0; i < size; i++) {
        ht->table[i] = NULL;
    }
    return ht;
}

// Düğümü Karma Tabloya Ekleme (Insert)
void insertNode(HashTable* ht, GraphNode* newNode) {
    if (ht == NULL || newNode == NULL) return;

    int hashIndex = hashFunction(newNode->id, ht->size);
    
    // Yeni girdi oluştur
    HashEntry* newEntry = (HashEntry*)malloc(sizeof(HashEntry));
    newEntry->key = newNode->id;
    newEntry->node = newNode;
    newEntry->next = NULL;

    // Eğer o index boşsa direkt ekle
    if (ht->table[hashIndex] == NULL) {
        ht->table[hashIndex] = newEntry;
    } else {
        // Çarpışma (Collision) varsa, bağlı listenin başına ekle (O(1) ekleme süresi)
        newEntry->next = ht->table[hashIndex];
        ht->table[hashIndex] = newEntry;
    }
}

// ID'ye Göre Düğüm Arama (Search) -> Ortalama O(1) sürede çalışır
GraphNode* getNode(HashTable* ht, int searchId) {
    if (ht == NULL) return NULL;

    int hashIndex = hashFunction(searchId, ht->size);
    HashEntry* entry = ht->table[hashIndex];

    // İlgili index'teki bağlı listeyi tara
    while (entry != NULL) {
        if (entry->key == searchId) {
            return entry->node; // Düğüm bulundu
        }
        entry = entry->next;
    }
    
    return NULL; // Düğüm bulunamadı
}

// Belleği Temizleme (Memory Management)
void freeHashTable(HashTable* ht) {
    if (ht == NULL) return;
    for (int i = 0; i < ht->size; i++) {
        HashEntry* entry = ht->table[i];
        while (entry != NULL) {
            HashEntry* temp = entry;
            entry = entry->next;
            // Not: GraphNode'un kendi belleği burada veya graf silinirken ayrıca serbest bırakılmalıdır.
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

// Test / Örnek Kullanım
int main() {
    // 1. Tabloyu oluştur
    HashTable* myTable = createHashTable(HASH_TABLE_SIZE);

    // 2. Örnek bir düğüm oluştur
    GraphNode* user1 = (GraphNode*)malloc(sizeof(GraphNode));
    user1->id = 101;
    user1->type = NODE_USER;
    user1->properties = NULL; // Şimdilik boş, ileride struct UserProperties eklenebilir

    // 3. Düğümü hash table'a ekle
    insertNode(myTable, user1);

    // 4. Düğümü ID ile hızlıca O(1) sürede ara
    GraphNode* foundNode = getNode(myTable, 101);
    
    if (foundNode != NULL) {
        printf("Düğüm Bulundu! ID: %d, Tur: %d\n", foundNode->id, foundNode->type);
    } else {
        printf("Düğüm bulunamadı.\n");
    }

    // 5. Belleği temizle
    free(user1);
    freeHashTable(myTable);

    return 0;
}