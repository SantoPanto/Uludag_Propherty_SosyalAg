#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "graph_adj.h"
#include "ui_render.h"
#include "ui_integration.h"
#include "trie.h"
#include "raygui.h"

// --- Global Değişkenler ---
Camera2D camera = { 0 };
Font guiFont = { 0 };
static Node* dragging_node = NULL;
static bool is_selecting = false;
static Vector2 selection_start = { 0 };
static Vector2 selection_end = { 0 };

// --- Yardımcı Renk Fonksiyonları (Karanlık Tema) ---
static Color node_color_for_type(NodeType type) {
    switch (type) {
        case USER: return (Color){ 64, 196, 255, 255 };   // Canlı Mavi
        case PHOTO: return (Color){ 105, 240, 174, 255 }; // Neon Yeşil
        case EVENT: return (Color){ 178, 113, 255, 255 }; // Parlak Mor
        default: return LIGHTGRAY;
    }
}

static Color edge_color_for_type(EdgeType type) {
    switch (type) {
        case FRIEND: return (Color){ 150, 150, 150, 180 };
        case LIKES: return (Color){ 255, 82, 82, 180 };
        case ATTENDS: return (Color){ 255, 171, 64, 180 };
        case HAS_PHOTO: return (Color){ 24, 255, 255, 180 };
        default: return DARKGRAY;
    }
}

void Boran_focus_on_node(Node* node) {
    if (node == NULL) return;
    camera.target = (Vector2){ node->x, node->y };
    camera.zoom = 1.8f;
}

// --- Grafik Penceresi Başlatma ---
void init_graphics_window(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(1280, 720, "Property Graph - Sosyal Ag");
    SetTargetFPS(60);

    // Font Yükleme
    guiFont = LoadFontEx("Roboto-Regular.ttf", 18, 0, 250);
    GuiSetFont(guiFont);

    // Dark Mode Stilleri
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1E2126FF);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0x2A2E35FF);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0x444B57FF);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x15181CFF);
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x4A5365FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xE6E6E6FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);

    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 640.0f, 360.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.85f;
}

void close_graphics_window(void) {
    UnloadFont(guiFont);
    CloseWindow();
}

