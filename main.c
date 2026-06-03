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
    printf("=== Property Graph: Sosyal Ag Analiz Sistemi ===\n\n");

    int test_users = 300;
    int test_photos = 10;
    int test_events = 10;
    int max_nodes = test_users + test_photos + test_events + 50;

    srand((unsigned int)time(NULL));

    my_graph = create_graph(max_nodes);
    g_ht = create_hash_table(max_nodes * 2);
    g_trie = Boran_createTrieNode();

    clock_t start, end;
    double cpu_time_used;

    printf("[1] Veri yukleme testi...\n");
    start = clock();
    init_synthetic_data(my_graph, g_ht, g_trie, test_users, test_photos, test_events);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[-] Yukleme suresi: %f sn\n\n", cpu_time_used);

    printf("[2] Hash table (10.000 arama)...\n");
    start = clock();
    for (int i = 0; i < 10000; i++) {
        int random_id = (rand() % test_users) + 1;
        get_from_hash(g_ht, random_id);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[-] Hash 10K arama: %f sn\n\n", cpu_time_used);

    printf("[3] Trie on ek arama ('ali')...\n");
    start = clock();
    Boran_autocomplete(g_trie, "ali");
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[-] Trie arama: %f sn\n\n", cpu_time_used);

    printf("[4] BFS en kisa yol (ID 1 -> %d)...\n", test_users / 2);
    start = clock();
    find_shortest_path(my_graph, 1, test_users / 2);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("[-] BFS: %f sn\n\n", cpu_time_used);

    find_most_active_node(my_graph);
    find_friends_events_photos(my_graph, 1);

    printf("\n[+] Arayuz baslatiliyor...\n");

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

    printf("\n[+] Program tamamlandi.\n");
    return 0;
}
