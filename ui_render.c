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

// Pencere ve grafik başlatma işlemleri
void init_graphics_window() {
    InitWindow(1280, 720, "Sosyal Ag Analiz Araci");
    
    // KAMERAYI BAŞLATIN
    camera.target = (Vector2){ 0.0f, 0.0f }; // Merkeze bak
    camera.offset = (Vector2){ 640.0f, 360.0f }; // Ekranın ortasına offsetle
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; // Zoom 0 olmamalı!
    
    SetTargetFPS(60);
}

// Pencereyi kapatma
void close_graphics_window() {
    CloseWindow();
}

void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer) {
    
    // --- 1. KAMERA KONTROLLERİ ---
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

    // --- 2. ÇİZİM İŞLEMLERİ ---
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Dünya uzayı çizimi
    BeginMode2D(camera);
        // Kenarların (çizgiler) çizilmesi
        for (int i = 0; i < graph->node_count; i++) {
            Node* src_node = graph->nodes[i];
            AdjListNode* adj = graph->adjLists[i];
            while (adj != NULL) {
                int target_idx = find_node_index(graph, adj->edge->target_id);
                if (target_idx != -1) {
                    Node* target_node = graph->nodes[target_idx];
                    DrawLine((int)src_node->x, (int)src_node->y, (int)target_node->x, (int)target_node->y, LIGHTGRAY);
                }
                adj = adj->next;
            }
        }
    EndMode2D(); // Dünya koordinatları bitti

    // --- DÜĞÜMLERİ EKRAN UZAYINDA (SCREEN SPACE) ÇİZ ---
    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        // BeginMode2D'nin dışına çıkarak koordinatlara offset (640, 360) ekledik
        bool is_selected = (*selected_node != NULL && (*selected_node)->id == n->id);
        DrawCircle((int)n->x + 640, (int)n->y + 360, is_selected ? 15.0f : 10.0f, is_selected ? RED : BLUE);
    }

    // --- 3. ARAYÜZ VE MANTIKSAL İŞLEMLER ---
    
    // UI panelini çiz
    Boran_draw_ui_panel(*selected_node, search_text_buffer, GetScreenWidth(), GetScreenHeight());

    // Arama Mantığı (Trie üzerinden düğüm bulma)
    if (IsKeyPressed(KEY_ENTER) && strlen(search_text_buffer) > 0) {
        TrieNode* current = trie_root;
        bool found = true;
        for (int i = 0; search_text_buffer[i] != '\0'; i++) {
            int index = tolower((unsigned char)search_text_buffer[i]) - 'a';
            if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == NULL) {
                found = false; break;
            }
            current = current->children[index];
        }

        if (found && current->isEndOfWord) {
            Node* target = current->matchingNodes->graphNode;
            camera.target = (Vector2){ target->x, target->y };
            camera.zoom = 2.0f;
        }
    }

    // Bilgilendirme metinleri
    DrawFPS(10, 10);
    DrawText(TextFormat("Dugum Sayisi: %d", graph->node_count), 10, 35, 20, DARKGRAY);

    EndDrawing();
}