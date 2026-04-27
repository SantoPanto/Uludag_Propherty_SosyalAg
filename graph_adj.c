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

    // Yeni bir bağlı liste düğümü oluştur (malloc)
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));

    // Kenar verilerini arkadaşının struct yapısına göre doldur
    newNode->edge.source_id = src_id;
    newNode->edge.target_id = dest_id;
    newNode->edge.type = type;
    newNode->edge.properties = NULL;
    newNode->edge.property_count = 0;

    // BAĞLI LİSTE MANTIRI: Yeni elemanı listenin başına ekle (Pointer manipülasyonu)
    newNode->next = graph->adjLists[src_idx];
    graph->adjLists[src_idx] = newNode;

    // Eğer yönsüzse (FRIEND gibi), tersi yönde de bir kenar ekle
    if (!is_directed) {
        add_edge(graph, dest_id, src_id, type, true);
    }
}

// Kenara dinamik özellik ekleme (Arkadaşının mantığıyla uyumlu)
void add_property_to_edge(Edge* edge, const char* key, DataType type, void* value) {
    edge->properties = (Property*)realloc(edge->properties, (edge->property_count + 1) * sizeof(Property));
    Property* new_prop = &edge->properties[edge->property_count];

    #ifdef _MSC_VER
        new_prop->name = _strdup(key);
    #else
        new_prop->name = strdup(key);
    #endif

    new_prop->type = type;

    switch (type) {
        case INTEGER: new_prop->value.i_val = *(int*)value; break;
        case FLOAT:   new_prop->value.f_val = *(float*)value; break;
        case BOOLEAN: new_prop->value.b_val = *(int*)value; break;
        case STRING:
            #ifdef _MSC_VER
                new_prop->value.s_val = _strdup((char*)value);
            #else
                new_prop->value.s_val = strdup((char*)value);
            #endif
            break;
    }
    edge->property_count++;
}

// Tüm grafı ve bağlı listeleri temizler (Bellek Yönetimi)
void free_graph(Graph* graph) {
    if (!graph) return;
    for (int i = 0; i < graph->node_count; i++) {
        AdjListNode* current = graph->adjLists[i];
        while (current != NULL) {
            AdjListNode* temp = current;
            current = current->next;

            // Kenar içindeki dinamik özellikleri temizle
            for(int j=0; j < temp->edge.property_count; j++) {
                free(temp->edge.properties[j].name);
                if(temp->edge.properties[j].type == STRING) free(temp->edge.properties[j].value.s_val);
            }
            free(temp->edge.properties);
            free(temp);
        }
        // 1. kişinin free_node fonksiyonunu kullanarak düğümü temizle
        free_node(graph->nodes[i]);
    }
    free(graph->nodes);
    free(graph->adjLists);
    free(graph);
}
