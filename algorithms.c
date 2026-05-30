#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "graph_adj.h"
#include "queue.h"

// Yardımcı fonksiyon: graph_adj.c'deki find_node_index'in prototipini kullanmalısın.
// Eğer graph_adj.h'da yoksa oraya eklemeyi unutma.
int find_node_index(Graph* graph, int id);

void bfs_and_find_degrees(Graph* graph, int start_node_id) {
    int start_idx = find_node_index(graph, start_node_id);
    if (start_idx == -1) {
        printf("Baslangic dugumu bulunamadi.\n");
        return;
    }

    // Ziyaret ve mesafe(derece) dizileri
    bool* visited = (bool*)calloc(graph->capacity, sizeof(bool));
    int* degrees = (int*)calloc(graph->capacity, sizeof(int));

    Queue* q = create_queue();
    
    // Başlangıç düğümü ayarları
    visited[start_idx] = true;
    degrees[start_idx] = 0; // Kendisiyle uzaklığı 0
    enqueue(q, graph->nodes[start_idx]);

    printf("--- %d ID'li Dugum Icin Katmanli Tarama (BFS) ---\n", start_node_id);

    while (!is_empty(q)) {
        Node* current = dequeue(q);
        int curr_idx = find_node_index(graph, current->id);
        
        printf("Dugum ID: %d, Baglanti Derecesi: %d\n", current->id, degrees[curr_idx]);

        // Komşuları gez
        AdjListNode* adj = graph->adjLists[curr_idx];
        while (adj != NULL) {
            int target_idx = find_node_index(graph, adj->edge->target_id);
            
            if (target_idx != -1 && !visited[target_idx]) {
                visited[target_idx] = true;
                degrees[target_idx] = degrees[curr_idx] + 1; // Bir alt katmana geçildi
                enqueue(q, graph->nodes[target_idx]);
            }
            adj = adj->next; // Sonraki komşuya geç
        }
    }

    free_queue(q);
    free(visited);
    free(degrees);
}

// DFS Yardımcı Rekürsif Fonksiyonu
void dfs_util(Graph* graph, int curr_idx, bool* visited) {
    // Mevcut düğümü işaretle ve yazdır
    visited[curr_idx] = true;
    Node* current = graph->nodes[curr_idx];
    printf("Ziyaret Edildi -> ID: %d (Tip: %d)\n", current->id, current->type);

    // Düğümün komşularını al (AdjList)
    AdjListNode* adj = graph->adjLists[curr_idx];
    while (adj != NULL) {
        int target_idx = find_node_index(graph, adj->edge->target_id);
        
        // Eğer komşu ziyaret edilmemişse derine in
        if (target_idx != -1 && !visited[target_idx]) {
            dfs_util(graph, target_idx, visited);
        }
        adj = adj->next;
    }
}

// DFS Ana Fonksiyonu
void dfs(Graph* graph, int start_node_id) {
    int start_idx = find_node_index(graph, start_node_id);
    if (start_idx == -1) return;

    bool* visited = (bool*)calloc(graph->capacity, sizeof(bool));
    
    printf("\n--- %d ID'li Dugum Icin Derinlik Oncelikli Arama (DFS) ---\n", start_node_id);
    dfs_util(graph, start_idx, visited);
    
    free(visited);
}

void recommend_friends(Graph* graph, int target_user_id) {
    int target_idx = find_node_index(graph, target_user_id);
    if(target_idx == -1) return;

    // Hızlı kontrol için hedef kullanıcının mevcut arkadaşlarını işaretleyelim
    bool* is_direct_friend = (bool*)calloc(graph->capacity, sizeof(bool));
    is_direct_friend[target_idx] = true; // Kendisini arkadaş listesine alma
    
    // İlk Tarama: Doğrudan arkadaşları bul ve işaretle
    AdjListNode* adj = graph->adjLists[target_idx];
    while (adj != NULL) {
        if (adj->edge->type == FRIEND) {
            // DİKKAT: Burada 'int' ile değişkeni tanımlıyoruz
            int direct_friend_idx = find_node_index(graph, adj->edge->target_id);
            if (direct_friend_idx != -1) {
                is_direct_friend[direct_friend_idx] = true;
            }
        }
        adj = adj->next;
    }

    printf("\n--- Kullanici %d Icin Arkadas Onerileri (Triadic Closure) ---\n", target_user_id);
    
    // İkinci Tarama: Arkadaşların arkadaşlarına (C kişilerine) bak
    adj = graph->adjLists[target_idx];
    bool suggestion_made = false;

    while (adj != NULL) {
        if (adj->edge->type == FRIEND) {
            // DİKKAT: friend_idx burada 'int' ile yeniden tanımlanıyor
            int friend_idx = find_node_index(graph, adj->edge->target_id);
            
            if (friend_idx != -1) {
                // Arkadaşın arkadaş listesine bak (C kişileri)
                AdjListNode* f_adj = graph->adjLists[friend_idx];
                while (f_adj != NULL) {
                    if (f_adj->edge->type == FRIEND) {
                        int fof_idx = find_node_index(graph, f_adj->edge->target_id);
                        
                        // C kişisi zaten A'nın arkadaşı değilse (veya kendisi değilse)
                        if (fof_idx != -1 && !is_direct_friend[fof_idx]) {
                            printf("[+] Oneri: %d (Ortak Arkadasiniz: %d)\n", f_adj->edge->target_id, adj->edge->target_id);
                            
                            is_direct_friend[fof_idx] = true; // Tekrar önermemek için işaretle
                            suggestion_made = true;
                        }
                    }
                    f_adj = f_adj->next;
                }
            }
        }
        adj = adj->next;
    }

    if(!suggestion_made) {
        printf("Su an icin yeni bir arkadas onerisi bulunamadi.\n");
    }

    free(is_direct_friend);
}