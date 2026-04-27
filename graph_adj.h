#ifndef GRAPH_ADJ_H_INCLUDED
#define GRAPH_ADJ_H_INCLUDED

#include "graph_models.h"
#include <stdbool.h>

// Komþuluk listesindeki her bir "bað" elemaný (Linked List Node)
typedef struct AdjListNode {
    Edge edge;                 // Tanýmlanan Edge verisi
    struct AdjListNode* next;  // Bir sonraki kenara iþaretçi
} AdjListNode;

// Ana Graf Yapýsý
typedef struct {
    Node** nodes;              // Düðümleri tutan dinamik dizi
    AdjListNode** adjLists;    // Her düðümün kenar listesinin baþý (head)
    int node_count;            // Mevcut düðüm sayýsý
    int capacity;              // Grafýn maksimum kapasitesi
} Graph;

// API Fonksiyonlarý
Graph* create_graph(int capacity);
void add_node_to_graph(Graph* graph, Node* node);
void add_edge(Graph* graph, int src_id, int dest_id, EdgeType type, bool is_directed);
void add_property_to_edge(Edge* edge, const char* key, DataType type, void* value);
void free_graph(Graph* graph);

#endif // GRAPH_ADJ_H_INCLUDED
