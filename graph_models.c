#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_models.h" // Kendi başlık dosyamızı dahil ediyoruz

// --- 1. Düğüm Oluşturma (Constructor) ---
// Yeni bir düğüm için bellekte (heap) yer açar ve başlangıç değerlerini atar[cite: 60].
Node* create_node(int id, NodeType type) {
    // 1. Düğüm için bellekte (heap) alan tahsis et (malloc)
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Hata: Düğüm için bellek tahsis edilemedi!\n");
        exit(EXIT_FAILURE); // Bellek kalmadıysa programı güvenle durdur
    }

    // 2. Başlangıç atamalarını yap
    new_node->id = id;
    new_node->type = type;
    new_node->properties = NULL; // Başlangıçta özellik yok
    new_node->property_count = 0;

    return new_node;
}

// --- 2. Düğüme Dinamik Özellik Ekleme ---
// Düğüme yeni bir özellik (anahtar-değer) ekler ve diziyi büyütür[cite: 61].
void add_property_to_node(Node* node, const char* key, DataType type, void* value) {
    if (node == NULL || key == NULL || value == NULL) return;

    // 1. Özellikler dizisini 1 eleman büyütecek şekilde yeniden boyutlandır (realloc)
    node->properties = (Property*)realloc(node->properties, (node->property_count + 1) * sizeof(Property));
    if (node->properties == NULL) {
        fprintf(stderr, "Hata: Özellik eklenirken bellek genişletilemedi!\n");
        exit(EXIT_FAILURE);
    }

    // 2. Yeni eklenecek özelliğin bellekteki yerini (referansını) al
    Property* new_prop = &node->properties[node->property_count];

    // 3. Anahtarı (Key) kopyala
    // _strdup, Visual Studio (MSVC) ve POSIX sistemlerinde metnin kopyasını heap'te oluşturur
    #ifdef _MSC_VER
        new_prop->name = _strdup(key);
    #else
        new_prop->name = strdup(key);
    #endif

    new_prop->type = type;

    // 4. void* tipindeki genel pointer'ı, gelen veri tipine göre çevirip Union'a ata
    switch (type) {
        case TYPE_INTEGER:
            new_prop->value.i_val = *(int*)value;
            break;
        case TYPE_FLOAT:
            new_prop->value.f_val = *(float*)value;
            break;
        case TYPE_BOOLEAN:
            new_prop->value.b_val = *(int*)value;
            break;
        case TYPE_STRING:
            // Eğer veri metin ise, değerin (value) de bir kopyasını almalıyız
            #ifdef _MSC_VER
                new_prop->value.s_val = _strdup((char*)value);
            #else
                new_prop->value.s_val = strdup((char*)value);
            #endif
            break;
    }

    // 5. Eleman sayısını artır
    node->property_count++;
}

// --- 3. Düğümü Silme (Destructor) ---
// Düğümü ve içindeki tüm dinamik dizileri/metinleri serbest bırakır (free eder). En kritik fonksiyondur[cite: 62].
void free_node(Node* node) {
    if (node == NULL) return;

    // 1. Düğümün içindeki dinamik özellikleri temizle
    for (int i = 0; i < node->property_count; i++) {
        free(node->properties[i].name); // Özellik adını (key) sil

        // Eğer özellik bir metin (STRING) ise, strdup ile kopyalanan s_val'i de sil
        if (node->properties[i].type == TYPE_STRING) {
            free(node->properties[i].value.s_val);
        }
    }

    // 2. Özellikleri tutan dizinin kendisini sil
    if (node->properties != NULL) {
        free(node->properties);
    }

    // 3. Son olarak ana düğümü sil
    free(node);
}
