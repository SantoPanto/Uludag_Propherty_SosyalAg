#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_adj.h"    // 1. ve 2. Kiþi (Düðüm ve Graf Mimarisi)
#include "trie.h"         // 4. Kiþi (Metin Arama Motoru)
#include "hash_table.h"   // 3. Kiþi (Hýzlý Eriþim Karma Tablosu)
#include "kuyruk.h"        // 5. Kiþi (Senin Kuyruk Yapýn)

int main() {
    printf("====================================================\n");
    printf("   FAZ 1: TUM MODULLERIN ENTEGRASYON TESTI\n");
    printf("====================================================\n\n");

    // 1. TÜM SÝSTEMLERÝ BAÞLAT
    Graph* social_net = create_graph(10);
    TrieNode* search_engine = createTrieNode();
    HashTable* node_index = create_hash_table(HASH_TABLE_SIZE);
    Queue* task_queue = create_queue();

    // 2. DÜÐÜM OLUÞTUR VE ÖZELLÝK EKLE
    Node* user1 = create_node(101, USER);
    Node* photo1 = create_node(505, PHOTO);

    char* userName = "Ahmet Yilmaz";
    char* photoRes = "4K";
    add_property_to_node(user1, "Name", TYPE_STRING, userName);
    add_property_to_node(photo1, "Resolution", TYPE_STRING, photoRes);

    // 3. DÜÐÜMLERÝ ÝLGÝLÝ VERÝ YAPILARINA DAÐIT
    // a) Grafa ekle ve baðla
    add_node_to_graph(social_net, user1);
    add_node_to_graph(social_net, photo1);
    add_edge(social_net, 101, 505, LIKES, true); // Ahmet fotoðrafý beðendi

    // b) Arama Motoruna (Trie) indeksle
    insertToTrie(search_engine, userName, user1);

    // c) Hash Tablosuna ekle
    insert_to_hash(node_index, user1);
    insert_to_hash(node_index, photo1);

    printf("[+] Veriler Graf, Trie ve Hash Tablosuna basariyla islendi.\n\n");

    // ---------------------------------------------------------
    // 4. ENTEGRASYON SENARYOSU: HASH'TEN BUL -> KUYRUÐA AT
    // ---------------------------------------------------------
    printf("--- Senaryo: 505 ID'li fotografi bul ve isleme al ---\n");

    // Hash tablosundan anýnda eriþim
    Node* target_node = get_from_hash(node_index, 505);

    if (target_node != NULL) {
        printf("[OK] Hash Tablosu: %d ID'li dugum O(1) surede bulundu.\n", target_node->id);

        // Bulunan düðümü senin kuyruðuna (iþlem sýrasýna) ekle
        enqueue(task_queue, target_node);
        printf("[OK] Kuyruk: Dugum genislik oncelikli arama (BFS) kuyruguna alindi.\n");

        // Kuyruktan çýkar ve iþlemi tamamla
        Node* processed_node = dequeue(task_queue);
        printf("[OK] Islem Tamamlandi: Cikan dugum ID: %d\n", processed_node->id);
    } else {
        printf("[HATA] Dugum bulunamadi!\n");
    }

    // ---------------------------------------------------------
    // 5. BELLEK TEMÝZLÝÐÝ (MEMORY LEAK ÖNLEME)
    // ---------------------------------------------------------
    free_queue(task_queue);         // Kuyruðu temizle (5. Kiþi)
    free_hash_table(node_index);    // Hash kapsüllerini temizle (3. Kiþi)
    freeTrie(search_engine);        // Trie aðacýný temizle (4. Kiþi)
    free_graph(social_net);         // Grafý ve asýl Düðümleri temizle (1. ve 2. Kiþi)

    printf("\n[+] Tum bellek (Heap) guvenle temizlendi.\n");
    printf("====================================================\n");

    return 0;
}
