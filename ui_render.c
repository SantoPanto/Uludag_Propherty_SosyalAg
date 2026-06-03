#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "graph_adj.h"
#include "ui_render.h"
#include "ui_integration.h"
#include "trie.h"
#include "raygui.h"

Camera2D camera = { 0 };
Font guiFont = { 0 };

// --- KARANLIK TEMA (DARK MODE) RENK PALETİ ---
static Color node_color_for_type(NodeType type) {
    switch (type) {
        case USER: return (Color){ 64, 196, 255, 255 };   // Canlı Mavi (Cyan)
        case PHOTO: return (Color){ 105, 240, 174, 255 }; // Neon Yeşil
        case EVENT: return (Color){ 178, 113, 255, 255 }; // Parlak Mor
        default: return LIGHTGRAY;
    }
}

static Color edge_color_for_type(EdgeType type) {
    switch (type) {
        case FRIEND: return (Color){ 150, 150, 150, 180 }; // Açık Gri
        case LIKES: return (Color){ 255, 82, 82, 180 };    // Canlı Kırmızı
        case ATTENDS: return (Color){ 255, 171, 64, 180 }; // Turuncu
        case HAS_PHOTO: return (Color){ 24, 255, 255, 180};// Turkuaz
        default: return DARKGRAY;
    }
}

void Boran_focus_on_node(Node* node) {
    if (node == NULL) return;
    camera.target = (Vector2){ node->x, node->y };
    camera.zoom = 1.8f;
}

// Yegane ve Temizlenmiş init_graphics_window fonksiyonu
void init_graphics_window(void) {
    // --- 1. PERFORMANS VE YÜKSEK ÇÖZÜNÜRLÜK AYARLARI ---
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 720, "Property Graph - Sosyal Ag");
    SetTargetFPS(60);

    // --- 2. RAYGUI KARANLIK TEMA (DARK MODE) AYARLARI ---
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
    
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1E2126FF);  // Ana arka plan (En Koyu)
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x2A2E35FF); // Butonların normal rengi
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x444B57FF);// Buton Hover rengi
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x15181CFF);// Buton tıklanma rengi
    
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x4A5365FF);        // Arama kutusu vb. ince kenarlıklar
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xE6E6E6FF); // Varsayılan yazı (Açık Gri)
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);// Hover yazısı (Tam Beyaz)

    // --- 3. KAMERA AYARLARI ---
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 640.0f, 360.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.85f;
}

void close_graphics_window(void) {
    CloseWindow();
}

