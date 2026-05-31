#include <stdio.h>
#include "ui_integration.h"

// Boran: Arayuzden (UI) bir dugume tiklandiginda cagrilacak fonksiyon.
// Faz 1'deki Hash Table'i kullanarak dugum detaylarini O(1) hizinda dondurur.
Node* Boran_get_node_details_for_ui(HashTable* ht, int node_id) {
    if (ht == NULL) return NULL;
    
    // Tiklanan dugumun IDsini hash tablosunda bul
    Node* clicked_node = get_from_hash(ht, node_id);
    
    if (clicked_node != NULL) {
        printf("[UI PANEL] %d ID'li dugum bilgileri arayuze gonderiliyor...\n", node_id);
    } else {
        printf("[UI PANEL HATA] %d ID'li dugum bulunamadi!\n", node_id);
    }
    
    return clicked_node;
}

// Boran: Kullanici arama cubuguna metin girdiginde cagrilacak fonksiyon.
// Faz 1'deki Trie (Onek Agaci) yapisini tetikleyerek UI'a sonuc uretir.
void Boran_get_autocomplete_results_for_ui(TrieNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL) return;
    
    printf("[UI SEARCH BAR] '%s' icin arama sonuclari getiriliyor:\n", prefix);
    
    // Faz 1'deki orijinal arama fonksiyonunu cagir
    autocomplete(root, prefix);
}