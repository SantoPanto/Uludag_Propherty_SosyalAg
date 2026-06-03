#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "graph_adj.h"
#include "ui_render.h"
#include "ui_integration.h"
#include "trie.h"

Camera2D camera = { 0 };

// --- SURUKLE VE BIRAK & COKLU SECIM DEGISKENLERI ---
#define MAX_MULTI_SELECT 1000
static Node* multi_selected_nodes[MAX_MULTI_SELECT];
static int multi_selected_count = 0;

static bool is_dragging_nodes = false;
static bool is_box_selecting = false;
static Vector2 box_start_pos = { 0 };
static Vector2 box_current_pos = { 0 };

// İki nokta arasındaki seçim dikdörtgenini hesaplayan yardımcı fonksiyon
static Rectangle get_selection_rect(Vector2 p1, Vector2 p2) {
    Rectangle r;
    r.x = (p1.x < p2.x) ? p1.x : p2.x;
    r.y = (p1.y < p2.y) ? p1.y : p2.y;
    r.width = (p1.x > p2.x ? p1.x - p2.x : p2.x - p1.x);
    r.height = (p1.y > p2.y ? p1.y - p2.y : p2.y - p1.y);
    return r;
}

// Bir düğümün çoklu seçim listesinde olup olmadığını kontrol eder
static bool is_node_multi_selected(int id) {
    for (int i = 0; i < multi_selected_count; i++) {
        if (multi_selected_nodes[i]->id == id) return true;
    }
    return false;
}

static Color node_color_for_type(NodeType type, bool selected) {
    if (selected) return RED; // Seçiliyse rengi kırmızı yap
    switch (type) {
        case USER: return BLUE;
        case PHOTO: return GREEN;
        case EVENT: return PURPLE;
        default: return GRAY;
    }
}

static Color edge_color_for_type(EdgeType type) {
    switch (type) {
        case FRIEND: return LIGHTGRAY;
        case LIKES: return SKYBLUE;
        case ATTENDS: return ORANGE;
        case HAS_PHOTO: return GOLD;
        default: return DARKGRAY;
    }
}

static Node* trie_first_match_from(TrieNode* node) {
    if (node == NULL) return NULL;
    if (node->isEndOfWord && node->matchingNodes != NULL) {
        return node->matchingNodes->graphNode;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != NULL) {
            Node* found = trie_first_match_from(node->children[i]);
            if (found != NULL) return found;
        }
    }
    return NULL;
}

static Node* trie_find_first_node(TrieNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL || prefix[0] == '\0') return NULL;

    TrieNode* current = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        int index = tolower((unsigned char)prefix[i]);
        if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == NULL) {
            return NULL;
        }
        current = current->children[index];
    }
    return trie_first_match_from(current);
}

void init_graphics_window(void) {
    InitWindow(1280, 720, "Property Graph - Sosyal Ag");
    SetTargetFPS(60);
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 640.0f, 360.0f };
    camera.rotation = 0.0f;
    camera.zoom = 0.85f;
}

void close_graphics_window(void) {
    CloseWindow();
}

