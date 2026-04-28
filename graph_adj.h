#ifndef GRAPH_ADJ_H_INCLUDED
#define GRAPH_ADJ_H_INCLUDED

#include "graph_models.h"
#include <stdbool.h>

// Komşuluk listesindeki her bir "bağ" elemanı (Linked List Node)
typedef struct AdjListNode {
    Edge* edge;                // DİKKAT: Pointer yapıldı!
    struct AdjListNode* next;  
} AdjListNode;

// Ana Graf Yapısı
typedef struct {
    Node** nodes;              // Düğümleri tutan dinamik dizi
    AdjListNode** adjLists;    // Her düğümün kenar listesinin başı (head)
    int node_count;            // Mevcut düğüm sayısı
    int capacity;              // Grafın maksimum kapasitesi
} Graph;

// API Fonksiyonları
Graph* create_graph(int capacity);
void add_node_to_graph(Graph* graph, Node* node);
void add_edge(Graph* graph, int src_id, int dest_id, EdgeType type, bool is_directed);
void free_graph(Graph* graph);

#endif // GRAPH_ADJ_H_INCLUDED