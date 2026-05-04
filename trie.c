#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

/*
 * YENİ TRIE DÜĞÜMÜ OLUŞTURMA
 */
TrieNode* createTrieNode() {
    TrieNode* newNode = (TrieNode*)malloc(sizeof(TrieNode));
    if (newNode) {
        newNode->isEndOfWord = false;
        newNode->matchingNodes = NULL;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            newNode->children[i] = NULL;
        }
    }
    return newNode;
}

/*
 * TRIE AĞACINA VERİ EKLEME (INDEXING)
 */
void insertToTrie(TrieNode* root, const char* word, Node* gNode) {
    if (root == NULL || word == NULL || gNode == NULL) return;
    
    TrieNode* current = root;
    int length = strlen(word);

    for (int i = 0; i < length; i++) {
        // 1. KRİTİK DÜZELTME: Negatif karakterleri önlemek için (unsigned char) casting
        int index = tolower((unsigned char)word[i]);

        // Karakterin ALPHABET_SIZE (128) sınırları içinde olduğundan emin ol
        if (index >= 0 && index < ALPHABET_SIZE) {
            if (current->children[index] == NULL) {
                current->children[index] = createTrieNode();
            }
            current = current->children[index];
        } else {
            // Desteklenmeyen bir karakter (Örn: Emoji veya UTF-8 aksan) gelirse atla
            continue; 
        }
    }

    current->isEndOfWord = true;

    // Graf düğümünü bağlı listenin BAŞINA ekle
    NodeList* newList = (NodeList*)malloc(sizeof(NodeList));
    newList->graphNode = gNode;
    newList->next = current->matchingNodes;
    current->matchingNodes = newList;
}

/*
 * DFS İLE KELİME TAMAMLAMA YARDIMCISI
 */
void findWordsWithPrefix(TrieNode* root, char* buffer, int level) {
    if (root == NULL) return;

    if (root->isEndOfWord) {
        buffer[level] = '\0'; 

        // 2. KRİTİK DÜZELTME: Aynı isme sahip (Collision) TÜM kullanıcıları döngüyle yazdır
        NodeList* temp = root->matchingNodes;
        while (temp != NULL) {
            printf(" -> %s (Dugum Bellek Adresi: %p)\n", buffer, (void*)temp->graphNode);
            temp = temp->next; // Bir sonraki aynı isimli kişiye geç
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            buffer[level] = i; 
            findWordsWithPrefix(root->children[i], buffer, level + 1); 
        }
    }
}

/*
 * OTOMATİK TAMAMLAMA ANA FONKSİYONU
 */
void autocomplete(TrieNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL) return;

    TrieNode* current = root;
    int length = strlen(prefix);

    for (int i = 0; i < length; i++) {
        // Güvenlik dönüşümü
        int index = tolower((unsigned char)prefix[i]);
        
        // Geçersiz karakter veya bulunamayan düğüm kontrolü
        if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == NULL) {
            printf("\n[%s] ile baslayan bir kayit bulunamadi.\n", prefix);
            return;
        }
        current = current->children[index];
    }

    printf("\n[%s] icin otomatik tamamlama sonuclari:\n", prefix);

    char buffer[256];
    for(int i = 0; i < length; i++) {
        buffer[i] = tolower((unsigned char)prefix[i]); // Buffer'a güvenle ekle
    }

    findWordsWithPrefix(current, buffer, length);
}

/*
 * BELLEK TEMİZLEME YARDIMCI FONKSİYONU
 */
void freeNodeList(NodeList* head) {
    while (head != NULL) {
        NodeList* temp = head;
        head = head->next;
        free(temp);
    }
}

/*
 * AĞAÇ SİLME (GARBAGE COLLECTION)
 */
void freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            freeTrie(root->children[i]);
        }
    }
    freeNodeList(root->matchingNodes); 
    free(root);                        
}