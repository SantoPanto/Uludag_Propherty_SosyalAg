#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "raylib.h"
#include "graph_models.h"
#include "graph_adj.h"
#include "trie.h"

extern Camera2D camera;

void init_graphics_window(void);
void close_graphics_window(void);
void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer);

#endif // UI_RENDER_H
