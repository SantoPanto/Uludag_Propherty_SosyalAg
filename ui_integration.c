#include "raylib.h"
#include "graph_adj.h"
#include "ui_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "graph_models.h"
#include "hash_table.h"
#include "trie.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

extern Graph* my_graph;
extern HashTable* g_ht;
extern TrieNode* g_trie;

#ifndef DISABLE_AI
extern char ai_result_buffer[256];
extern int ai_is_fetching;
extern int ai_new_data_ready;
void* fetch_ai_user_thread(void* arg);
#endif

static const char* edge_type_label(EdgeType type) {
    return edge_type_to_string(type);
}

void Boran_format_side_panel_text(Graph* graph, Node* node, char* buffer, int max_len) {
    if (buffer == NULL || max_len <= 0) return;

    if (node == NULL) {
        snprintf(buffer, (size_t)max_len,
                 "Haritadan bir dugume tiklayin.\n\n"
                 "Turler:\n- Mavi: Kullanici\n- Yesil: Fotograf\n- Mor: Etkinlik\n\n"
                 "Arama kutusuna isim yazip Enter'a basin.");
        return;
    }

    char label[128];
    node_get_display_label(node, label, sizeof(label));
    int offset = snprintf(buffer, (size_t)max_len,
                          "=== %s ===\nID: %d | Tur: %s\n\nOZELLIKLER:\n",
                          label, node->id, node_type_to_string(node->type));

    for (int i = 0; i < node->property_count && offset < max_len - 1; i++) {
        Property* p = &node->properties[i];
        if (p->name == NULL) continue;
        if (p->type == TYPE_STRING && p->value.s_val != NULL) {
            offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                               "- %s: %s\n", p->name, p->value.s_val);
        } else if (p->type == TYPE_INTEGER) {
            offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                               "- %s: %d\n", p->name, p->value.i_val);
        }
    }

    if (graph == NULL) return;

    int node_idx = find_node_index(graph, node->id);
    if (node_idx == -1) return;

    offset += snprintf(buffer + offset, (size_t)(max_len - offset), "\nILISKILER (cikis kenarlari):\n");

    int edge_lines = 0;
    AdjListNode* adj = graph->adjLists[node_idx];
    while (adj != NULL && offset < max_len - 40 && edge_lines < 12) {
        if (adj->edge != NULL) {
            int target_idx = find_node_index(graph, adj->edge->target_id);
            char target_label[96] = "?";
            if (target_idx != -1) {
                node_get_display_label(graph->nodes[target_idx], target_label, sizeof(target_label));
            }
            char since[32] = "";
            for (int j = 0; j < adj->edge->property_count; j++) {
                if (adj->edge->properties[j].name != NULL &&
                    strcmp(adj->edge->properties[j].name, "since") == 0 &&
                    adj->edge->properties[j].type == TYPE_STRING &&
                    adj->edge->properties[j].value.s_val != NULL) {
                    snprintf(since, sizeof(since), " [%s]", adj->edge->properties[j].value.s_val);
                    break;
                }
            }
            offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                               "-> %s: %s%s\n",
                               edge_type_label(adj->edge->type), target_label, since);
            edge_lines++;
        }
        adj = adj->next;
    }

    if (edge_lines == 0) {
        snprintf(buffer + offset, (size_t)(max_len - offset), "(Henuz baglanti yok)\n");
    } else if (adj != NULL) {
        snprintf(buffer + offset, (size_t)(max_len - offset), "... (daha fazla kenar var)\n");
    }
}

void Boran_draw_ui_panel(Graph* graph, Node* selected_node, char* search_text_buffer,
                         int screen_width, int screen_height) {
    int panel_width = 360;
    int panel_x = screen_width - panel_width;

    GuiPanel((Rectangle){ (float)panel_x, 0, (float)panel_width, (float)screen_height },
             "Property Graph Paneli");

    DrawText("Isim / etkinlik ara (Enter):", panel_x + 16, 36, 14, DARKGRAY);

    static bool search_edit_mode = false;
    if (GuiTextBox((Rectangle){ (float)panel_x + 16, 58, (float)panel_width - 32, 28 },
                   search_text_buffer, 63, search_edit_mode)) {
        search_edit_mode = !search_edit_mode;
    }

#ifndef DISABLE_AI
    if (ai_is_fetching) {
        GuiDisable();
        GuiButton((Rectangle){ (float)panel_x + 16, 96, (float)panel_width - 32, 28 },
                  "AI kullanici uretiliyor...");
        GuiEnable();
    } else {
        if (GuiButton((Rectangle){ (float)panel_x + 16, 96, (float)panel_width - 32, 28 },
                      "AI ile yeni kullanici")) {
            ai_is_fetching = 1;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, fetch_ai_user_thread, NULL);
            pthread_detach(thread_id);
        }
    }

    if (ai_new_data_ready == 1 && my_graph != NULL) {
        ai_new_data_ready = 0;

        char name[50] = "Yeni Kullanici";
        char* token = strtok(ai_result_buffer, "|");
        if (token != NULL) {
            strncpy(name, token, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
        }

        static int ai_user_id_counter = 5000;
        Node* new_user = create_node(ai_user_id_counter++, USER);
        new_user->x = (float)(rand() % 400) - 200.0f;
        new_user->y = (float)(rand() % 400) - 200.0f;

        add_property_to_node(new_user, "Name", TYPE_STRING, name);
        add_node_to_graph(my_graph, new_user);
        if (g_ht != NULL) insert_to_hash(g_ht, new_user);
        if (g_trie != NULL) Boran_insertToTrie(g_trie, name, new_user);
        printf("[+] AI kullanicisi eklendi: %s\n", name);
    }
#endif

    char detail_text[2048] = {0};
    Boran_format_side_panel_text(graph, selected_node, detail_text, sizeof(detail_text));

    int y = 96;
    const char* line = detail_text;
    char line_buf[256];
    while (*line != '\0' && y < screen_height - 20) {
        int i = 0;
        while (line[i] != '\0' && line[i] != '\n' && i < 255) {
            line_buf[i] = line[i];
            i++;
        }
        line_buf[i] = '\0';
        DrawText(line_buf, panel_x + 16, y, 14, BLACK);
        y += 18;
        if (line[i] == '\n') line++;
        else if (line[i] == '\0') break;
        else line++;
    }
}
