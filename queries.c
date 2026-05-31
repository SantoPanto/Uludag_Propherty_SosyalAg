#include <stdio.h>
#include <stdlib.h>
#include "queries.h"
#include "graph_models.h"

// 1. Cok Adimli Filtreleme: Kullanici -> Arkadas -> Etkinlik -> Fotograf
void find_friends_events_photos(Graph* graph, int start_user_id) {
    int user_idx = find_node_index(graph, start_user_id);
    if (user_idx == -1) {
        printf("Hata: %d ID'li kullanici bulunamadi.\n", start_user_id);
        return;
    }

    printf("\n--- %d ID'li Kullanicinin Agindaki Fotograflar ---\n", start_user_id);

    AdjListNode* friend_edge = graph->adjLists[user_idx];
    while (friend_edge != NULL) {
        if (friend_edge->edge != NULL && friend_edge->edge->type == 1) { // 1: EDGE_FRIEND
            int friend_id = friend_edge->edge->target_id;
            int friend_idx = find_node_index(graph, friend_id);

            // GÜVENLÝK DUVARI 1: Eðer arkadaþ gerçekten grafta varsa içine gir
            if (friend_idx != -1) {
                AdjListNode* event_edge = graph->adjLists[friend_idx];
                while (event_edge != NULL) {
                    if (event_edge->edge != NULL && event_edge->edge->type == 2) { // 2: EDGE_ATTENDED
                        int event_id = event_edge->edge->target_id;
                        int event_idx = find_node_index(graph, event_id);

                        // GÜVENLÝK DUVARI 2: Eðer etkinlik gerçekten grafta varsa içine gir
                        if (event_idx != -1) {
                            AdjListNode* photo_edge = graph->adjLists[event_idx];
                            while (photo_edge != NULL) {
                                if (photo_edge->edge != NULL && photo_edge->edge->type == 3) { // 3: EDGE_HAS_PHOTO
                                    printf("  [+] Bulunan Fotograf ID: %d (Etkinlik: %d, Arkadas: %d)\n",
                                           photo_edge->edge->target_id, event_id, friend_id);
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
}

// 2. Merkezilik: En aktif dugumu bulma
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
        printf("\n[MERKEZILIK ANALIZI] Agin En Aktif Dugumu ID: %d (Toplam Baglanti: %d)\n", most_active_id, max_degree);
    }
}