void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer) {
    // --- Kamera Kontrolleri ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        camera.zoom += (wheel * 0.12f);
        if (camera.zoom < 0.08f) camera.zoom = 0.08f;
        if (camera.zoom > 8.0f) camera.zoom = 8.0f;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    BeginDrawing();
    
    // YENİ ARKA PLAN: Koyu Antrasit
    ClearBackground((Color){ 34, 39, 46, 255 }); 

    BeginMode2D(camera);

    // 1. AŞAMA: KENARLARI (EDGES) ÇİZ
    for (int i = 0; i < graph->node_count; i++) {
        Node* src_node = graph->nodes[i];
        int src_idx = i;
        AdjListNode* adj = graph->adjLists[src_idx];
        
        while (adj != NULL) {
            if (adj->edge != NULL) {
                int target_idx = find_node_index(graph, adj->edge->target_id);
                if (target_idx != -1 && src_idx < target_idx) {
                    Node* target_node = graph->nodes[target_idx];
                    
                    bool is_src_selected = (*selected_node != NULL && (*selected_node)->id == src_node->id);
                    bool is_target_selected = (*selected_node != NULL && (*selected_node)->id == target_node->id);
                    bool any_node_selected = (*selected_node != NULL);

                    float line_thick = 2.0f;
                    Color edge_color = edge_color_for_type(adj->edge->type);

                    if (any_node_selected) {
                        if (is_src_selected || is_target_selected) {
                            line_thick = 4.0f; 
                            edge_color.a = 255; 
                        } else {
                            line_thick = 1.0f;
                            edge_color.a = 40; 
                        }
                    }

                    if (line_thick > 2.0f) {
                        DrawLineEx((Vector2){ src_node->x, src_node->y },
                                   (Vector2){ target_node->x, target_node->y },
                                   line_thick, edge_color);
                    } else {
                        DrawLineV((Vector2){ src_node->x, src_node->y },
                                  (Vector2){ target_node->x, target_node->y },
                                  edge_color);
                    }
                }
            }
            adj = adj->next;
        }
    }

    // 2. AŞAMA: DÜĞÜMLERİ (NODES) ÇİZ
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        bool is_selected = (*selected_node != NULL && (*selected_node)->id == n->id);
        
        Vector2 center = { n->x, n->y };
        
        float base_size = 14.0f; 
        if (is_selected) {
            base_size = 22.0f; 
        }
        
        Color fill_color = node_color_for_type(n->type);
        Color border_color = is_selected ? (Color){ 255, 235, 59, 255 } : (Color){ 20, 24, 30, 255 };
        float border_thickness = is_selected ? 5.0f : 2.5f;

        if (n->type == PHOTO) {
            float s = base_size * 2.0f;
            Rectangle border_rec = { n->x - (s/2) - border_thickness/2, n->y - (s/2) - border_thickness/2, s + border_thickness, s + border_thickness };
            Rectangle fill_rec = { n->x - s/2, n->y - s/2, s, s };
            
            DrawRectangleRec(border_rec, border_color);
            DrawRectangleRec(fill_rec, fill_color);
        } 
        else if (n->type == EVENT) {
            DrawPoly(center, 3, base_size + border_thickness, 0, border_color);
            DrawPoly(center, 3, base_size, 0, fill_color);
        } 
        else {
            DrawCircleV(center, base_size + border_thickness/2, border_color);
            DrawCircleV(center, base_size, fill_color);
        }

        // 3. AŞAMA: ETİKETLERİ ÇİZ
        if (camera.zoom > 0.6f) {
            char label[64];
            node_get_display_label(n, label, sizeof(label));
            
            int font_size = 10;
            int text_width = MeasureText(label, font_size);
            int text_x = (int)n->x - (text_width / 2);
            int text_y = (int)n->y + (int)base_size + 8;

            DrawRectangle(text_x - 4, text_y - 2, text_width + 8, font_size + 4, Fade(BLACK, 0.7f));
            DrawText(label, text_x, text_y, font_size, RAYWHITE);
        }
    }

    // --- Tıklama (Hitbox) Algılaması ---
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
        bool hit = false;
        for (int i = graph->node_count - 1; i >= 0; i--) {
            Node* n = graph->nodes[i];
            
            bool is_currently_selected = (*selected_node != NULL && (*selected_node)->id == n->id);
            float hit_radius = is_currently_selected ? 24.0f : 16.0f; 
            
            if (CheckCollisionPointCircle(mouse_world, (Vector2){ n->x, n->y }, hit_radius)) {
                *selected_node = n;
                hit = true;
                break;
            }
        }
        
        if (!hit) {
            int sw = GetScreenWidth();
            if (GetMouseX() < sw - 360) {
                *selected_node = NULL;
            }
        }
    }

    EndMode2D();

    // UI Paneli Çizimi
    Boran_draw_ui_panel(
        graph,
        *selected_node,
        search_text_buffer,
        GetScreenWidth(),
        GetScreenHeight()
    );

    // --- Arama Mantığı ---
    if (IsKeyPressed(KEY_ENTER) && search_text_buffer != NULL && strlen(search_text_buffer) > 0) {
        char clean_search_text[64] = {0};
        extern void Boran_turkish_to_ascii(char *dest, const char *src);
        Boran_turkish_to_ascii(clean_search_text, search_text_buffer);

        Node* found = Boran_findFirstNode(trie_root, clean_search_text);
        if (found != NULL) {
            *selected_node = found;
            Boran_focus_on_node(found);
        }
    }

    // --- Sol Üst Bilgi Kutusu (Karanlık Tema) ---
    DrawRectangle(8, 8, 200, 52, Fade(BLACK, 0.6f));
    DrawRectangleLines(8, 8, 200, 52, DARKGRAY);
    DrawFPS(16, 14);
    DrawText(TextFormat("Toplam Dugum: %d", graph->node_count), 16, 38, 14, RAYWHITE);

    EndDrawing();
}