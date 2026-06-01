#include "raylib.h"
#include "graph_adj.h"
#include "ui_integration.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
float TextToFloat(const char *text) {
    return (float)atof(text);
}
// Raygui sadece burada implemente edilecek!
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Yardýmcý Fonksiyon: Seçili düðümün özelliklerini metne çevirir
void Boran_format_side_panel_text(Node* node, char* buffer, int max_len) {
    if (node == NULL) {
        snprintf(buffer, max_len, "Lutfen haritadan bir dugume tiklayin.");
        return;
    }

    // Düðümün ID ve Tipini yaz
    char* type_str = (node->type == USER) ? "Kullanici" : (node->type == PHOTO) ? "Fotograf" : "Etkinlik";
    int offset = snprintf(buffer, max_len, "ID: %d\nTip: %s\n\n--- OZELLIKLER ---\n", node->id, type_str);

    // Düðümün içindeki dinamik özellikleri (Name, Age vb.) alt alta ekle
    for (int i = 0; i < node->property_count; i++) {
        if (offset >= max_len) break;

        Property* p = &node->properties[i];
        if (p->type == TYPE_STRING) {
            offset += snprintf(buffer + offset, max_len - offset, "%s: %s\n", p->name, p->value.s_val);
        } else if (p->type == TYPE_INTEGER) {
            offset += snprintf(buffer + offset, max_len - offset, "%s: %d\n", p->name, p->value.i_val);
        }
    }
}

// Boran'ýn Ana Arayüz Çizim Fonksiyonu
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height) {
    // Panel boyutlarini dinamik ayarlayalim (Ekranin sag tarafinda 350 piksel genislikte)
    int panel_width = 350;
    int panel_x = screen_width - panel_width;

    // 1. Ana Yan Paneli Ciz
    GuiPanel((Rectangle){ panel_x, 0, panel_width, screen_height }, "Sosyal Ag Yonetim Paneli");

    // 2. Arama Cubugunu (Search Bar) Ciz
    static bool search_edit_mode = false;
    if (GuiTextBox((Rectangle){ panel_x + 20, 40, panel_width - 40, 30 }, search_text_buffer, 64, search_edit_mode)) {
        search_edit_mode = !search_edit_mode; // Tiklandiginda yazi yazma modunu ac/kapat
    }

    // Arama cubugu etiketi
    DrawText("Kullanici veya Etkinlik Ara:", panel_x + 20, 20, 10, DARKGRAY);

    // 3. Dugum Detaylarini Goster
    char detail_text[1024] = {0};

    // Formatlayiciyi cagiriyoruz
    Boran_format_side_panel_text(selected_node, detail_text, 1024);

    // Formatlanmis metni ekranin sag paneline bas
    DrawText(detail_text, panel_x + 20, 100, 16, BLACK);
}
