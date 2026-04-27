#ifndef GRAPH_MODELS_H
#define GRAPH_MODELS_H

// --- ADIM 1: Sabitler ve Tip Tanýmlamalarý ---
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
    char* name;          // Özelliðin adý (Örn: "Age", "Resolution")
    DataType type;       // Verinin tipi
    PropertyValue value; // Gerçek veri (Union sayesinde bellek tasarrufu saðlar)
} Property;

// --- ADIM 3: Temel Düðüm (Node) Struct'ý ---
typedef struct {
    int id;                 // Düðümün benzersiz kimliði
    NodeType type;          // Düðümün tipi (USER, PHOTO, EVENT)

    // Dinamik özellikler listesi
    Property* properties;   // Özellikleri tutacak dinamik dizi (malloc/realloc ile yönetilecek)
    int property_count;     // Dizide o an kaç özellik olduðunu tutar

    // DÝKKAT: next pointer'ý eklenmedi. 2. kiþi kendi AdjListNode yapýsýnda bunu yönetecek.
} Node;

// --- ADIM 4: Kenar (Edge) Tipleri ---
typedef enum {
    FRIEND,     // Arkadaþlýk iliþkisi (Örn: USER -> USER)
    LIKES,      // Beðenme iliþkisi (Örn: USER -> PHOTO)
    ATTENDS     // Katýlým iliþkisi (Örn: USER -> EVENT)
} EdgeType;

//Temel Kenar (Edge) Struct'ý ---
typedef struct {
    int source_id;          // Kaynak düðümün ID'si
    int target_id;          // Hedef düðümün ID'si
    EdgeType type;          // Kenarýn tipi (FRIEND, LIKES, ATTENDS)

    // Kenara ait dinamik özellikler (Örn: "Takipleþme Tarihi", "Etkileþim Aðýrlýðý")
    Property* properties;   // Özellikleri tutacak dinamik dizi
    int property_count;     // Kenarda kaç özellik olduðunu tutar
} Edge;



// Bellek Yönetimi Fonksiyon Prototipleri ---
Node* create_node(int id, NodeType type);
void add_property_to_node(Node* node, const char* key, DataType type, void* value);
void free_node(Node* node);

#endif // GRAPH_MODELS_H
