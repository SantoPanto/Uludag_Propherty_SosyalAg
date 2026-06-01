#ifndef UI_INTEGRATION_H
#define UI_INTEGRATION_H

#include "graph_adj.h" // Node yapýsýný tanýmasý için gerekli

// Boran'ýn arayüz çizim fonksiyonunun prototipi
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height);

// Yardýmcý metin formatlama fonksiyonunun prototipi
void Boran_format_side_panel_text(Node* node, char* buffer, int max_len);

#endif // UI_INTEGRATION_H
