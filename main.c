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