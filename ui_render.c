#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include "ui_render.h"
#include "graph_adj.h"
#include "ui_integration.h" // Boran'ýn fonksiyonlarýný tanýmak için

Camera2D camera = { 0 };
Node* selected_node = NULL;
char search_text_buffer[64] = "";

void init_graphics_window() {
    InitWindow(1280, 720, "Sosyal Ag Analiz Araci");
    SetTargetFPS(60);
    camera.target = (Vector2){ 0.0f, 0.0f };
    camera.offset = (Vector2){ 1280.0f / 2.0f, 720.0f / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
}

void draw_graph_network(Graph* graph) {
    // --- 1. KAMERA KONTROLLERÝ ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        camera.offset = GetMousePosition();
        camera.target = mouseWorldPos;
        camera.zoom += (wheel * 0.1f);
        if (camera.zoom < 0.05f) camera.zoom = 0.05f;
        if (camera.zoom > 10.0f) camera.zoom = 10.0f;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f / camera.zoom);
        camera.target = Vector2Add(camera.target, delta);
    }

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // --- KAMERA DÜNYASI (Senin Alanýn) ---
    BeginMode2D(camera);

    // Çizgiler (Kenarlar)
    for (int i = 0; i < graph->node_count; i++) {
        Node* src_node = graph->nodes[i];
        AdjListNode* adj = graph->adjLists[i];

        while (adj != NULL) {
            int target_idx = find_node_index(graph, adj->edge->target_id);
            if (target_idx != -1) {
                Node* target_node = graph->nodes[target_idx];
                Color edge_color = LIGHTGRAY;
                if (adj->edge->type == FRIEND) edge_color = LIGHTGRAY;
                else if (adj->edge->type == LIKES) edge_color = SKYBLUE;
                else if (adj->edge->type == ATTENDS) edge_color = ORANGE;

                DrawLine((int)src_node->x, (int)src_node->y,
                         (int)target_node->x, (int)target_node->y, edge_color);
            }
            adj = adj->next;
        }
    }

    // Düðümler
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        bool is_selected = (selected_node != NULL && selected_node->id == n->id);
        float radius = is_selected ? 15.0f : 10.0f;

        if (n->type == USER) {
            DrawCircle((int)n->x, (int)n->y, radius, is_selected ? RED : BLUE);
        }
        else if (n->type == PHOTO) {
            float size = radius * 2.0f;
            DrawRectangle((int)n->x - (size/2), (int)n->y - (size/2), (int)size, (int)size, is_selected ? RED : GREEN);
        }
        else if (n->type == EVENT) {
            DrawCircle((int)n->x, (int)n->y, radius, is_selected ? RED : DARKPURPLE);
        }

        if (camera.zoom > 1.2f) {
            // Harita üzerindeki ID yazýlarýnýn boyutu 10'dan 20'ye çýkarýldý
            DrawText(TextFormat("ID:%d", n->id), (int)n->x - 10, (int)n->y - 18, 14, DARKGRAY);
        }
    }

    // Týklama Tespiti
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_world_pos = GetScreenToWorld2D(GetMousePosition(), camera);
        bool clicked_on_node = false;

        for (int i = 0; i < graph->node_count; i++) {
            Node* n = graph->nodes[i];
            if (CheckCollisionPointCircle(mouse_world_pos, (Vector2){n->x, n->y}, 12.0f)) {
                selected_node = n;
                clicked_on_node = true;
                break;
            }
        }
        if (!clicked_on_node) selected_node = NULL;
    }

    EndMode2D();
    // --- KAMERA DÜNYASI BÝTTÝ ---


    // --- EKRAN DÜNYASI (Boran'ýn Alaný) ---
    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();

    // ui_integration.c dosyasýndaki fonksiyonu tetikliyoruz
    Boran_draw_ui_panel(selected_node, search_text_buffer, screen_width, screen_height);

    // --- YENÝ EKLENEN: ARAMA KUTUSU ENTER MANTIÐI ---
    if (IsKeyPressed(KEY_ENTER)) {
        int aranan_id = 0;

        // Kullanýcý "User_15", "Event_5" veya sadece "15" yazarsa sayýyý bul
        if (sscanf(search_text_buffer, "User_%d", &aranan_id) == 1 ||
            sscanf(search_text_buffer, "Event_%d", &aranan_id) == 1 ||
            sscanf(search_text_buffer, "%d", &aranan_id) == 1) {

            // Graf içinde o ID'ye sahip düðümü ara
            int target_idx = find_node_index(graph, aranan_id);

            if (target_idx != -1) {
                // Düðümü bulursan onu seçili yap (Kýrmýzý yanacak)
                selected_node = graph->nodes[target_idx];

                // Kamerayý direkt hedefin üstüne kitle ve yakýnlaþ
                camera.target = (Vector2){ selected_node->x, selected_node->y };
                camera.zoom = 2.0f;
            }
        }
    }
    // ------------------------------------------------

    // FPS ve Ýstatistik Göstergesi (Yazýlar büyüdüðü için arka plan kutusu da büyütüldü)
    DrawRectangle(5, 5, 200, 60, Fade(WHITE, 0.8f));
    DrawFPS(10, 10);
    // Sol üstteki "Dugum Sayisi" yazýsý 10'dan 20'ye çýkarýldý
    DrawText(TextFormat("Dugum Sayisi: %d", graph->node_count), 10, 35, 20, DARKGRAY);

    EndDrawing();
}

void close_graphics_window() {
    CloseWindow();
}
