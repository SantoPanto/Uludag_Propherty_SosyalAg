#include "ui_render.h"

// Raygui'nin C dosyasinda derlenebilmesi icin bu makro sarttir
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Boran: Arayuzu (UI) ekrana cizdiren ana fonksiyon.
// 4. Gorevli kendi ana dongusunun icinde bu fonksiyonu cagiracak.
void Boran_draw_ui_panel(Node* selected_node, char* search_text_buffer, int screen_width, int screen_height) {
    
    // Panel boyutlarini dinamik ayarlayalim (Ekranin sag tarafinda 350 piksel genislikte)
    int panel_width = 350;
    int panel_x = screen_width - panel_width;
    
    // 1. Ana Yan Paneli Ciz
    GuiPanel((Rectangle){ panel_x, 0, panel_width, screen_height }, "Boran - Sosyal Ag Yonetim Paneli");

    // 2. Arama Cubugunu (Search Bar) Ciz
    // GuiTextBox, kullanicinin yazi yazmasini saglar. Eger tiklanirsa aktif olur.
    static bool search_edit_mode = false;
    if (GuiTextBox((Rectangle){ panel_x + 20, 40, panel_width - 40, 30 }, search_text_buffer, 64, search_edit_mode)) {
        search_edit_mode = !search_edit_mode; // Tiklandiginda yazi yazma modunu ac/kapat
    }
    
    // Arama cubugu etiketi
    DrawText("Kullanici veya Etkinlik Ara:", panel_x + 20, 20, 10, DARKGRAY);

    // 3. Dugum Detaylarini Goster
    char detail_text[1024] = {0};
    
    // Daha once yazdigimiz formatlayiciyi cagiriyoruz!
    Boran_format_side_panel_text(selected_node, detail_text, 1024);
    
    // Formatlanmis metni ekranin sag paneline, arama cubugunun altina bas
    DrawText(detail_text, panel_x + 20, 100, 14, BLACK);
}