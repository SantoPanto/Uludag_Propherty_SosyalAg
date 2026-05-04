#ifndef GRAPH_MODELS_H
#define GRAPH_MODELS_H

// --- ADIM 1: Sabitler ve Tip Tanımlamaları ---
typedef enum {
    USER,
    PHOTO,
    EVENT
} NodeType;

typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOLEAN
} DataType;

// --- ADIM 2: Esnek Özellik (Property) Mimarisi ---
typedef union {
    int i_val;
    float f_val;
    char* s_val;
    int b_val;
} PropertyValue;

typedef struct {
    char* name;          // Özelliğin adı (Örn: "Age", "Resolution")
    DataType type;       // Verinin tipi
    PropertyValue value; // Gerçek veri (Union sayesinde bellek tasarrufu sağlar)
} Property;

// --- ADIM 3: Temel Düğüm (Node) Struct'ı ---
typedef struct {
    int id;                 // Düğümün benzersiz kimliği
    NodeType type;          // Düğümün tipi (USER, PHOTO, EVENT)

    // Dinamik özellikler listesi
    Property* properties;   // Özellikleri tutacak dinamik dizi (malloc/realloc ile yönetilecek)
    int property_count;     // Dizide o an kaç özellik olduğunu tutar

    // DİKKAT: next pointer'ı eklenmedi. 2. kişi kendi AdjListNode yapısında bunu yönetecek.
} Node;

// --- ADIM 4: Kenar (Edge) Tipleri ---
typedef enum {
    FRIEND,     // Arkadaşlık ilişkisi (Örn: USER -> USER)
    LIKES,      // Beğenme ilişkisi (Örn: USER -> PHOTO)
    ATTENDS     // Katılım ilişkisi (Örn: USER -> EVENT)
} EdgeType;

//Temel Kenar (Edge) Struct'ı ---
typedef struct {
    int source_id;          // Kaynak düğümün ID'si
    int target_id;          // Hedef düğümün ID'si
    EdgeType type;          // Kenarın tipi (FRIEND, LIKES, ATTENDS)

    // Kenara ait dinamik özellikler (Örn: "Takipleşme Tarihi", "Etkileşim Ağırlığı")
    Property* properties;   // Özellikleri tutacak dinamik dizi
    int property_count;     // Kenarda kaç özellik olduğunu tutar
} Edge;



// Bellek Yönetimi Fonksiyon Prototipleri ---
Node* create_node(int id, NodeType type);
void add_property_to_node(Node* node, const char* key, DataType type, void* value);
void free_node(Node* node);

#endif // GRAPH_MODELS_H
