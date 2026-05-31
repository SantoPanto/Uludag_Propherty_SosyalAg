#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Projenizin ana veri yapısı başlık dosyaları
#include "graph_adj.h"
#include "hash_table.h"
#include "trie.h"

// Kendi başlık dosyamız
#include "data_generator.h"

void init_synthetic_data(Graph* net, HashTable* ht, TrieNode* trie_root, 
                         int user_count, int photo_count, int event_count) {
                         
    printf("[*] Sentetik veri uretimi basliyor...\n");
    
    // Rastgele sayi uretecini baslat
    srand((unsigned int)time(NULL));

    int current_id = 1; // ID'lerin benzersiz olmasi icin ortak sayac

    // --- 1. KULLANICI (USER) DÜĞÜMLERİ ---
    for (int i = 0; i < user_count; i++) {
        Node* u = create_node(current_id, USER);
        
        // Isim uret ve ozellik olarak ekle
        char* name_buffer = (char*)malloc(32 * sizeof(char));
        sprintf(name_buffer, "User_%d", current_id);
        add_property_to_node(u, "Name", TYPE_STRING, name_buffer);

        // Veri yapilarina kayit
        add_node_to_graph(net, u);
        insert_to_hash(ht, u);
        insertToTrie(trie_root, name_buffer, u); // Sadece isimleri Trie'ye ekliyoruz
        
        current_id++;
    }

    // --- 2. FOTOĞRAF (PHOTO) DÜĞÜMLERİ ---
    int photo_start_id = current_id;
    for (int i = 0; i < photo_count; i++) {
        Node* p = create_node(current_id, PHOTO);
        
        // Fotograf aciklamasi uret
        char* desc_buffer = (char*)malloc(32 * sizeof(char));
        sprintf(desc_buffer, "Photo_Desc_%d", current_id);
        add_property_to_node(p, "Description", TYPE_STRING, desc_buffer);

        // Veri yapilarina kayit (Trie'ye eklemiyoruz cunku fotograflar isme gore aranmayacak)
        add_node_to_graph(net, p);
        insert_to_hash(ht, p);
        
        current_id++;
    }

    // --- 3. ETKİNLİK (EVENT) DÜĞÜMLERİ ---
    int event_start_id = current_id;
    for (int i = 0; i < event_count; i++) {
        Node* e = create_node(current_id, EVENT);
        
        // Etkinlik basligi uret
        char* title_buffer = (char*)malloc(32 * sizeof(char));
        sprintf(title_buffer, "Event_Title_%d", current_id);
        add_property_to_node(e, "Title", TYPE_STRING, title_buffer);

        // Veri yapilarina kayit
        add_node_to_graph(net, e);
        insert_to_hash(ht, e);
        
        current_id++;
    }

    // --- 4. İLİŞKİLERİ (EDGES) ÜRETME ---
    
    // a) User -> FRIEND -> User (Her kullanicinin ortalama 3 arkadasi olsun)
    for (int i = 0; i < user_count * 3; i++) {
        int src = (rand() % user_count) + 1; // User ID'leri 1'den baslar
        int dest = (rand() % user_count) + 1;
        
        if (src != dest) {
            // Yonsuz arkadaslik iliskisi
            add_edge(net, src, dest, FRIEND, false);
        }
    }

    // b) User -> LIKES -> Photo (Her kullanici ortalama 5 fotograf begensin)
    for (int i = 0; i < user_count * 5; i++) {
        int src = (rand() % user_count) + 1;
        int dest_photo = photo_start_id + (rand() % photo_count);
        
        // Begenme yonlu (directed) bir iliskidir
        add_edge(net, src, dest_photo, LIKES, true); 
    }

    // c) User -> ATTENDS -> Event (Her kullanici ortalama 2 etkinlige katilsin)
    for (int i = 0; i < user_count * 2; i++) {
        int src = (rand() % user_count) + 1;
        int dest_event = event_start_id + (rand() % event_count);
        
        // Katilim yonlu (directed) bir iliskidir
        add_edge(net, src, dest_event, ATTENDS, true);
    }

    printf("[+] %d User, %d Photo, %d Event ve aralarindaki baglantilar basariyla sisteme yuklendi.\n", 
           user_count, photo_count, event_count);
}