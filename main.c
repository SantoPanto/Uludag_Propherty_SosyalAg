#include <stdio.h>
#include <stdlib.h>
#include "graph_models.h"

int main() {
    printf("--- Faz 1: Izole Test Basliyor ---\n\n");

    // 1. "User" düğümü oluştur
    Node* my_user = create_node(1, USER);
    printf("Dugum olusturuldu: ID = %d, Tip = USER\n", my_user->id);

    // 2. Özellikleri ekle
    // Not: add_property_to_node fonksiyonu void* beklediği için verilerin bellek adreslerini (&) göndermeliyiz.
    char* name_val = "Ahmet";
    int age_val = 21;

    add_property_to_node(my_user, "Name", STRING, name_val);
    add_property_to_node(my_user, "Age", INTEGER, &age_val);
    
    printf("Ozellikler basariyla eklendi.\n\n");

    // 3. Özellikleri terminale yazdir
    printf("--- Dugum Ozellikleri ---\n");
    for (int i = 0; i < my_user->property_count; i++) {
        Property prop = my_user->properties[i];
        
        // Veri tipine göre doğru union değerini okuyoruz
        if (prop.type == STRING) {
            printf("- %s: %s\n", prop.name, prop.value.s_val);
        } else if (prop.type == INTEGER) {
            printf("- %s: %d\n", prop.name, prop.value.i_val);
        }
    }
    printf("-------------------------\n\n");

    // 4. Belleği temizle (Destructor)
    free_node(my_user);
    printf("Bellek basariyla temizlendi (free_node calisti).\n");

    return 0;
}