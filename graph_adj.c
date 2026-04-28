#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_adj.h"

// Graf iskeletini oluşturur
Graph* create_graph(int capacity) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    graph->node_count = 0;
    graph->capacity = capacity;
    graph->nodes = (Node**)malloc(capacity * sizeof(Node*));
    graph->adjLists = (AdjListNode**)malloc(capacity * sizeof(AdjListNode*));
    for (int i = 0; i < capacity; i++) {
        graph->adjLists[i] = NULL;
    }
    return graph;
}

// Yeni bir düğümü graf listesine ekler
void add_node_to_graph(Graph* graph, Node* node) {
    if (graph->node_count < graph->capacity) {
        graph->nodes[graph->node_count] = node;
        graph->node_count++;
    } else {
        fprintf(stderr, "Uyari: Graf kapasitesi dolu, yeni dugum eklenemedi!\n");
    }
}

// Yardımcı Fonksiyon: ID'ye göre düğümün dizideki indeksini bulur
int find_node_index(Graph* graph, int id) {
    for (int i = 0; i < graph->node_count; i++) {
        if (graph->nodes[i]->id == id) return i;
    }
    return -1;
}

// İki düğüm arasında bağ (Kenar) kurar
void add_edge(Graph* graph, int src_id, int dest_id, EdgeType type, bool is_directed) {
    int src_idx = find_node_index(graph, src_id);
    if (src_idx == -1) return;

    // Yeni bir bağlı liste düğümü oluştur
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));

    // 1. KİŞİNİN MİMARİSİ KULLANILIYOR: create_edge çağrıldı!
    newNode->edge = create_edge(src_id, dest_id, type);
    
    // BAĞLI LİSTE MANTIĞI: Yeni elemanı listenin başına ekle
    newNode->next = graph->adjLists[src_idx];
    graph->adjLists[src_idx] = newNode;

    // Eğer yönsüzse (FRIEND gibi), tersi yönde de bir kenar ekle
    if (!is_directed) {
        add_edge(graph, dest_id, src_id, type, true);
    }
}

// DİKKAT: add_property_to_edge fonksiyonu buradan silindi çünkü graph_models.c içinde zaten var!

// Tüm grafı ve bağlı listeleri temizler (Bellek Yönetimi)
void free_graph(Graph* graph) {
    if (!graph) return;
    
    for (int i = 0; i < graph->node_count; i++) {
        AdjListNode* current = graph->adjLists[i];
        while (current != NULL) {
            AdjListNode* temp = current;
            current = current->next;
            
            // 1. KİŞİNİN MİMARİSİ KULLANILIYOR: free_edge çağrıldı!
            free_edge(temp->edge); 
            
            free(temp); // Bağlı liste düğümünü sil
        }
        // 1. kişinin free_node fonksiyonunu kullanarak asıl düğümü temizle
        free_node(graph->nodes[i]);
    }
    
    free(graph->nodes);
    free(graph->adjLists);
    free(graph);
}