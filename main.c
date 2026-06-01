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
void draw_graph_network(Graph* graph);
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
    TrieNode* trie_root = createTrieNode();

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
    autocomplete(trie_root, "User_100"); // "User_100", "User_1000" gibi eşleşmeleri bulacak
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
        // Senin değişkenin 'net' olduğu için burayı net olarak düzelttim
        draw_graph_network(net);
    }

    close_graphics_window();
    // ------------------------------------

    // Belleği temizleme
    free_graph(net);
    free_hash_table(ht);
    freeTrie(trie_root);

    printf("\n[+] Testler tamamlandi ve bellek temizlendi.\n");

    return 0;


    /*
    FATIHIN KODLAR
     printf("--- Faz 2 ve 3: Tam Entegrasyon Testi ---\n\n");

    Graph* net = create_graph(10);
    HashTable* ht = create_hash_table(20); // O(1) arama için Hash Table
    TrieNode* trie_root = createTrieNode(); // Otomatik tamamlama için Trie

    // 1. Düğümleri Oluştur ve İsim (Property) Ekle
    Node* u1 = create_node(1, USER);
    add_property_to_node(u1, "Name", TYPE_STRING, "Fatih Sahin");

    Node* u2 = create_node(2, USER);
    add_property_to_node(u2, "Name", TYPE_STRING, "Suha");

    Node* u3 = create_node(3, USER);
    add_property_to_node(u3, "Name", TYPE_STRING, "Aybey");

    Node* u4 = create_node(4, USER);
    add_property_to_node(u4, "Name", TYPE_STRING, "Emre");

    // İzole bir kullanıcı ekleyelim (Global DFS testi için)
    Node* u5 = create_node(5, USER);
    add_property_to_node(u5, "Name", TYPE_STRING, "Yabanci Kullanici");

    // Düğümleri Graf, Hash Table ve Trie'ye Kaydet (Üçlü Entegrasyon)
    Node* users[] = {u1, u2, u3, u4, u5};
    for(int i=0; i<5; i++) {
        add_node_to_graph(net, users[i]);
        insert_to_hash(ht, users[i]);

        // İsmi Trie'ye ekle (Property'den ismi çekiyoruz)
        char* name = users[i]->properties[0].value.s_val;
        insertToTrie(trie_root, name, users[i]);
    }

    // 2. Kenarları Ekle
    add_edge(net, 1, 2, FRIEND, false); // Fatih - Süha
    add_edge(net, 2, 3, FRIEND, false); // Süha - Aybey
    add_edge(net, 3, 4, FRIEND, false); // Aybey - Emre
    add_edge(net, 1, 4, FRIEND, false); // Fatih - Emre (Ortak arkadaş testini güçlendirmek için)

    // --- TEST 1: TRIE İLE İSİM ARAMA ---
    printf("\n=== TRIE OTOMATIK TAMAMLAMA TESTI ===\n");
    autocomplete(trie_root, "fa");  // "fa" yazan birine "fatih sahin" önermeli
    autocomplete(trie_root, "su");

    // --- TEST 2: HASH TABLE İLE HIZLI DOĞRULAMA ---
    printf("\n=== HASH TABLE O(1) ARAMA TESTI ===\n");
    Node* found = get_from_hash(ht, 3);
    if(found) printf("3 ID'li kullanici hizlica bulundu: %s\n", found->properties[0].value.s_val);

    // --- TEST 3: ORTAK ARKADAŞ SKORLAMASI ---
    printf("\n=== GELISMIS TRIADIC CLOSURE ===\n");
    recommend_friends(net, 2); // Süha için öneri isteyelim

    // --- TEST 4: KOPUK AĞLARI BULAN GLOBAL DFS ---
    printf("\n=== GLOBAL DFS ===\n");
    dfs_full_network(net); // 5. kullanıcının ayrı bir adada (bileşen) olduğunu göstermeli

    // --- TEST 5: EN KISA YOL (SHORTEST PATH) ---
    printf("\n=== EN KISA YOL BULUCU (BFS PATHFINDING) ===\n");
    // Süha ile Emre (4) arasındaki en kısa bağlantı rotasını bul
    find_shortest_path(net, 2, 4);

    // Belleği temizle
    free_graph(net);
    free_hash_table(ht);
    freeTrie(trie_root);
    printf("\n[+] Test tamamlandi. Tum bellek (Graf, Hash, Trie) temizlendi.\n");

    return 0;

    */

    /*

// --- EMINE: FAZ 2 SORGULARI ---
    find_most_active_node(net);
    find_friends_events_photos(net, 1);

// EGER BU YAZI EKRANA CIKARSA EMINENIN KODLARI KUSURSUZDUR
    printf("\n[+] >>> EMINE'NIN KODU SORUNSUZ CALISTI VE BITTI <<<\n\n");

    printf("[DEBUG] free_graph calisiyor...\n");
    free_graph(net);
    printf("[DEBUG] free_graph sorunsuz bitti!\n\n");

    printf("[DEBUG] free_hash_table calisiyor...\n");
    free_hash_table(ht);
    printf("[DEBUG] free_hash_table sorunsuz bitti!\n\n");

    printf("[DEBUG] freeTrie calisiyor...\n");
    freeTrie(trie_root);  // Eger seninki trie_root degilse projedeki adini yaz
    printf("[DEBUG] freeTrie sorunsuz bitti!\n\n");
    printf("[+] Testler tamamlandi ve bellek temizlendi.\n");

    return 0;
*/

}
