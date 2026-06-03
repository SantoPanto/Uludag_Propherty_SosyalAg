#include "raylib.h"
#include "graph_adj.h"
#include "ui_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "graph_models.h"
#include "trie.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// --- Global Değişkenler ve Prototip ---
extern Graph* my_graph;
extern char ai_result_buffer[256];
extern int ai_is_fetching;
extern int ai_new_data_ready;
void* fetch_ai_user_thread(void* arg);

// --- Yardımcı Fonksiyon (Tanımlama) ---
void Boran_format_side_panel_text(Node* node, char* buffer, int max_len) {
    if (node == NULL) {
        snprintf(buffer, max_len, "Lutfen bir dugume tiklayin.");
        return;
    }
    // Düğümün adını buffer'a yazdıralım
    for (int i = 0; i < node->property_count; i++) {
        if (strcmp(node->properties[i].name, "Name") == 0) {
            snprintf(buffer, max_len, "Kullanici: %s", node->properties[i].value.s_val);
            return;
        }
    }
    snprintf(buffer, max_len, "ID: %d (Ozellik yok)", node->id);
}

// --- Ana Arayüz Çizim Fonksiyonu ---
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height) {
    int panel_width = 350;
    int panel_x = screen_width - panel_width;

    // 1. Ana Yan Paneli Çiz
    GuiPanel((Rectangle){ (float)panel_x, 0, (float)panel_width, (float)screen_height }, "Sosyal Ag Yonetim Paneli");

    // Arama Başlığı
    DrawText("Kullanici veya Etkinlik Ara:", panel_x + 20, 40, 15, DARKGRAY);

    // 2. YAPAY ZEKA BUTONU
    if (ai_is_fetching) {
        GuiDisable();
        GuiButton((Rectangle){ (float)panel_x + 20, 80, (float)panel_width - 40, 30 }, "AI İle Kullanici Uretiliyor...");
        GuiEnable();
    } else {
        if (GuiButton((Rectangle){ (float)panel_x + 20, 80, (float)panel_width - 40, 30 }, "AI İle Yeni Kullanici Uret")) {
            ai_is_fetching = 1;
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, fetch_ai_user_thread, NULL);
            pthread_detach(thread_id);
        }
    }

    // 3. YENİ VERİ GELDİ Mİ KONTROLÜ
    if (ai_new_data_ready == 1) {
        ai_new_data_ready = 0; 
        
        char name[50];
        char *token = strtok(ai_result_buffer, "|");
        if (token != NULL) strcpy(name, token);

        // Düğümü oluştur ve koordinatları ata
        static int ai_user_id_counter = 1000;
        Node* new_user = create_node(ai_user_id_counter++, USER);
        
        // Rastgele koordinat ataması
        new_user->x = (float)(rand() % 1200) - 600;
        new_user->y = (float)(rand() % 800) - 400;

        add_property_to_node(new_user, "Name", TYPE_STRING, name);

        // Graf'a ekle
        add_node_to_graph(my_graph, new_user); 
        
        printf("[+] AI Kullanicisi Grafa Eklendi: %s (x:%.1f, y:%.1f)\n", name, new_user->x, new_user->y);
    }

    // 4. Düğüm Detaylarını Göster
    char detail_text[1024] = {0};
    Boran_format_side_panel_text(selected_node, detail_text, 1024);
    
    // Şimdi "detail_text" burada tanımlı ve erişilebilir!
    DrawText(detail_text, panel_x + 20, 140, 16, BLACK);
} // <--- Bu parantez Boran_draw_ui_panel fonksiyonunu kapatıyor.