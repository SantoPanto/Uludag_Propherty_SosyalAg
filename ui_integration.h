#ifndef UI_INTEGRATION_H
#define UI_INTEGRATION_H

#include "graph_adj.h" // Node yap�s�n� tan�mas� i�in gerekli

// Boran'�n aray�z �izim fonksiyonunun prototipi
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height);

// Yard�mc� metin formatlama fonksiyonunun prototipi
void Boran_format_side_panel_text(Node* node, char* buffer, int max_len);

#endif // UI_INTEGRATION_H
