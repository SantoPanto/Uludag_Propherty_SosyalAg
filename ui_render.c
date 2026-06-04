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

    guiFont = GetFontDefault();
    GuiSetFont(guiFont);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 16); 
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

    // Ekranın dünya koordinatlarındaki sınırlarını bul (CULLING KALKANI İÇİN)
    // Ekrana 100 piksellik bir pay (padding) bırakıyoruz ki kenardakiler aniden yok olmasın
    Vector2 top_left = GetScreenToWorld2D((Vector2){ -100, -100 }, camera);
    Vector2 bottom_right = GetScreenToWorld2D((Vector2){ sw + 100, GetScreenHeight() + 100 }, camera);

    // Mouse Etkileşimleri
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
                        *selected_node = n; 
                    }
                }
                is_selecting = false;
            }
            dragging_node = NULL;
        }
    }

    BeginDrawing();
    ClearBackground((Color){ 34, 39, 46, 255 }); 
    BeginMode2D(camera);

    float time_now = GetTime(); 

    // --- KENARLARI ÇİZ VE OPTİMİZE ET ---
    for (int i = 0; i < graph->node_count; i++) {
        Node* src = graph->nodes[i];
        AdjListNode* adj = graph->adjLists[i];
        while (adj != NULL) {
            int target_idx = find_node_index(graph, adj->edge->target_id);
            if (target_idx != -1) {
                Node* target = graph->nodes[target_idx];
                bool is_src_sel = src->is_selected || (*selected_node != NULL && (*selected_node)->id == src->id);
                bool is_target_sel = target->is_selected || (*selected_node != NULL && (*selected_node)->id == target->id);
                
                float thick = (is_src_sel || is_target_sel) && !is_selecting ? 4.0f : 1.0f;
                Color col = edge_color_for_type(adj->edge->type);
                if (thick == 1.0f && (*selected_node != NULL || is_selecting)) col.a = 40; 
                
                Vector2 start_pos = { src->x, src->y };
                Vector2 end_pos = { target->x, target->y };
                
                // 1. Düz çizgiyi her halükarda çiz (DrawLine çok hafiftir, FPS düşürmez)
                DrawLineEx(start_pos, end_pos, thick, col);

                // --- 2. GÖRÜŞ ALANI (CULLING) KONTROLÜ ---
                // Düğümlerden en az biri kameranın içindeyse o ağır matematiğe gir, değilse atla!
                bool is_src_visible = (start_pos.x > top_left.x && start_pos.x < bottom_right.x && start_pos.y > top_left.y && start_pos.y < bottom_right.y);
                bool is_target_visible = (end_pos.x > top_left.x && end_pos.x < bottom_right.x && end_pos.y > top_left.y && end_pos.y < bottom_right.y);

                if (is_src_visible || is_target_visible) {
                    float dx = end_pos.x - start_pos.x;
                    float dy = end_pos.y - start_pos.y;
                    float angle = atan2f(dy, dx); // Artık saniyede binlerce kez gereksiz yere çalışmıyor
                    
                    float target_radius = (target->is_selected) ? 22.0f : 14.0f;
                    float offset = target_radius + 4.0f; 
                    Vector2 arrow_end = { end_pos.x - cosf(angle) * offset, end_pos.y - sinf(angle) * offset };

                    // Yön Okları
                    float arrow_size = thick > 1.0f ? 12.0f : 8.0f; 
                    Vector2 wing1 = { arrow_end.x - cosf(angle - 0.4f) * arrow_size, arrow_end.y - sinf(angle - 0.4f) * arrow_size };
                    Vector2 wing2 = { arrow_end.x - cosf(angle + 0.4f) * arrow_size, arrow_end.y - sinf(angle + 0.4f) * arrow_size };
                    
                    DrawLineEx(arrow_end, wing1, thick + 1.0f, col);
                    DrawLineEx(arrow_end, wing2, thick + 1.0f, col);

                    // Enerji Akışı (Sadece yakınken)
                    if (camera.zoom > 0.4f && col.a > 50) { 
                        float anim_speed = 0.6f; 
                        float time_offset = (float)(src->id + target->id) * 0.1f; 
                        
                        float t = fmodf((time_now * anim_speed) + time_offset, 1.0f);
                        Vector2 particle_pos = { start_pos.x + dx * t, start_pos.y + dy * t };
                        
                        Color particle_color = edge_color_for_type(adj->edge->type);
                        particle_color.a = 255; 
                        
                        // Kare çizmek yuvarlak çizmekten ÇOK daha hafiftir ve cyberpunk temaya çok uyar
                        float p_size = thick + 2.0f;
                        DrawRectangleRec((Rectangle){particle_pos.x - p_size, particle_pos.y - p_size, p_size*2, p_size*2}, particle_color);
                    }
                }
            }
            adj = adj->next;
        }
    }

    // Düğümleri Çiz
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        
        // --- DÜĞÜM CULLING ---
        // Ekranda olmayan düğümü hiç çizmeye uğraşma
        if (n->x < top_left.x || n->x > bottom_right.x || n->y < top_left.y || n->y > bottom_right.y) {
            continue; 
        }

        bool is_node_sel = n->is_selected || (*selected_node != NULL && (*selected_node)->id == n->id);
        float size = is_node_sel ? 22.0f : 14.0f;
        Color fill = node_color_for_type(n->type);
        Color border = is_node_sel ? (Color){ 255, 235, 59, 255 } : (Color){ 20, 24, 30, 255 }; 
        
        bool is_hovered = CheckCollisionPointCircle(mouse_world, (Vector2){n->x, n->y}, size);

        // Neon Parlama
        if (is_node_sel || is_hovered) {
            float glow_size = size * 1.8f; 
            Color glow_color = Fade(fill, 0.4f); 
            
            if (n->type == PHOTO) DrawRectangleRec((Rectangle){n->x-glow_size, n->y-glow_size, glow_size*2, glow_size*2}, glow_color);
            else if (n->type == EVENT) DrawPoly((Vector2){n->x, n->y}, 3, glow_size, 0, glow_color);
            else DrawCircleV((Vector2){n->x, n->y}, glow_size, glow_color);
        }

        // Derinlik ve Çerçeve
        float border_thick = 3.0f; 

        if (n->type == PHOTO) { 
            DrawRectangleRec((Rectangle){n->x-size-border_thick, n->y-size-border_thick, (size+border_thick)*2, (size+border_thick)*2}, border);
            DrawRectangleRec((Rectangle){n->x-size, n->y-size, size*2, size*2}, fill);
        }
        else if (n->type == EVENT) { 
            DrawPoly((Vector2){n->x, n->y}, 3, size + border_thick, 0, border);
            DrawPoly((Vector2){n->x, n->y}, 3, size, 0, fill);
        }
        else { 
            DrawCircleV((Vector2){n->x, n->y}, size + border_thick, border);
            DrawCircleV((Vector2){n->x, n->y}, size, fill);
        }
        
        // Akıllı Etiket (Sadece yakınken veya seçiliyken)
        if (camera.zoom > 1.2f || is_node_sel || is_hovered) {
            char label[64]; 
            node_get_display_label(n, label, sizeof(label));
            
            Vector2 sz = MeasureTextEx(guiFont, label, 14, 1);
            float text_x = n->x - (sz.x / 2.0f);
            float text_y = n->y + size + border_thick + 8.0f; 

            DrawRectangle((int)text_x - 4, (int)text_y - 2, (int)sz.x + 8, (int)sz.y + 4, Fade(BLACK, 0.8f));
            Color text_color = (is_hovered || is_node_sel) ? (Color){ 64, 196, 255, 255 } : RAYWHITE;
            DrawTextEx(guiFont, label, (Vector2){text_x, text_y}, 14, 1, text_color);
        }
    }

    if (is_selecting) {
        Rectangle sel_rect = { fminf(selection_start.x, selection_end.x), fminf(selection_start.y, selection_end.y), 
                               fabsf(selection_end.x - selection_start.x), fabsf(selection_end.y - selection_start.y) };
        DrawRectangleRec(sel_rect, Fade(SKYBLUE, 0.2f));
        DrawRectangleLinesEx(sel_rect, 2.0f / camera.zoom, (Color){ 64, 196, 255, 255 });
    }

    EndMode2D();
    
    Boran_draw_ui_panel(graph, *selected_node, search_text_buffer, sw, GetScreenHeight());

    int box_y = 35; 
    DrawRectangle(8, box_y, 200, 64, (Color){ 25, 29, 36, 220 }); 
    DrawRectangleLines(8, box_y, 200, 64, (Color){ 74, 83, 101, 255 }); 

    int current_fps = GetFPS();
    Color fps_color = (current_fps >= 45) ? (Color){ 105, 240, 174, 255 } : (Color){ 255, 171, 64, 255 }; 
    
    DrawText(TextFormat("%d FPS", current_fps), 16, box_y + 12, 16, fps_color);
    DrawText(TextFormat("Toplam Dugum: %d", graph->node_count), 16, box_y + 36, 14, RAYWHITE);

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