#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_models.h" // 1. Kişinin (Heterojen Modelleme) Modülü
#include "trie.h"         // 4. Kişinin (Metin Arama Motoru) Modülü

int main() {
    // ---------------------------------------------------------
    // BÖLÜM 1: SİSTEM BAŞLATMA
    // ---------------------------------------------------------
    
    // Arama Motorunu (Trie) kök düğüm ile başlatıyoruz
    TrieNode* root = createTrieNode();
    printf("Sistem Baslatiliyor... Veri Yapilari Olusturuluyor...\n");

    // ---------------------------------------------------------
    // BÖLÜM 2: GRAF DÜĞÜMLERİNİN (NODE) OLUŞTURULMASI
    // 1. Kişinin geliştirdiği API'ler kullanılarak ağa veri ekleniyor
    // ---------------------------------------------------------
    
    Node* user1 = create_node(1, USER);
    char* isim1 = strdup("Boran Mandaci");
    add_property_to_node(user1, "Name", TYPE_STRING, isim1); 

    Node* user2 = create_node(2, USER);
    char* isim2 = strdup("Bora Yilmaz");
    add_property_to_node(user2, "Name", TYPE_STRING, isim2);

    Node* event1 = create_node(3, EVENT);
    char* eventName = strdup("Uludag Bahar Senligi");
    add_property_to_node(event1, "Name", TYPE_STRING, eventName);

    // ---------------------------------------------------------
    // BÖLÜM 3: TRIE ENTEGRASYONU (ARAMA MOTORU İNDEKSLEME)
    // Yeni oluşturulan düğümler, isimleriyle birlikte arama ağacına (Trie) indekslenir.
    // Parametreler: Ağaç Kökü, İndekslenecek String (Name), Bağlanacak Düğüm Pointer'ı
    // ---------------------------------------------------------
    
    insertToTrie(root, isim1, user1);
    insertToTrie(root, isim2, user2);
    insertToTrie(root, eventName, event1);

    // ---------------------------------------------------------
    // BÖLÜM 4: SİSTEM TESTLERİ (FAZ 1 - ARAMA KONTROLLERİ)
    // ---------------------------------------------------------
    printf("\n--- OTOMATIK TAMAMLAMA (AUTOCOMPLETE) TESTLERI ---\n");

    // Test 1: Aynı önekten (prefix) türeyen farklı kayıtların başarılı listelenmesi
    // Beklenen: Boran ve Bora isimlerinin her ikisini de O(L) sürede getirmeli
    autocomplete(root, "bor");

    // Test 2: Büyük/Küçük harf duyarsızlık testi (Case-Insensitivity)
    // Beklenen: Orijinali "Uludag..." olan kaydı, "ulu" aramasıyla eşleştirebilmeli
    autocomplete(root, "ulu");

    // Test 3: Ağaçta bulunmayan bir kaydın aranması (Hata yönetimi testi)
    // Beklenen: Kayıt bulunamadı uyarısı vermeli, program çökmemeli (Segmentation fault olmamalı)
    autocomplete(root, "Ahmet");

    // ---------------------------------------------------------
    // BÖLÜM 5: BELLEK TEMİZLİĞİ VE GÜVENLİ ÇIKIŞ (MEMORY MANAGEMENT)
    // İşletim sisteminden alınan tüm Heap (Dinamik Bellek) alanları iade ediliyor
    // ---------------------------------------------------------
    
    freeTrie(root);  // Ağacın tamamını rekürsif olarak temizler
    
    // Özelliklere atanan string bellekleri boşaltılıyor
    free(isim1);
    free(isim2);
    free(eventName);
    
    // Not: Faz 1 tam entegre edildiğinde 1. kişinin free_node(user1) gibi 
    // düğüm temizleyici fonksiyonları da burada çağrılacaktır.

    return 0;
}