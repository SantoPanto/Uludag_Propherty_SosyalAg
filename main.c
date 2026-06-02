#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graph_adj.h"
#include "hash_table.h"
#include "trie.h"
#include "data_generator.h"
#include "queries.h"
#include "ui_render.h"
#include "raylib.h"

// algorithms.c içerisindeki fonksiyonları burada tanıtıyoruz
void bfs_and_find_degrees(Graph* graph, int start_node_id);
void dfs(Graph* graph, int start_node_id);
void recommend_friends(Graph* graph, int target_user_id);
void dfs_full_network(Graph* graph);
void find_shortest_path(Graph* graph, int start_node_id, int target_node_id);

// Arayuz fonksiyonlarini derleyiciye manuel tanitiyoruz
void init_graphics_window();
void draw_graph_network(Graph* graph, TrieNode* trie_root); 
void close_graphics_window();

int main() {

    printf("=== FAZ 2: PERFORMANS VE DARBOGAZ TESTLERI ===\n\n");

    // Test parametreleri (Ölçeklenebilirliği görmek için bu sayıları artırarak testler yapmalısınız)
    int test_users = 50;
    int test_photos = 20;
    int test_events = 10;
    int max_nodes = test_users + test_photos + test_events + 100;

    Graph* net = create_graph(max_nodes);
    HashTable* ht = create_hash_table(max_nodes * 2); // Çakışmaları azaltmak için kapasiteyi büyük tutuyoruz
    TrieNode* trie_root = Boran_createTrieNode();

    clock_t start, end;
    double cpu_time_used;

    // --- TEST 1: VERİ YÜKLEME SÜRESİ ---
    printf("[1] Veri Yukleme Testi Basliyor (%d Dugum)...\n", max_nodes);
    start = clock();
    init_synthetic_data(net, ht, trie_root, test_users, test_photos, test_events);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[-] Veri Yukleme Suresi: %f saniye\n\n", cpu_time_used);

    // --- TEST 2: HASH TABLE O(1) ARAMA TESTİ ---
    // İşlemler çok hızlı gerçekleşeceği için 10.000 kere arama yapıp toplam süreyi ölçüyoruz
    printf("[2] Hash Table Stres Testi (10.000 Arama)...\n");
    start = clock();
    for(int i = 0; i < 10000; i++) {
        int random_id = (rand() % test_users) + 1;
        get_from_hash(ht, random_id);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[-] Hash Table 10K Arama Suresi: %f saniye\n\n", cpu_time_used);

    // --- TEST 3: TRIE PREFIX ARAMA TESTİ ---
    printf("[3] Trie Otomatik Tamamlama Testi...\n");
    start = clock();
    Boran_autocomplete(trie_root, "User_100"); // "User_100", "User_1000" gibi eşleşmeleri bulacak
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[-] Trie Arama Suresi: %f saniye\n\n", cpu_time_used);

    // --- TEST 4: GRAF TRAVERSAL (BFS - EN KISA YOL) TESTİ ---
    printf("[4] Graf BFS (En Kisa Yol) Testi...\n");
    int src_node = 1;
    int dest_node = test_users / 2; // Ağın ortalarından bir kullanıcı seçelim
    start = clock();
    find_shortest_path(net, src_node, dest_node);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("[-] BFS En Kisa Yol Bulma Suresi: %f saniye\n\n", cpu_time_used);

// --- EMINE: FAZ 2 SORGULARI ---
    find_most_active_node(net);
    find_friends_events_photos(net, 1);


    // --- FAZ 3: ARAYÜZ (UI) BAŞLATMA ---
    printf("\n[+] Arayuz baslatiliyor... Lutfen bekleyin.\n");

    init_graphics_window();

    // Pencere çarpıdan kapatılana kadar döner
    while (!WindowShouldClose()) {
        // BORAN: trie_root değişkeni arayüzü çizen fonksiyona gönderildi!
        draw_graph_network(net, trie_root);
    }

    close_graphics_window();
    // ------------------------------------

    // Belleği temizleme
    free_graph(net);
    free_hash_table(ht);
    Boran_freeTrie(trie_root);

    printf("\n[+] Testler tamamlandi ve bellek temizlendi.\n");

    return 0;

}
