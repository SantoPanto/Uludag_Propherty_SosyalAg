#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <ctype.h>        // YENİ EKLENDİ: tolower() fonksiyonu için gerekli
#include "graph_models.h" // Graf düğümlerini (Node) Trie ile eşleştirmek için içeri aktarıyoruz

// Standart ASCII karakterleri kapsamak için 128 boyutunda bir dizi kullanıyoruz.
#define ALPHABET_SIZE 128

typedef struct NodeList {
    Node* graphNode;       // 1. Kişinin tasarladığı asıl graf düğümünün bellek adresi
    struct NodeList* next; // Sonraki aynı isimli düğüme işaretçi
} NodeList;

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE]; 
    bool isEndOfWord;                         
    NodeList* matchingNodes;                  
} TrieNode;

// Boran: Dışarıdan erişilebilecek fonksiyon prototipleri (API İmzaları)
TrieNode* Boran_createTrieNode();
void Boran_insertToTrie(TrieNode* root, const char* word, Node* gNode);
void Boran_autocomplete(TrieNode* root, const char* prefix);

Node* Boran_findFirstNode(TrieNode* root, const char* word);

void Boran_freeTrie(TrieNode* root);

#endif // TRIE_H