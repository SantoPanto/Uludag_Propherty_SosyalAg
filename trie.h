#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include "graph_models.h" // Graf düðümlerini (Node) Trie ile eþleþtirmek için içeri aktarýyoruz

// Ýngilizce alfabe ve temel ASCII karakterleri kapsamak için 128 boyutunda bir dizi kullanýyoruz
#define ALPHABET_SIZE 128

/*
 * ÝSÝM ÇAKIÞMASI ÇÖZÜMÜ (COLLISION HANDLING):
 * Sosyal aðlarda ayný isme sahip birden fazla kullanýcý olabilir (Örn: iki farklý "Ali" hesabý).
 * Trie aðacýnýn yaprak düðümünde tek bir pointer tutmak yerine bir Baðlý Liste (Linked List)
 * tutarak, ayný isimdeki tüm kullanýcýlarýn bellek adreslerini kaybetmeden saklýyoruz.
 */
typedef struct NodeList {
    Node* graphNode;       // 1. Kiþinin tasarladýðý asýl graf düðümünün bellek adresi
    struct NodeList* next; // Sonraki ayný isimli düðüme iþaretçi
} NodeList;

/*
 * TRIE (ÖNEK AÐACI) DÜÐÜM YAPISI:
 * Her düðüm, alfabedeki karakter sayýsý kadar çocuk barýndýrabilir.
 */
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE]; // Alt karakterlere giden yollar (Branching)
    bool isEndOfWord;                         // Bu düðüm geçerli bir kelimenin son harfi mi?
    NodeList* matchingNodes;                  // Eðer kelimenin sonuysa, bu isme sahip graf düðümleri listesi
} TrieNode;

// Dýþarýdan eriþilebilecek fonksiyon prototipleri (API Ýmzalarý)
TrieNode* createTrieNode();
void insertToTrie(TrieNode* root, const char* word, Node* gNode);
void autocomplete(TrieNode* root, const char* prefix);
void freeTrie(TrieNode* root);

#endif
