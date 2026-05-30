#ifndef DATA_GENERATOR_H
#define DATA_GENERATOR_H

// Bu modülün graf, hash table ve trie yapılarını tanıması için gerekli başlık dosyaları
#include "graph_adj.h"
#include "hash_table.h"
#include "trie.h"

// data_generator.c dosyasında yazdığımız ana fonksiyonun imzası (prototipi)
void init_synthetic_data(Graph* net, HashTable* ht, TrieNode* trie_root, 
                         int user_count, int photo_count, int event_count);

#endif // DATA_GENERATOR_H