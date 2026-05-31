#include <stdio.h>
#include "ui_integration.h"

// Boran: Arayuzden (UI) bir dugume tiklandiginda cagrilacak fonksiyon.
// Faz 1'deki Hash Table'i kullanarak dugum detaylarini O(1) hizinda dondurur.
Node* Boran_get_node_details_for_ui(HashTable* ht, int node_id) {
    if (ht == NULL) return NULL;
    
    // Tiklanan dugumun IDsini hash tablosunda bul
    Node* clicked_node = get_from_hash(ht, node_id);
    
    if (clicked_node != NULL) {
        printf("[UI PANEL] %d ID'li dugum bilgileri arayuze gonderiliyor...\n", node_id);
    } else {
        printf("[UI PANEL HATA] %d ID'li dugum bulunamadi!\n", node_id);
    }
    
    return clicked_node;
}

// Boran: Kullanici arama cubuguna metin girdiginde cagrilacak fonksiyon.
// Faz 1'deki Trie (Onek Agaci) yapisini tetikleyerek UI'a sonuc uretir.
void Boran_get_autocomplete_results_for_ui(TrieNode* root, const char* prefix) {
    if (root == NULL || prefix == NULL) return;
    
    printf("[UI SEARCH BAR] '%s' icin arama sonuclari getiriliyor:\n", prefix);
    
    // Faz 1'deki orijinal arama fonksiyonunu cagir
    autocomplete(root, prefix);
}

// Boran: UI (Arayuz) yan panelinde gosterilecek metni hazirlar.
// Gelen dugumun tum ozelliklerini alt alta okunabilir bir string (metin) formatina cevirir.
void Boran_format_side_panel_text(Node* node, char* output_buffer, int buffer_size) {
    if (node == NULL || output_buffer == NULL) {
        snprintf(output_buffer, buffer_size, "Lutfen detaylarini gormek icin bir dugume tiklayin.");
        return;
    }

    // Baslangic bilgilerini yaz
    int current_len = snprintf(output_buffer, buffer_size, 
             "--- DUGUM DETAYLARI ---\n"
             "ID: %d\n"
             "Tip: %d (0:User, 1:Photo, 2:Event)\n"
             "-----------------------\n", 
             node->id, node->type);

    // Eger dugumun dinamik ozellikleri (Property) varsa, onlari da ekle
    if (node->property_count > 0 && node->properties != NULL) {
        for (int i = 0; i < node->property_count; i++) {
            // Buffer'in sinirini asmamak icin kalan boslugu kontrol et
            if (current_len >= buffer_size - 1) break; 
            
            Property* prop = &node->properties[i];
            
            // Arkadasinin Union yapisina uygun sekilde verileri arayuz icin hazirliyoruz
            if (prop->type == TYPE_STRING && prop->value.s_val != NULL) {
                current_len += snprintf(output_buffer + current_len, buffer_size - current_len, 
                                        "[*] %s: %s\n", prop->name, prop->value.s_val);
            } 
            else if (prop->type == TYPE_INTEGER) {
                current_len += snprintf(output_buffer + current_len, buffer_size - current_len, 
                                        "[*] %s: %d\n", prop->name, prop->value.i_val);
            } 
            else if (prop->type == TYPE_FLOAT) {
                current_len += snprintf(output_buffer + current_len, buffer_size - current_len, 
                                        "[*] %s: %.2f\n", prop->name, prop->value.f_val);
            }
            else if (prop->type == TYPE_BOOLEAN) {
                current_len += snprintf(output_buffer + current_len, buffer_size - current_len, 
                                        "[*] %s: %s\n", prop->name, prop->value.b_val ? "Evet" : "Hayir");
            }
        }
    } else {
        snprintf(output_buffer + current_len, buffer_size - current_len, "Bu dugume ait ekstra ozellik bulunmuyor.\n");
    }
}