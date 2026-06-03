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

Graph* my_graph = NULL;
HashTable* g_ht = NULL;
TrieNode* g_trie = NULL;

void bfs_and_find_degrees(Graph* graph, int start_node_id);
void dfs(Graph* graph, int start_node_id);
void recommend_friends(Graph* graph, int target_user_id);
void dfs_full_network(Graph* graph);
void find_shortest_path(Graph* graph, int start_node_id, int target_node_id);

void init_graphics_window();
void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer);
void close_graphics_window();

int main(void) {
    printf("=== Property Graph: Sosyal Ag Analiz Sistemi (Interaktif Mod) ===\n\n");

    // İleride eklenecek düğümler için geniş bir kapasite (1000) ayırıyoruz
    int max_nodes = 1000; 

    srand((unsigned int)time(NULL));

    // Sistem başlangıcında içi bomboş olan veri yapılarını oluşturuyoruz
    my_graph = create_graph(max_nodes);
    g_ht = create_hash_table(max_nodes * 2);
    g_trie = Boran_createTrieNode();

    // NOT: Harita başlangıçta boş olacağı için eski otomatik performans testlerini
    // (BFS, Hash arama vb.) kapattık. Aksi takdirde boş veride arama yapmaya çalışıp çökebilirdi.
    
    printf("[+] Arayuz baslatiliyor... Lutfen dugumleri sag paneldeki butonlarla ekleyin.\n");

    Node* selected_node = NULL;
    char search_text_buffer[64] = "";

    init_graphics_window();

    while (!WindowShouldClose()) {
        draw_graph_network(my_graph, g_trie, &selected_node, search_text_buffer);
    }

    close_graphics_window();

    free_graph(my_graph);
    free_hash_table(g_ht);
    Boran_freeTrie(g_trie);

    printf("\n[+] Program tamamlandi ve bellek temizlendi.\n");
    return 0;
}