#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "trie.h"

/*
 * YENİ TRIE DÜĞÜMÜ OLUŞTURMA
 * Bellekten (Heap) yeni bir düğüm için yer ayırır ve varsayılan değerleri atar.
 * Zaman Karmaşıklığı: O(1) - (Sabit sayıdaki alfabe boyutu kadar döngü çalışır)
 */
TrieNode* createTrieNode() {
    TrieNode* newNode = (TrieNode*)malloc(sizeof(TrieNode));
    if (newNode) {
        newNode->isEndOfWord = false;
        newNode->matchingNodes = NULL;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            newNode->children[i] = NULL; // Başlangıçta hiçbir çocuk düğüm yok (NULL)
        }
    }
    return newNode;
}

/*
 * TRIE AĞACINA VERİ EKLEME (INDEXING)
 * Yeni bir sosyal ağ düğümü oluşturulduğunda onu ismine göre ağaca yerleştirir.
 * Zaman Karmaşıklığı: O(L) - L: Eklenen kelimenin uzunluğu
 */
void insertToTrie(TrieNode* root, const char* word, Node* gNode) {
    TrieNode* current = root;
    int length = strlen(word);

    // Kelimeyi harf harf parçalayıp ağaçta aşağı doğru iniyoruz
    for (int i = 0; i < length; i++) {
        // Kullanıcı deneyimi (UX) için büyük/küçük harf duyarsız arama (Case-Insensitive) altyapısı:
        // Tüm karakterler ağaca küçük harf olarak eklenir.
        int index = tolower(word[i]); 
        
        // Eğer o harf için bir yol yoksa, yeni bir düğüm oluşturarak yolu aç
        if (current->children[index] == NULL) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    
    // Kelimenin son harfine ulaştık, burayı kelime bitişi olarak işaretle
    current->isEndOfWord = true;
    
    // Graf düğümünü (Pointer), bu harfin bağlı listesinin BAŞINA O(1) süresinde ekliyoruz
    NodeList* newList = (NodeList*)malloc(sizeof(NodeList));
    newList->graphNode = gNode;
    newList->next = current->matchingNodes;
    current->matchingNodes = newList;
}

/*
 * DFS (DERİNLİK ÖNCELİKLİ ARAMA) İLE KELİME TAMAMLAMA YARDIMCISI
 * Verilen bir önekten (prefix) başlayarak ağacın en derinlerine kadar inip olası tüm kelimeleri bulur.
 * Bu, Recursive (Özyineli) bir fonksiyondur.
 */
void findWordsWithPrefix(TrieNode* root, char* buffer, int level) {
    if (root == NULL) return;

    // Eğer geçerli bir ismin bittiği düğüme geldiysek, sonuçları ekrana bas
    if (root->isEndOfWord) {
        buffer[level] = '\0'; // C dilinde string'in bittiğini sisteme bildirir
        
        // Bu isme sahip tüm graf düğümlerinin bellek adreslerini yazdır
        // Not: Gerçek sistemde burada ID veya username yazdırılabilir
        printf(" -> %s (Dugum Bellek Adresi: %p)\n", buffer, (void*)root->matchingNodes->graphNode);
    }

    // Alfabedeki tüm karakterleri kontrol ederek ağacın alt dallarına (çocuklarına) in
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            buffer[level] = i; // Karakteri geçici buffer'a ekle
            findWordsWithPrefix(root->children[i], buffer, level + 1); // Bir alt seviyeye in
        }
    }
}

/*
 * OTOMATİK TAMAMLAMA ANA FONKSİYONU
 * Kullanıcının girdiği öneki (Örn: "bor") ağaçta bulur ve oradan itibaren DFS başlatır.
 * Zaman Karmaşıklığı: O(P + V) - P: Önek uzunluğu, V: Önek altındaki toplam harf/düğüm sayısı
 */
void autocomplete(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    int length = strlen(prefix);
    
    // 1. Aşama: Kullanıcının girdiği kelime kısmının bittiği düğüme kadar in
    for (int i = 0; i < length; i++) {
        int index = tolower(prefix[i]);
        if (current->children[index] == NULL) {
            printf("\n[%s] ile baslayan bir kayit bulunamadi.\n", prefix);
            return;
        }
        current = current->children[index];
    }

    printf("\n[%s] icin otomatik tamamlama sonuclari:\n", prefix);
    
    // 2. Aşama: Öneki tutan bir buffer oluştur ve DFS aramasını başlat
    char buffer[256];
    for(int i = 0; i < length; i++) {
        buffer[i] = tolower(prefix[i]);
    }
    
    findWordsWithPrefix(current, buffer, length);
}

/*
 * BELLEK TEMİZLEME YARDIMCI FONKSİYONU
 * Trie yapraklarındaki bağlı listeleri bellekten siler (Memory Leak önleme)
 */
void freeNodeList(NodeList* head) {
    while (head != NULL) {
        NodeList* temp = head;
        head = head->next;
        free(temp);
    }
}

/*
 * AĞAÇ SİLME (GARBAGE COLLECTION / CLEANUP)
 * Post-order traversal (Önce çocuklar, sonra ebeveyn) mantığıyla tüm ağacı bellekten siler.
 * Dinamik bellek (malloc) kullanan her C programında program kapanmadan önce yapılması zorunludur.
 */
void freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i] != NULL) {
            freeTrie(root->children[i]);
        }
    }
    freeNodeList(root->matchingNodes); // Önce bağlı listeyi sil
    free(root);                        // Sonra kendisini sil
}