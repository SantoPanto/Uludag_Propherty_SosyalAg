#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

/*
 * YENÝ TRIE DÜÐÜMÜ OLUÞTURMA
 * Bellekten (Heap) yeni bir düðüm için yer ayýrýr ve varsayýlan deðerleri atar.
 * Zaman Karmaþýklýðý: O(1) - (Sabit sayýdaki alfabe boyutu kadar döngü çalýþýr)
 */
TrieNode* createTrieNode() {
    TrieNode* newNode = (TrieNode*)malloc(sizeof(TrieNode));
    if (newNode) {
        newNode->isEndOfWord = false;
        newNode->matchingNodes = NULL;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            newNode->children[i] = NULL; // Baþlangýçta hiçbir çocuk düðüm yok (NULL)
        }
    }
    return newNode;
}

/*
 * TRIE AÐACINA VERÝ EKLEME (INDEXING)
 * Yeni bir sosyal að düðümü oluþturulduðunda onu ismine göre aðaca yerleþtirir.
 * Zaman Karmaþýklýðý: O(L) - L: Eklenen kelimenin uzunluðu
 */
void insertToTrie(TrieNode* root, const char* word, Node* gNode) {
    TrieNode* current = root;
    int length = strlen(word);

    // Kelimeyi harf harf parçalayýp aðaçta aþaðý doðru iniyoruz
    for (int i = 0; i < length; i++) {
        // Kullanýcý deneyimi (UX) için büyük/küçük harf duyarsýz arama (Case-Insensitive) altyapýsý:
        // Tüm karakterler aðaca küçük harf olarak eklenir.
        int index = tolower(word[i]);

        // Eðer o harf için bir yol yoksa, yeni bir düðüm oluþturarak yolu aç
        if (current->children[index] == NULL) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }

    // Kelimenin son harfine ulaþtýk, burayý kelime bitiþi olarak iþaretle
    current->isEndOfWord = true;

    // Graf düðümünü (Pointer), bu harfin baðlý listesinin BAÞINA O(1) süresinde ekliyoruz
    NodeList* newList = (NodeList*)malloc(sizeof(NodeList));
    newList->graphNode = gNode;
    newList->next = current->matchingNodes;
    current->matchingNodes = newList;
}

/*
 * DFS (DERÝNLÝK ÖNCELÝKLÝ ARAMA) ÝLE KELÝME TAMAMLAMA YARDIMCISI
 * Verilen bir önekten (prefix) baþlayarak aðacýn en derinlerine kadar inip olasý tüm kelimeleri bulur.
 * Bu, Recursive (Özyineli) bir fonksiyondur.
 */
void findWordsWithPrefix(TrieNode* root, char* buffer, int level) {
    if (root == NULL) return;

    // Eðer geçerli bir ismin bittiði düðüme geldiysek, sonuçlarý ekrana bas
    if (root->isEndOfWord) {
        buffer[level] = '\0'; // C dilinde string'in bittiðini sisteme bildirir

        // Bu isme sahip tüm graf düðümlerinin bellek adreslerini yazdýr
        // Not: Gerçek sistemde burada ID veya username yazdýrýlabilir
        printf(" -> %s (Dugum Bellek Adresi: %p)\n", buffer, (void*)root->matchingNodes->graphNode);
    }

    // Alfabedeki tüm karakterleri kontrol ederek aðacýn alt dallarýna (çocuklarýna) in
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            buffer[level] = i; // Karakteri geçici buffer'a ekle
            findWordsWithPrefix(root->children[i], buffer, level + 1); // Bir alt seviyeye in
        }
    }
}

/*
 * OTOMATÝK TAMAMLAMA ANA FONKSÝYONU
 * Kullanýcýnýn girdiði öneki (Örn: "bor") aðaçta bulur ve oradan itibaren DFS baþlatýr.
 * Zaman Karmaþýklýðý: O(P + V) - P: Önek uzunluðu, V: Önek altýndaki toplam harf/düðüm sayýsý
 */
void autocomplete(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    int length = strlen(prefix);

    // 1. Aþama: Kullanýcýnýn girdiði kelime kýsmýnýn bittiði düðüme kadar in
    for (int i = 0; i < length; i++) {
        int index = tolower(prefix[i]);
        if (current->children[index] == NULL) {
            printf("\n[%s] ile baslayan bir kayit bulunamadi.\n", prefix);
            return;
        }
        current = current->children[index];
    }

    printf("\n[%s] icin otomatik tamamlama sonuclari:\n", prefix);

    // 2. Aþama: Öneki tutan bir buffer oluþtur ve DFS aramasýný baþlat
    char buffer[256];
    for(int i = 0; i < length; i++) {
        buffer[i] = tolower(prefix[i]);
    }

    findWordsWithPrefix(current, buffer, length);
}

/*
 * BELLEK TEMÝZLEME YARDIMCI FONKSÝYONU
 * Trie yapraklarýndaki baðlý listeleri bellekten siler (Memory Leak önleme)
 */
void freeNodeList(NodeList* head) {
    while (head != NULL) {
        NodeList* temp = head;
        head = head->next;
        free(temp);
    }
}

/*
 * AÐAÇ SÝLME (GARBAGE COLLECTION / CLEANUP)
 * Post-order traversal (Önce çocuklar, sonra ebeveyn) mantýðýyla tüm aðacý bellekten siler.
 * Dinamik bellek (malloc) kullanan her C programýnda program kapanmadan önce yapýlmasý zorunludur.
 */
void freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            freeTrie(root->children[i]);
        }
    }
    freeNodeList(root->matchingNodes); // Önce baðlý listeyi sil
    free(root);                        // Sonra kendisini sil
}
