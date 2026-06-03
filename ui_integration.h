#ifndef UI_INTEGRATION_H
#define UI_INTEGRATION_H

#include "graph_models.h"
#include "graph_adj.h"
#include "trie.h"

// Arayüz çizim fonksiyonlarının prototipleri
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height);
void Boran_format_side_panel_text(Node* node, char* buffer, int max_len);

// Render fonksiyonunun prototipi
void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer);

#endif // UI_INTEGRATION_H