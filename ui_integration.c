#include "raylib.h"
#include "graph_adj.h"
#include "ui_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include "graph_models.h"
#include "hash_table.h"
#include "trie.h"
#include "queries.h" // YENI EKLENDI: Faz 3 analizleri icin

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

extern Graph* my_graph;
extern HashTable* g_ht;
extern TrieNode* g_trie;

#ifndef DISABLE_AI
extern char ai_result_buffer[8192];
extern int ai_is_fetching;
extern int ai_new_data_ready;
extern int ai_bulk_count;
void* fetch_ai_user_thread(void* arg);
#endif

static int dynamic_id_counter = 5000;

static const char* edge_type_label(EdgeType type) {
    return edge_type_to_string(type);
}

void Boran_turkish_to_ascii(char *dest, const char *src) {
    int d = 0;
    for (int i = 0; src[i] != '\0'; i++) {
        unsigned char c = src[i];
        unsigned char next = src[i+1];

        if (c == 0xC3) {
            if (next == 0xA7) { dest[d++] = 'c'; i++; continue; }
            if (next == 0x87) { dest[d++] = 'C'; i++; continue; }
            if (next == 0xB6) { dest[d++] = 'o'; i++; continue; }
            if (next == 0x96) { dest[d++] = 'O'; i++; continue; }
            if (next == 0xBC) { dest[d++] = 'u'; i++; continue; }
            if (next == 0x9C) { dest[d++] = 'U'; i++; continue; }
        } else if (c == 0xC4) {
            if (next == 0x9F) { dest[d++] = 'g'; i++; continue; }
            if (next == 0x9E) { dest[d++] = 'G'; i++; continue; }
            if (next == 0xB1) { dest[d++] = 'i'; i++; continue; }
            if (next == 0xB0) { dest[d++] = 'I'; i++; continue; }
        } else if (c == 0xC5) {
            if (next == 0x9F) { dest[d++] = 's'; i++; continue; }
            if (next == 0x9E) { dest[d++] = 'S'; i++; continue; }
        }
        dest[d++] = src[i];
    }
    dest[d] = '\0';
}

void find_suggestions_recursive(TrieNode* current, Node** results, int* count, int max_count) {
    if (current == NULL || *count >= max_count) return;

    if (current->isEndOfWord && current->matchingNodes != NULL) {
        results[*count] = current->matchingNodes->graphNode;
        (*count)++;
    }

    for (int i = 0; i < ALPHABET_SIZE && *count < max_count; i++) {
        if (current->children[i] != NULL) {
            find_suggestions_recursive(current->children[i], results, count, max_count);
        }
    }
}

void Boran_format_side_panel_text(Graph* graph, Node* node, char* buffer, int max_len) {
    if (buffer == NULL || max_len <= 0) return;

    if (node == NULL) {
        snprintf(buffer, (size_t)max_len,
                 "Haritadan bir dugume tiklayin.\n\n"
                 "--- TURLER ---\n- Mavi: Kullanici\n- Yesil: Fotograf\n- Mor: Etkinlik\n\n"
                 "Arama kutusuna isim yazip Enter'a basin.");
        return;
    }

    char label[128];
    node_get_display_label(node, label, sizeof(label));

    int offset = snprintf(buffer, (size_t)max_len,
                          "=== %s ===\nID: %d | Tur: %s\n\n--- OZELLIKLER ---\n",
                          label, node->id, node_type_to_string(node->type));

    for (int i = 0; i < node->property_count && offset < max_len - 1; i++) {
        Property* p = &node->properties[i];
        if (p->name == NULL) continue;
        if (p->type == TYPE_STRING && p->value.s_val != NULL) {
            offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                               " %s: %s\n", p->name, p->value.s_val);
        } else if (p->type == TYPE_INTEGER) {
            offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                               " %s: %d\n", p->name, p->value.i_val);
        }
    }

    if (graph == NULL) return;

    int node_idx = find_node_index(graph, node->id);
    if (node_idx == -1) return;

    offset += snprintf(buffer + offset, (size_t)(max_len - offset), "\n--- CIKIS BAGLANTILARI ---\n");

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
        offset += snprintf(buffer + offset, (size_t)(max_len - offset), "(Cikis baglantisi yok)\n");
    } else if (adj != NULL) {
        offset += snprintf(buffer + offset, (size_t)(max_len - offset), "... (daha fazlasi var)\n");
    }

    offset += snprintf(buffer + offset, (size_t)(max_len - offset), "\n--- GELEN BAGLANTILAR ---\n");

    int incoming_lines = 0;
    for (int i = 0; i < graph->node_count && offset < max_len - 40 && incoming_lines < 12; i++) {
        if (graph->nodes[i]->id == node->id) continue;

        AdjListNode* current_adj = graph->adjLists[i];
        while (current_adj != NULL && offset < max_len - 40 && incoming_lines < 12) {
            if (current_adj->edge != NULL && current_adj->edge->target_id == node->id) {
                char source_label[96] = "?";
                node_get_display_label(graph->nodes[i], source_label, sizeof(source_label));

                offset += snprintf(buffer + offset, (size_t)(max_len - offset),
                                   "<- %s: %s\n",
                                   edge_type_label(current_adj->edge->type), source_label);
                incoming_lines++;
            }
            current_adj = current_adj->next;
        }
    }

    if (incoming_lines == 0) {
        snprintf(buffer + offset, (size_t)(max_len - offset), "(Gelen baglanti yok)\n");
    } else if (incoming_lines >= 12) {
        snprintf(buffer + offset, (size_t)(max_len - offset), "... (daha fazlasi var)\n");
    }
}

