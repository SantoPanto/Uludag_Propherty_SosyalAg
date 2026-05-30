#include <stdio.h>
#include <stdlib.h>
#include "graph_adj.h"

// algorithms.c içerisindeki fonksiyonları burada tanıtıyoruz
void bfs_and_find_degrees(Graph* graph, int start_node_id);
void dfs(Graph* graph, int start_node_id);
void recommend_friends(Graph* graph, int target_user_id);

int main() {
    printf("--- Faz 2: Temel Algoritmalar Entegrasyon Testi ---\n\n");

    // 10 kapasiteli bir graf oluşturuyoruz
    Graph* net = create_graph(10); 

    // 1. Düğümleri (Kullanıcıları) Oluştur
    Node* user_ali = create_node(1, USER);
    Node* user_mehmet  = create_node(2, USER);
    Node* user_suha = create_node(3, USER);
    Node* user_emre  = create_node(4, USER);

    // Düğümleri Grafa Ekle
    add_node_to_graph(net, user_ali);
    add_node_to_graph(net, user_suha);
    add_node_to_graph(net, user_mehmet);
    add_node_to_graph(net, user_emre);

    // 2. Kenarları (Arkadaşlıkları) Ekle
    // FRIEND tipi bağlantılar çift yönlü olacağı için is_directed = false veriyoruz.
    
    // Ali - Mehmet arkadaşlığı (1. Derece bağlantı)
    add_edge(net, 1, 2, FRIEND, false);
    
    // Mehmet - Süha arkadaşlığı (Mehmet üzerinden 2. derece bağlantı)
    add_edge(net, 2, 3, FRIEND, false);
    
    // Süha - Emre arkadaşlığı (Süha üzerinden 3. derece bağlantı)
    add_edge(net, 3, 4, FRIEND, false);

    // --- TEST 1: BFS ---
    bfs_and_find_degrees(net, 1);

    // --- TEST 2: DFS ---
    dfs(net, 1);

    // --- TEST 3: Triadic Closure ---
    recommend_friends(net, 1);

    // Belleği temizle ve kapat
    free_graph(net);
    printf("\n[+] Test tamamlandi. Tum bellek temizlendi.\n");

    return 0;
}