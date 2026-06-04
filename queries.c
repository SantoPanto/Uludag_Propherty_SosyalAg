#include <stdio.h>
#include <stdlib.h>
#include "queries.h"
#include "graph_models.h"
#include "graph_adj.h"

//EMÝNE TABAN

static void print_node_label(Graph* graph, int node_id) {
    int idx = find_node_index(graph, node_id);
    if (idx == -1) {
        printf("ID:%d", node_id);
        return;
    }
    char label[128];
    node_get_display_label(graph->nodes[idx], label, sizeof(label));
    printf("%s (ID:%d)", label, node_id);
}

void find_friends_events_photos(Graph* graph, int start_user_id) {
    int user_idx = find_node_index(graph, start_user_id);
    if (user_idx == -1) {
        printf("Hata: %d ID'li kullanici bulunamadi.\n", start_user_id);
        return;
    }

    char user_label[128];
    node_get_display_label(graph->nodes[user_idx], user_label, sizeof(user_label));
    printf("\n--- %s icin cok adimli sorgu (Arkadas -> Etkinlik -> Fotograf) ---\n", user_label);

    int found = 0;
    AdjListNode* friend_edge = graph->adjLists[user_idx];
    while (friend_edge != NULL) {
        if (friend_edge->edge != NULL && friend_edge->edge->type == FRIEND) {
            int friend_id = friend_edge->edge->target_id;
            int friend_idx = find_node_index(graph, friend_id);

            if (friend_idx != -1) {
                AdjListNode* event_edge = graph->adjLists[friend_idx];
                while (event_edge != NULL) {
                    if (event_edge->edge != NULL && event_edge->edge->type == ATTENDS) {
                        int event_id = event_edge->edge->target_id;
                        int event_idx = find_node_index(graph, event_id);

                        if (event_idx != -1) {
                            AdjListNode* photo_edge = graph->adjLists[event_idx];
                            while (photo_edge != NULL) {
                                if (photo_edge->edge != NULL && photo_edge->edge->type == HAS_PHOTO) {
                                    printf("  [+] Fotograf: ");
                                    print_node_label(graph, photo_edge->edge->target_id);
                                    printf(" | Arkadas: ");
                                    print_node_label(graph, friend_id);
                                    printf(" | Etkinlik: ");
                                    print_node_label(graph, event_id);
                                    printf("\n");
                                    found = 1;
                                }
                                photo_edge = photo_edge->next;
                            }
                        }
                    }
                    event_edge = event_edge->next;
                }
            }
        }
        friend_edge = friend_edge->next;
    }

    if (!found) {
        printf("  (Bu kullanici icin Arkadas->Etkinlik->Fotograf zinciri bulunamadi.)\n");
    }
}

void find_most_active_node(Graph* graph) {
    int max_degree = -1;
    int most_active_id = -1;

    for (int i = 0; i < graph->node_count; i++) {
        int current_degree = 0;
        AdjListNode* current = graph->adjLists[i];

        while (current != NULL) {
            current_degree++;
            current = current->next;
        }

        if (current_degree > max_degree) {
            max_degree = current_degree;
            most_active_id = graph->nodes[i]->id;
        }
    }

    if (most_active_id != -1) {
        int idx = find_node_index(graph, most_active_id);
        char label[128];
        if (idx != -1) {
            node_get_display_label(graph->nodes[idx], label, sizeof(label));
            printf("\n[MERKEZILIK] En aktif: %s (ID:%d, baglanti:%d)\n", label, most_active_id, max_degree);
        } else {
            printf("\n[MERKEZILIK] En aktif dugum ID: %d (baglanti:%d)\n", most_active_id, max_degree);
        }
    }
}
