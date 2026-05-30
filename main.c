#include <stdio.h>
#include <stdlib.h>
#include "graph_adj.h"
#include "hash_table.h"
#include "trie.h"

// algorithms.c içerisindeki fonksiyonları burada tanıtıyoruz
void bfs_and_find_degrees(Graph* graph, int start_node_id);
void dfs(Graph* graph, int start_node_id);
void recommend_friends(Graph* graph, int target_user_id);
void dfs_full_network(Graph* graph);


int main() {
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

    // Belleği temizle
    free_graph(net);
    free_hash_table(ht);
    freeTrie(trie_root);
    printf("\n[+] Test tamamlandi. Tum bellek (Graf, Hash, Trie) temizlendi.\n");

    return 0;
}