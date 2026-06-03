#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

/*
 * YENİ TRIE DÜĞÜMÜ OLUŞTURMA
 */
TrieNode* Boran_createTrieNode() {
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
void Boran_insertToTrie(TrieNode* root, const char* word, Node* gNode) {
    if (root == NULL || word == NULL || gNode == NULL) return;
    
    TrieNode* current = root;
    int length = strlen(word);

    for (int i = 0; i < length; i++) {
        int index = tolower((unsigned char)word[i]);

        if (index >= 0 && index < ALPHABET_SIZE) {
            if (current->children[index] == NULL) {
                current->children[index] = Boran_createTrieNode();
            }
            current = current->children[index];
        } else {
            continue; 
        }
    }

    current->isEndOfWord = true;

    NodeList* newList = (NodeList*)malloc(sizeof(NodeList));
    newList->graphNode = gNode;
    newList->next = current->matchingNodes;
    current->matchingNodes = newList;
}

/*
 * DFS İLE KELİME TAMAMLAMA YARDIMCISI (Dahili Fonksiyon)
 */
void Boran_findWordsWithPrefix(TrieNode* root, char* buffer, int level) {
    if (root == NULL) return;

    if (root->isEndOfWord) {
        buffer[level] = '\0'; 

        NodeList* temp = root->matchingNodes;
        while (temp != NULL) {
            printf(" -> %s (Dugum Bellek Adresi: %p)\n", buffer, (void*)temp->graphNode);
            temp = temp->next;
        }
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            buffer[level] = i; 
            Boran_findWordsWithPrefix(root->children[i], buffer, level + 1); 
        }
    }
}

/*
 * OTOMATİK TAMAMLAMA ANA FONKSİYONU
 */
void Boran_autocomplete(TrieNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL) return;

    TrieNode* current = root;
    int length = strlen(prefix);

    for (int i = 0; i < length; i++) {
        int index = tolower((unsigned char)prefix[i]);
        
        if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == NULL) {
            printf("\n[%s] ile baslayan bir kayit bulunamadi.\n", prefix);
            return;
        }
        current = current->children[index];
    }

    printf("\n[%s] icin otomatik tamamlama sonuclari:\n", prefix);

    char buffer[256];
    for(int i = 0; i < length; i++) {
        buffer[i] = tolower((unsigned char)prefix[i]); 
    }

    Boran_findWordsWithPrefix(current, buffer, length);
}

/*
 * BELLEK TEMİZLEME YARDIMCI FONKSİYONU
 */
void Boran_freeNodeList(NodeList* head) {
    while (head != NULL) {
        NodeList* temp = head;
        head = head->next;
        free(temp);
    }
}

/*
 * AĞAÇ SİLME (GARBAGE COLLECTION)
 */
void Boran_freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            Boran_freeTrie(root->children[i]);
        }
    }
    Boran_freeNodeList(root->matchingNodes); 
    free(root);                                
}

Node* Boran_findFirstNode(TrieNode* root, const char* word)
{
    if (root == NULL || word == NULL)
        return NULL;

    TrieNode* current = root;

    for (int i = 0; word[i] != '\0'; i++)
    {
        int index = tolower((unsigned char)word[i]);

        if (index < 0 ||
            index >= ALPHABET_SIZE ||
            current->children[index] == NULL)
        {
            return NULL;
        }

        current = current->children[index];
    }

    if (current->isEndOfWord &&
        current->matchingNodes != NULL)
    {
        return current->matchingNodes->graphNode;
    }

    return NULL;
}