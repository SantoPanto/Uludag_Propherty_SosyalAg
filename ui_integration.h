#ifndef UI_INTEGRATION_H
#define UI_INTEGRATION_H

#include "graph_models.h"
#include "graph_adj.h"
#include "trie.h"

// BU SATIRI EKLEYİN:
void Boran_turkish_to_ascii(char *dest, const char *src);

void Boran_draw_ui_panel(Graph* graph, Node* selected_node, char* search_text_buffer,
                         int screen_width, int screen_height);
void Boran_format_side_panel_text(Graph* graph, Node* node, char* buffer, int max_len);

#endif // UI_INTEGRATION_H