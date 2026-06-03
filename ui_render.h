#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "raylib.h"
#include "graph_models.h"
#include "ui_integration.h" 
#include "trie.h"

// Boran: Ana ekranda sag tarafa UI panelini ve arama cubugunu cizer
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height);

// Arayüzü çizen ana fonksiyon prototipi
void draw_graph_network(Graph* graph, TrieNode* trie_root);

#endif // UI_RENDER_H