// --- Ana Çizim Döngüsü ---
void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer) {
    // Kamera Kontrolleri
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

    Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
    int sw = GetScreenWidth();
    bool is_mouse_on_ui = (GetMouseX() > sw - 360);

    // Mouse Etkileşimleri (Yeni Seçim Mantığı Korundu)
    if (!is_mouse_on_ui) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool hit = false;
            for (int i = graph->node_count - 1; i >= 0; i--) {
                Node* n = graph->nodes[i];
                float hit_radius = (n->is_selected || (*selected_node != NULL && (*selected_node)->id == n->id)) ? 24.0f : 16.0f;
                if (CheckCollisionPointCircle(mouse_world, (Vector2){ n->x, n->y }, hit_radius)) {
                    hit = true;
                    dragging_node = n;
                    if (!n->is_selected) {
                        for(int j=0; j < graph->node_count; j++) graph->nodes[j]->is_selected = 0;
                        *selected_node = n;
                        n->is_selected = 1;
                    }
                    break;
                }
            }
            if (!hit) {
                for(int j=0; j < graph->node_count; j++) graph->nodes[j]->is_selected = 0;
                *selected_node = NULL;
                is_selecting = true;
                selection_start = mouse_world;
                selection_end = mouse_world;
            }
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (dragging_node != NULL) {
                Vector2 delta = Vector2Scale(GetMouseDelta(), 1.0f / camera.zoom);
                for (int i = 0; i < graph->node_count; i++) {
                    if (graph->nodes[i]->is_selected) {
                        graph->nodes[i]->x += delta.x;
                        graph->nodes[i]->y += delta.y;
                    }
                }
            } else if (is_selecting) {
                selection_end = mouse_world;
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (is_selecting) {
                Rectangle sel_rect = { fminf(selection_start.x, selection_end.x), fminf(selection_start.y, selection_end.y), 
                                       fabsf(selection_end.x - selection_start.x), fabsf(selection_end.y - selection_start.y) };
                for (int i = 0; i < graph->node_count; i++) {
                    Node* n = graph->nodes[i];
                    if (CheckCollisionPointRec((Vector2){n->x, n->y}, sel_rect)) {
                        n->is_selected = 1;
                        *selected_node = n; // Son seçileni aktif node yap
                    }
                }
                is_selecting = false;
            }
            dragging_node = NULL;
        }
    }

    BeginDrawing();
    ClearBackground((Color){ 34, 39, 46, 255 }); // Koyu Antrasit Arka Plan
    BeginMode2D(camera);

    // Kenarları Çiz
    for (int i = 0; i < graph->node_count; i++) {
        Node* src = graph->nodes[i];
        AdjListNode* adj = graph->adjLists[i];
        while (adj != NULL) {
            int target_idx = find_node_index(graph, adj->edge->target_id);
            if (target_idx != -1 && i < target_idx) {
                Node* target = graph->nodes[target_idx];
                bool is_src_sel = src->is_selected || (*selected_node != NULL && (*selected_node)->id == src->id);
                bool is_target_sel = target->is_selected || (*selected_node != NULL && (*selected_node)->id == target->id);
                
                float thick = (is_src_sel || is_target_sel) && !is_selecting ? 4.0f : 1.0f;
                Color col = edge_color_for_type(adj->edge->type);
                if (thick == 1.0f && (*selected_node != NULL || is_selecting)) col.a = 40; // Ghosting efekti
                
                DrawLineEx((Vector2){src->x, src->y}, (Vector2){target->x, target->y}, thick, col);
            }
            adj = adj->next;
        }
    }

    // Düğümleri Çiz
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        bool is_node_sel = n->is_selected || (*selected_node != NULL && (*selected_node)->id == n->id);
        float size = is_node_sel ? 22.0f : 14.0f;
        Color fill = node_color_for_type(n->type);
        Color border = is_node_sel ? (Color){ 255, 235, 59, 255 } : (Color){ 20, 24, 30, 255 };
        
        if (n->type == PHOTO) DrawRectangleRec((Rectangle){n->x-size, n->y-size, size*2, size*2}, fill);
        else if (n->type == EVENT) DrawPoly((Vector2){n->x, n->y}, 3, size, 0, fill);
        else DrawCircleV((Vector2){n->x, n->y}, size, fill);
        
        // --- YAZILAR (Cool Kapsül Efekti Geri Döndü) ---
        if (camera.zoom > 0.6f) {
            char label[64]; 
            node_get_display_label(n, label, sizeof(label));
            
            // Yeni font sistemine göre yazının piksel genişliğini ölçüyoruz
            Vector2 sz = MeasureTextEx(guiFont, label, 14, 1);
            
            float text_x = n->x - (sz.x / 2.0f);
            float text_y = n->y + size + 8.0f;

            // Yazıların arkasına yarı saydam siyah kapsül eklendi
            DrawRectangle((int)text_x - 4, (int)text_y - 2, (int)sz.x + 8, (int)sz.y + 4, Fade(BLACK, 0.7f));
            
            // Yazıyı yüksek çözünürlüklü çizdir
            DrawTextEx(guiFont, label, (Vector2){text_x, text_y}, 14, 1, RAYWHITE);
        }
    }

    // Seçim Kutusu (Marquee) Çizimi
    if (is_selecting) {
        Rectangle sel_rect = { fminf(selection_start.x, selection_end.x), fminf(selection_start.y, selection_end.y), 
                               fabsf(selection_end.x - selection_start.x), fabsf(selection_end.y - selection_start.y) };
        DrawRectangleRec(sel_rect, Fade(SKYBLUE, 0.2f));
        DrawRectangleLinesEx(sel_rect, 2.0f / camera.zoom, (Color){ 64, 196, 255, 255 });
    }

    EndMode2D();
    
    // Sağ Paneli Çiz
    Boran_draw_ui_panel(graph, *selected_node, search_text_buffer, sw, GetScreenHeight());

    

    // --- YENİ: Sol Üst Bilgi Kutusu ve FPS Sayacı ---
    int box_y = 35; // Kutuyu pencerenin üstünden biraz daha uzaklaştırdık
    
    // Kutunun yüksekliğini 56'dan 64'e çıkardık ki yazılar rahat nefes alsın
    DrawRectangle(8, box_y, 200, 64, (Color){ 25, 29, 36, 220 }); 
    DrawRectangleLines(8, box_y, 200, 64, (Color){ 74, 83, 101, 255 }); 

    int current_fps = GetFPS();
    Color fps_color = (current_fps >= 45) ? (Color){ 105, 240, 174, 255 } : (Color){ 255, 171, 64, 255 }; 
    
    // Yazıları kutunun üst çizgisinden biraz daha aşağı kaydırdık (+12 ve +36)
    DrawText(TextFormat("%d FPS", current_fps), 16, box_y + 12, 16, fps_color);
    DrawText(TextFormat("Toplam Dugum: %d", graph->node_count), 16, box_y + 36, 14, RAYWHITE);

    // Arama
    if (IsKeyPressed(KEY_ENTER) && search_text_buffer != NULL && strlen(search_text_buffer) > 0) {
        char clean[64] = {0};
        Boran_turkish_to_ascii(clean, search_text_buffer);
        Node* found = Boran_findFirstNode(trie_root, clean);
        if (found != NULL) {
            for(int j=0; j < graph->node_count; j++) graph->nodes[j]->is_selected = 0;
            found->is_selected = 1;
            *selected_node = found;
            Boran_focus_on_node(found);
        }
    }
    EndDrawing();
}