void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer) {
    // --- KAMERA YONETIMI ---
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

    // --- FARE ETKILESI (MOUSE INPUT) ---
    Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
    int sw = GetScreenWidth();
    bool mouse_on_ui = GetMouseX() > sw - 360; // Yan panele tıklıyorsak işlemi yoksay

    if (!mouse_on_ui) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            bool hit = false;
            // Düğümlere tıklandığını kontrol et
            for (int i = graph->node_count - 1; i >= 0; i--) {
                Node* n = graph->nodes[i];
                float hit_radius = (n->type == PHOTO) ? 14.0f : 12.0f;
                if (CheckCollisionPointCircle(mouse_world, (Vector2){ n->x, n->y }, hit_radius)) {
                    hit = true;
                    // Eğer tıklanan node önceden seçili listesinde değilse, eski seçimi temizle
                    if (!is_node_multi_selected(n->id)) {
                        multi_selected_count = 0;
                        multi_selected_nodes[multi_selected_count++] = n;
                    }
                    *selected_node = n; // Yan panel için detayı göster
                    is_dragging_nodes = true; // Sürükleme modunu başlat
                    break;
                }
            }

            if (!hit) {
                // Boşluğa tıklandı: Seçimleri sıfırla ve kutu çizimini başlat
                multi_selected_count = 0;
                *selected_node = NULL;
                is_box_selecting = true;
                box_start_pos = mouse_world;
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (is_dragging_nodes) {
                // Seçili tüm düğümleri fare hareketi kadar (delta) kaydır
                Vector2 screen_delta = GetMouseDelta();
                Vector2 world_delta = Vector2Scale(screen_delta, 1.0f / camera.zoom);
                for (int i = 0; i < multi_selected_count; i++) {
                    multi_selected_nodes[i]->x += world_delta.x;
                    multi_selected_nodes[i]->y += world_delta.y;
                }
            }
            else if (is_box_selecting) {
                // Kutu seçim sınırını güncelle
                box_current_pos = mouse_world;
            }
        }
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            if (is_dragging_nodes) {
                is_dragging_nodes = false;
            }
            if (is_box_selecting) {
                is_box_selecting = false;
                box_current_pos = mouse_world;
                Rectangle selection_rect = get_selection_rect(box_start_pos, box_current_pos);

                // Kutunun içinde kalan tüm düğümleri toplu seçim listesine ekle
                multi_selected_count = 0;
                for (int i = 0; i < graph->node_count; i++) {
                    Node* n = graph->nodes[i];
                    if (CheckCollisionPointRec((Vector2){n->x, n->y}, selection_rect)) {
                        if (multi_selected_count < MAX_MULTI_SELECT) {
                            multi_selected_nodes[multi_selected_count++] = n;
                        }
                    }
                }
                
                // Eğer kutu içinde düğüm varsa, yan panele ilkini bilgi olarak ver
                if (multi_selected_count > 0) {
                    *selected_node = multi_selected_nodes[0];
                }
            }
        }
    } else {
        // Fare UI üzerine geldiğinde bug olmaması için sürüklemeyi iptal et
        is_dragging_nodes = false;
        is_box_selecting = false;
    }

    // --- CIZIM ASAMASI ---
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

    // 1. Kenarları Çiz
    for (int i = 0; i < graph->node_count; i++) {
        Node* src_node = graph->nodes[i];
        int src_idx = i;
        AdjListNode* adj = graph->adjLists[src_idx];
        while (adj != NULL) {
            if (adj->edge != NULL) {
                int target_idx = find_node_index(graph, adj->edge->target_id);
                if (target_idx != -1 && src_idx < target_idx) {
                    Node* target_node = graph->nodes[target_idx];
                    DrawLineEx((Vector2){ src_node->x, src_node->y },
                               (Vector2){ target_node->x, target_node->y },
                               1.5f, edge_color_for_type(adj->edge->type));
                }
            }
            adj = adj->next;
        }
    }

    // 2. Düğümleri Çiz
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        
        // Düğüm seçili mi kontrolü (çoklu seçim listesinde arar)
        bool is_selected = is_node_multi_selected(n->id);
        
        float radius = is_selected ? 14.0f : 9.0f;
        Color fill = node_color_for_type(n->type, is_selected);

        if (n->type == PHOTO) {
            float s = radius * 2.0f;
            DrawRectangle((int)(n->x - s / 2), (int)(n->y - s / 2), (int)s, (int)s, fill);
        } else {
            DrawCircle((int)n->x, (int)n->y, radius, fill);
        }

        if (camera.zoom > 0.5f) {
            char label[48];
            node_get_display_label(n, label, sizeof(label));
            DrawText(label, (int)n->x - 40, (int)n->y - 28, 11, DARKGRAY);
        }
    }

    // 3. Seçim Kutusunu Çiz (Marquee Box)
    if (is_box_selecting) {
        Rectangle sel_rec = get_selection_rect(box_start_pos, box_current_pos);
        DrawRectangleRec(sel_rec, Fade(BLUE, 0.2f)); // İçini yarı saydam doldur
        DrawRectangleLinesEx(sel_rec, 2.0f / camera.zoom, BLUE); // Kenarlıklar
    }

    EndMode2D();

    // --- ARAYÜZ (UI) ÇİZİMİ ---
    Boran_draw_ui_panel(graph, *selected_node, search_text_buffer, GetScreenWidth(), GetScreenHeight());

    // Arama fonksiyonu tetikleyici
    if (IsKeyPressed(KEY_ENTER) && search_text_buffer != NULL && strlen(search_text_buffer) > 0) {
        Node* found = trie_find_first_node(trie_root, search_text_buffer);
        if (found != NULL) {
            multi_selected_count = 0;
            multi_selected_nodes[multi_selected_count++] = found;
            *selected_node = found;
            camera.target = (Vector2){ found->x, found->y };
            camera.zoom = 1.8f;
        }
    }

    DrawRectangle(8, 8, 240, 52, Fade(WHITE, 0.85f));
    DrawFPS(14, 14);
    DrawText(TextFormat("Dugum: %d", graph->node_count), 14, 36, 16, DARKGRAY);

    EndDrawing();
}