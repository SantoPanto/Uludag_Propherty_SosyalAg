#ifndef UI_INTEGRATION_H
#define UI_INTEGRATION_H

#include "graph_models.h"
#include "hash_table.h"
#include "trie.h"

// Boran: Yan panelde (Side Panel) gosterilmek uzere dugum detaylarini ceken fonksiyon
Node* Boran_get_node_details_for_ui(HashTable* ht, int node_id);

// Boran: Arama cubuguna (Search Bar) yazilan metinler icin otomatik tamamlama tetikleyicisi
void Boran_get_autocomplete_results_for_ui(TrieNode* root, const char* prefix);

#endif // UI_INTEGRATION_H