void Boran_draw_ui_panel(Graph* graph, Node* selected_node, char* search_text_buffer,
                         int screen_width, int screen_height) {
    int panel_width = 360;
    int panel_x = screen_width - panel_width;

    GuiPanel((Rectangle){ (float)panel_x, 0, (float)panel_width, (float)screen_height }, "Property Graph Paneli");

    DrawText("Isim / etkinlik ara (Enter):", panel_x + 16, 36, 14, (Color){ 200, 210, 220, 255 });

    static bool search_edit_mode = false;

    int suggestion_count = 0;
    Node* suggestions[5] = {0};
    bool dropdown_active = false;
    Rectangle drop_rect = {0};

    if (search_edit_mode && strlen(search_text_buffer) > 0 && g_trie != NULL) {
        TrieNode* current = g_trie;
        int len = strlen(search_text_buffer);
        bool found = true;

        for (int i = 0; i < len; i++) {
            int index = tolower((unsigned char)search_text_buffer[i]);
            if (index < 0 || index >= ALPHABET_SIZE || current->children[index] == NULL) {
                found = false;
                break;
            }
            current = current->children[index];
        }

        if (found) {
            find_suggestions_recursive(current, suggestions, &suggestion_count, 5);
            if (suggestion_count > 0) {
                dropdown_active = true;
                drop_rect = (Rectangle){ (float)panel_x + 16, 86, (float)panel_width - 32, (float)(suggestion_count * 25) };
            }
        }
    }

    Vector2 mousePoint = GetMousePosition();
    bool mouse_in_dropdown = dropdown_active && CheckCollisionPointRec(mousePoint, drop_rect);

    bool textbox_toggled = GuiTextBox((Rectangle){ (float)panel_x + 16, 58, (float)panel_width - 32, 28 }, search_text_buffer, 63, search_edit_mode);

    if (textbox_toggled) {
        if (!mouse_in_dropdown) {
            search_edit_mode = !search_edit_mode;
        } else {
            search_edit_mode = true;
        }
    }

    int start_y = 96;
    static int bulk_add_count = 1;
    static bool spinner_edit_mode = false;

    DrawText("Eklenecek Miktar:", panel_x + 16, start_y + 6, 14, (Color){ 200, 210, 220, 255 });
    if (GuiSpinner((Rectangle){ (float)panel_x + 140, start_y, 100, 28 }, "", &bulk_add_count, 1, 100, spinner_edit_mode) && !mouse_in_dropdown) {
        spinner_edit_mode = !spinner_edit_mode;
    }
    start_y += 38;

#ifndef DISABLE_AI
    if (ai_is_fetching) {
        GuiDisable();
        GuiButton((Rectangle){ (float)panel_x + 16, start_y, (float)panel_width - 32, 28 }, "Yapay Zeka Uretiyor...");
        GuiButton((Rectangle){ (float)panel_x + 16, start_y + 38, (float)panel_width - 32, 28 }, "Yapay Zeka Uretiyor...");
        GuiButton((Rectangle){ (float)panel_x + 16, start_y + 76, (float)panel_width - 32, 28 }, "Yapay Zeka Uretiyor...");
        GuiEnable();
        start_y += 114;
    } else {
        if (GuiButton((Rectangle){ (float)panel_x + 16, start_y, (float)panel_width - 32, 28 }, "Yapay Zeka: Kullanici Ekle") && !mouse_in_dropdown) {
            ai_bulk_count = bulk_add_count; ai_is_fetching = 2;
            pthread_t t; pthread_create(&t, NULL, fetch_ai_user_thread, NULL); pthread_detach(t);
        }
        start_y += 38;
        if (GuiButton((Rectangle){ (float)panel_x + 16, start_y, (float)panel_width - 32, 28 }, "Yapay Zeka: Fotograf Ekle") && !mouse_in_dropdown) {
            ai_bulk_count = bulk_add_count; ai_is_fetching = 3;
            pthread_t t; pthread_create(&t, NULL, fetch_ai_user_thread, NULL); pthread_detach(t);
        }
        start_y += 38;
        if (GuiButton((Rectangle){ (float)panel_x + 16, start_y, (float)panel_width - 32, 28 }, "Yapay Zeka: Etkinlik Ekle") && !mouse_in_dropdown) {
            ai_bulk_count = bulk_add_count; ai_is_fetching = 4;
            pthread_t t; pthread_create(&t, NULL, fetch_ai_user_thread, NULL); pthread_detach(t);
        }
        start_y += 38;
    }

    if (ai_new_data_ready > 0 && my_graph != NULL) {
        int mode = ai_new_data_ready;
        ai_new_data_ready = 0;

        if (strcmp(ai_result_buffer, "Hata") != 0) {
            char* token = strtok(ai_result_buffer, "|");
            while (token != NULL) {
                char safe_str[128] = {0};
                Boran_turkish_to_ascii(safe_str, token);

                Node* new_node = NULL;

                if (mode == 2) {
                    new_node = create_node(dynamic_id_counter++, USER);
                    new_node->x = (float)(rand() % 3000) - 1500.0f;
                    new_node->y = (float)(rand() % 3000) - 1500.0f;
                    int age = 18 + rand() % 40;
                    add_property_to_node(new_node, "Name", TYPE_STRING, strdup(safe_str));
                    add_property_to_node(new_node, "Age", TYPE_INTEGER, &age);

                    // YENI EKLENEN: Terminal ciktisi
                    printf("[FAZ 2] AI ile Kullanici olusturuldu: %s (ID: %d, Yas: %d)\n", safe_str, new_node->id, age);

                    add_node_to_graph(my_graph, new_node);
                    if (g_ht != NULL) insert_to_hash(g_ht, new_node);
                    if (g_trie != NULL) Boran_insertToTrie(g_trie, strdup(safe_str), new_node);

                    if (my_graph->node_count > 1) {
                        for (int t = 0; t < 20; t++) {
                            int random_target_idx = rand() % my_graph->node_count;
                            Node* target = my_graph->nodes[random_target_idx];
                            if (target->id != new_node->id && target->type == USER) {
                                add_edge(my_graph, new_node->id, target->id, FRIEND, false);
                                break;
                            }
                        }
                    }
                }
                else if (mode == 3) {
                    new_node = create_node(dynamic_id_counter++, PHOTO);
                    new_node->x = (float)(rand() % 3000) - 1500.0f;
                    new_node->y = (float)(rand() % 3000) - 1500.0f;
                    int size_mb = 2 + rand() % 10;

                    add_property_to_node(new_node, "Title", TYPE_STRING, safe_str);
                    add_property_to_node(new_node, "Description", TYPE_STRING, safe_str);
                    add_property_to_node(new_node, "Resolution", TYPE_STRING, "1920x1080");
                    add_property_to_node(new_node, "Size(MB)", TYPE_INTEGER, &size_mb);

                    // YENI EKLENEN: Terminal ciktisi
                    printf("[FAZ 2] AI ile Fotograf eklendi: %s (ID: %d, Boyut: %dMB)\n", safe_str, new_node->id, size_mb);

                    add_node_to_graph(my_graph, new_node);
                    if (g_ht != NULL) insert_to_hash(g_ht, new_node);
                    if (g_trie != NULL) Boran_insertToTrie(g_trie, safe_str, new_node);

                    if (my_graph->node_count > 1) {
                        for (int t = 0; t < 20; t++) {
                            int random_target_idx = rand() % my_graph->node_count;
                            Node* target = my_graph->nodes[random_target_idx];
                            if (target->id != new_node->id && target->type == USER) {
                                add_edge(my_graph, target->id, new_node->id, LIKES, true);
                                break;
                            }
                        }
                    }
                }
                else if (mode == 4) {
                    new_node = create_node(dynamic_id_counter++, EVENT);
                    new_node->x = (float)(rand() % 3000) - 1500.0f;
                    new_node->y = (float)(rand() % 3000) - 1500.0f;
                    int capacity = 50 + rand() % 500;
                    add_property_to_node(new_node, "Title", TYPE_STRING, strdup(safe_str));
                    add_property_to_node(new_node, "Capacity", TYPE_INTEGER, &capacity);

                    // YENI EKLENEN: Terminal ciktisi
                    printf("[FAZ 2] AI ile Etkinlik olusturuldu: %s (ID: %d, Kapasite: %d)\n", safe_str, new_node->id, capacity);

                    add_node_to_graph(my_graph, new_node);
                    if (g_ht != NULL) insert_to_hash(g_ht, new_node);
                    if (g_trie != NULL) Boran_insertToTrie(g_trie, strdup(safe_str), new_node);

                    if (my_graph->node_count > 1) {
                        for (int k = 0; k < 2; k++) {
                            for (int t = 0; t < 20; t++) {
                                int random_target_idx = rand() % my_graph->node_count;
                                Node* target = my_graph->nodes[random_target_idx];
                                if (target->id != new_node->id && target->type == USER) {
                                    add_edge(my_graph, target->id, new_node->id, ATTENDS, true);
                                    break;
                                }
                            }
                        }
                    }
                }
                token = strtok(NULL, "|");
            }

            // YENI EKLENEN: Toplu islem sonrasi Faz 3 Analizi
            printf("\n--- [FAZ 3] AGI YENIDEN ANALIZ EDILIYOR ---\n");
            find_most_active_node(my_graph);
            printf("----------------------------------------------\n\n");
        }
    }
#endif

    char detail_text[2048] = {0};
    Boran_format_side_panel_text(graph, selected_node, detail_text, sizeof(detail_text));

    int y = start_y + 10;
    const char* line = detail_text;
    char line_buf[256];

    while (*line != '\0' && y < screen_height - 20) {
        int i = 0;
        while (line[i] != '\0' && line[i] != '\n' && i < 255) {
            line_buf[i] = line[i];
            i++;
        }
        line_buf[i] = '\0';

        Color text_color = RAYWHITE;

        if (strstr(line_buf, "===") != NULL || strstr(line_buf, "---") != NULL) {
            text_color = (Color){ 64, 196, 255, 255 };
        }
        else if (strstr(line_buf, "->") != NULL || strstr(line_buf, "<-") != NULL) {
            text_color = (Color){ 180, 200, 220, 255 };
        }

        DrawText(line_buf, panel_x + 16, y, 14, text_color);
        y += 18;

        line += i;
        if (*line == '\n') line++;
    }

    if (dropdown_active) {
        DrawRectangleRec(drop_rect, (Color){ 30, 34, 40, 255 });
        DrawRectangleLinesEx(drop_rect, 1, (Color){ 74, 83, 101, 255 });

        for (int i = 0; i < suggestion_count; i++) {
            Rectangle item_rect = { drop_rect.x, drop_rect.y + (i * 25), drop_rect.width, 25 };
            bool isHovering = CheckCollisionPointRec(mousePoint, item_rect);

            if (isHovering) {
                DrawRectangleRec(item_rect, (Color){ 68, 75, 87, 255 });
            }

            char label[128] = {0};
            node_get_display_label(suggestions[i], label, sizeof(label));

            DrawText(label, item_rect.x + 8, item_rect.y + 6, 14, isHovering ? (Color){ 64, 196, 255, 255 } : RAYWHITE);

            if (isHovering && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                strncpy(search_text_buffer, label, 63);
                search_text_buffer[63] = '\0';
                search_edit_mode = false;
            }
        }
    }
}
