<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_adj.h"    // 1. ve 2. Kişi (Düğüm ve Graf Mimarisi)
#include "trie.h"         // 4. Kişi (Metin Arama Motoru)
#include "hash_table.h"   // 3. Kişi (Hızlı Erişim Karma Tablosu)
#include "queue.h"        // 5. Kişi (Senin Kuyruk Yapın)

int main() {
    printf("====================================================\n");
    printf("   FAZ 1: TUM MODULLERIN ENTEGRASYON TESTI\n");
    printf("====================================================\n\n");

    // 1. TÜM SİSTEMLERİ BAŞLAT
    Graph* social_net = create_graph(10);
    TrieNode* search_engine = createTrieNode();
    HashTable* node_index = create_hash_table(100);
    Queue* task_queue = create_queue();

    // 2. DÜĞÜM OLUŞTUR VE ÖZELLİK EKLE
    Node* user1 = create_node(101, USER);
    Node* photo1 = create_node(505, PHOTO);

    char* userName = "Ahmet Yilmaz";
    char* photoRes = "4K";
    add_property_to_node(user1, "Name", TYPE_STRING, userName);
    add_property_to_node(photo1, "Resolution", TYPE_STRING, photoRes);

    // 3. DÜĞÜMLERİ İLGİLİ VERİ YAPILARINA DAĞIT
    // a) Grafa ekle ve bağla
    add_node_to_graph(social_net, user1);
    add_node_to_graph(social_net, photo1);
    add_edge(social_net, 101, 505, LIKES, true); // Ahmet fotoğrafı beğendi
    // Kenar ekleme testi geri bildirimi
printf("[OK] Kenar: 101 (Ahmet) ile 505 (Fotograf) arasinda LIKES iliskisi kuruldu.\n");

    // b) Arama Motoruna (Trie) indeksle
    insertToTrie(search_engine, userName, user1);

    // c) Hash Tablosuna ekle
    insert_to_hash(node_index, user1);
    insert_to_hash(node_index, photo1);

    printf("[+] Veriler Graf, Trie ve Hash Tablosuna basariyla islendi.\n\n");

    // ---------------------------------------------------------
    // 4. ENTEGRASYON SENARYOSU: HASH'TEN BUL -> KUYRUĞA AT
    // ---------------------------------------------------------
    printf("--- Senaryo: 505 ID'li fotografi bul ve isleme al ---\n");

    // Hash tablosundan anında erişim
    Node* target_node = get_from_hash(node_index, 505);

    if (target_node != NULL) {
        printf("[OK] Hash Tablosu: %d ID'li dugum O(1) surede bulundu.\n", target_node->id);

        // Bulunan düğümü senin kuyruğuna (işlem sırasına) ekle
        enqueue(task_queue, target_node);
        printf("[OK] Kuyruk: Dugum genislik oncelikli arama (BFS) kuyruguna alindi.\n");

        // Kuyruktan çıkar ve işlemi tamamla
        Node* processed_node = dequeue(task_queue);
        printf("[OK] Islem Tamamlandi: Cikan dugum ID: %d\n", processed_node->id);
    } else {
        printf("[HATA] Dugum bulunamadi!\n");
    }

    // ---------------------------------------------------------
    // 5. BELLEK TEMİZLİĞİ (MEMORY LEAK ÖNLEME)
    // ---------------------------------------------------------
    free_queue(task_queue);         // Kuyruğu temizle (5. Kişi)
    free_hash_table(node_index);    // Hash kapsüllerini temizle (3. Kişi)
    freeTrie(search_engine);        // Trie ağacını temizle (4. Kişi)
    free_graph(social_net);         // Grafı ve asıl Düğümleri temizle (1. ve 2. Kişi)

    printf("\n[+] Tum bellek (Heap) guvenle temizlendi.\n");
    printf("====================================================\n");

    return 0;
}
=======
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
    // algorithms.c içerisindeki fonksiyonları burada tanıtıyoruz
    void bfs_and_find_degrees(Graph* graph, int start_node_id);
    void dfs(Graph* graph, int start_node_id);
    void recommend_friends(Graph* graph, int target_user_id);
    void dfs_full_network(Graph* graph);
    void find_shortest_path(Graph* graph, int start_node_id, int target_node_id);

    // Arayuz fonksiyonlarini derleyiciye manuel tanitiyoruz
    void init_graphics_window();
    void draw_graph_network(Graph* graph, TrieNode* trie_root, Node** selected_node, char* search_text_buffer);
    void close_graphics_window();

    

    int main() {

        printf("=== FAZ 2: PERFORMANS VE DARBOGAZ TESTLERI ===\n\n");

        // Test parametreleri (Ölçeklenebilirliği görmek için bu sayıları artırarak testler yapmalısınız)
        int test_users = 10;
        int test_photos = 10;
        int test_events = 10;
        int max_nodes = test_users + test_photos + test_events + 100;

        my_graph = create_graph(max_nodes);
        HashTable* ht = create_hash_table(max_nodes * 2);    // Çakışmaları azaltmak için kapasiteyi büyük tutuyoruz
        TrieNode* trie_root = Boran_createTrieNode();

        clock_t start, end;
        double cpu_time_used;

        // --- TEST 1: VERİ YÜKLEME SÜRESİ ---
        printf("[1] Veri Yukleme Testi Basliyor (%d Dugum)...\n", max_nodes);
        start = clock();
        init_synthetic_data(my_graph, ht, trie_root, test_users, test_photos, test_events);
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
        find_shortest_path(my_graph, src_node, dest_node);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("[-] BFS En Kisa Yol Bulma Suresi: %f saniye\n\n", cpu_time_used);

    // --- EMINE: FAZ 2 SORGULARI ---
        find_most_active_node(my_graph);
        find_friends_events_photos(my_graph, 1);


        // --- FAZ 3: ARAYÜZ (UI) BAŞLATMA ---
        printf("\n[+] Arayuz baslatiliyor... Lutfen bekleyin.\n");


        Node* selected_node = NULL;
        char search_text_buffer[64] = "";

        init_graphics_window();

        // Pencere çarpıdan kapatılana kadar döner
        while (!WindowShouldClose()) {
            // BORAN: trie_root değişkeni arayüzü çizen fonksiyona gönderildi!
            draw_graph_network(my_graph, trie_root, &selected_node, search_text_buffer);
        }

        close_graphics_window();
        // ------------------------------------

        // Belleği temizleme
        free_graph(my_graph);
        free_hash_table(ht);
        Boran_freeTrie(trie_root);

        printf("\n[+] Testler tamamlandi ve bellek temizlendi.\n");

        return 0;

    }
>>>>>>> main
