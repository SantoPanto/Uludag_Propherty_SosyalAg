#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "raylib.h"
#include "graph_models.h"
#include "ui_integration.h" 

// Boran: Ana ekranda sag tarafa UI panelini ve arama cubugunu cizer
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height);

#endif // UI_RENDER_H