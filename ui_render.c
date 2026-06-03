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

static Color node_color_for_type(NodeType type, bool selected) {
    if (selected) return RED;
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
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);

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

    for (int i = 0; i < graph->node_count; i++) {
        Node* n = graph->nodes[i];
        bool is_selected = (*selected_node != NULL && (*selected_node)->id == n->id);
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

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_world = GetScreenToWorld2D(GetMousePosition(), camera);
        bool hit = false;
        for (int i = graph->node_count - 1; i >= 0; i--) {
            Node* n = graph->nodes[i];
            float hit_radius = (n->type == PHOTO) ? 14.0f : 12.0f;
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

    Boran_draw_ui_panel(graph, *selected_node, search_text_buffer, GetScreenWidth(), GetScreenHeight());

    if (IsKeyPressed(KEY_ENTER) && search_text_buffer != NULL && strlen(search_text_buffer) > 0) {
        Node* found = trie_find_first_node(trie_root, search_text_buffer);
        if (found != NULL) {
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
