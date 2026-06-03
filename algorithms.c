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

// Ağdaki tüm düğümleri eksiksiz tarayan Global DFS
void dfs_full_network(Graph* graph) {
    bool* visited = (bool*)calloc(graph->capacity, sizeof(bool));
    int component_count = 0;

    printf("\n--- Tum Agin Derinlik Oncelikli Taranmasi (Global DFS) ---\n");

    for (int i = 0; i < graph->node_count; i++) {
        if (!visited[i]) {
            component_count++;
            printf("\n>> Baglanti Bileseni %d Basliyor (Baslangic ID: %d):\n", component_count, graph->nodes[i]->id);
            dfs_util(graph, i, visited); // Mevcut yardımcı fonksiyonumuzu kullanıyoruz
        }
    }
    
    printf("\n[i] Agda toplam %d farkli bagimsiz grup (bilesen) bulundu.\n", component_count);
    free(visited);
}

void recommend_friends(Graph* graph, int target_user_id) {
    int target_idx = find_node_index(graph, target_user_id);
    if(target_idx == -1) return;

    bool* is_direct_friend = (bool*)calloc(graph->capacity, sizeof(bool));
    int* mutual_friend_scores = (int*)calloc(graph->capacity, sizeof(int)); // YENİ: Skor tablosu
    
    is_direct_friend[target_idx] = true; 
    
    // 1. Doğrudan arkadaşları bul ve işaretle
    AdjListNode* adj = graph->adjLists[target_idx];
    while (adj != NULL) {
        if (adj->edge->type == FRIEND) {
            int direct_friend_idx = find_node_index(graph, adj->edge->target_id);
            if (direct_friend_idx != -1) {
                is_direct_friend[direct_friend_idx] = true;
            }
        }
        adj = adj->next;
    }

    // 2. Arkadaşların arkadaşlarını bul ve SKORLARINI artır
    adj = graph->adjLists[target_idx];
    while (adj != NULL) {
        if (adj->edge->type == FRIEND) {
            int friend_idx = find_node_index(graph, adj->edge->target_id);
            if (friend_idx != -1) {
                AdjListNode* f_adj = graph->adjLists[friend_idx];
                while (f_adj != NULL) {
                    if (f_adj->edge->type == FRIEND) {
                        int fof_idx = find_node_index(graph, f_adj->edge->target_id);
                        // C kişisi doğrudan arkadaşımız değilse skorunu artır
                        if (fof_idx != -1 && !is_direct_friend[fof_idx]) {
                            mutual_friend_scores[fof_idx]++; // SKOR ARTIRIMI
                        }
                    }
                    f_adj = f_adj->next;
                }
            }
        }
        adj = adj->next;
    }

    // 3. Sonuçları listele (En az 1 ortak arkadaşı olanlar)
    printf("\n--- Kullanici %d Icin Arkadas Onerileri ---\n", target_user_id);
    bool suggestion_made = false;
    for (int i = 0; i < graph->node_count; i++) {
        if (mutual_friend_scores[i] > 0) {
            printf("[+] Oneri: Kullanici ID %d | Ortak Arkadas Sayisi: %d\n", graph->nodes[i]->id, mutual_friend_scores[i]);
            suggestion_made = true;
        }
    }

    if(!suggestion_made) printf("Su an icin yeni bir arkadas onerisi bulunamadi.\n");

    free(is_direct_friend);
    free(mutual_friend_scores);
}

// En Kısa Yolu Bulan ve Ekrana Çizen BFS Fonksiyonu
void find_shortest_path(Graph* graph, int start_node_id, int target_node_id) {
    int start_idx = find_node_index(graph, start_node_id);
    int target_idx = find_node_index(graph, target_node_id);

    if (start_idx == -1 || target_idx == -1) {
        printf("Baslangic veya hedef dugum bulunamadi!\n");
        return;
    }

    // Klasik ziyaret dizisi
    bool* visited = (bool*)calloc(graph->capacity, sizeof(bool));
    
    // YENİ: Her dügümün "nereden gelindiğini" tutan ebeveyn dizisi
    int* parent = (int*)malloc(graph->capacity * sizeof(int));
    for (int i = 0; i < graph->capacity; i++) parent[i] = -1; // -1: Henüz bir rotası yok

    Queue* q = create_queue();
    visited[start_idx] = true;
    enqueue(q, graph->nodes[start_idx]);

    bool found = false;

    // BFS Araması
    while (!is_empty(q)) {
        Node* current = dequeue(q);
        int curr_idx = find_node_index(graph, current->id);

        // Hedefe ulaştıysak tüm grafı taramaya gerek yok, çıkabiliriz.
        if (curr_idx == target_idx) {
            found = true;
            break; 
        }

        AdjListNode* adj = graph->adjLists[curr_idx];
        while (adj != NULL) {
            int neighbor_idx = find_node_index(graph, adj->edge->target_id);

            if (neighbor_idx != -1 && !visited[neighbor_idx]) {
                visited[neighbor_idx] = true;
                parent[neighbor_idx] = curr_idx; // Ekmek kırıntısını bırak (Yolu kaydet)
                enqueue(q, graph->nodes[neighbor_idx]);
            }
            adj = adj->next;
        }
    }
    
    // Temizlik (Kuyrukta kalanlar olabilir)
    free_queue(q);

    printf("\n--- En Kisa Yol Raporu: [ID %d -> ID %d] ---\n", start_node_id, target_node_id);

    if (found) {
        // Yolu geriye doğru (Hedekten -> Başlangıca) takip et
        int path[100]; // Geçici yol dizisi
        int path_length = 0;
        int curr = target_idx;

        while (curr != -1) {
            path[path_length++] = curr;
            curr = parent[curr]; // Bir önceki adıma git
        }

        // Diziye sondan başa eklediğimiz için, ekrana baştan sona (tersten) yazdırıyoruz
        printf("Rota: ");
        for (int i = path_length - 1; i >= 0; i--) {
            Node* n = graph->nodes[path[i]];
            
            // Eğer düğümün "Name" özelliği varsa ekrana onu yazdır, yoksa sadece ID yazdır
            char* name = "Isimsiz";
            if (n->property_count > 0 && n->properties[0].type == TYPE_STRING) {
                name = n->properties[0].value.s_val;
            }
            
            printf("%s", name);
            if (i > 0) printf(" -> ");
        }
        printf("\nBaglanti Mesafesi (Derece): %d\n", path_length - 1);
    } else {
        printf("Hedefe ulasilamadi! Düğümler arasinda bir baglanti yok.\n");
    }

    free(visited);
    free(parent);
}