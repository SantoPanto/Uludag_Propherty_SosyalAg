#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph_adj.h"

static void ensure_id_map_capacity(Graph* graph, int id) {
    if (graph == NULL || graph->id_to_index == NULL) return;
    if (id < graph->id_map_size) return;

    int new_size = graph->id_map_size > 0 ? graph->id_map_size : 16;
    while (new_size <= id) {
        new_size *= 2;
    }

    int* new_map = (int*)realloc(graph->id_to_index, (size_t)new_size * sizeof(int));
    if (new_map == NULL) return;

    for (int i = graph->id_map_size; i < new_size; i++) {
        new_map[i] = -1;
    }
    graph->id_to_index = new_map;
    graph->id_map_size = new_size;
}

static void add_edge_date_property(Edge* edge, int day_offset);

static bool edge_exists(Graph* graph, int src_idx, int dest_id, EdgeType type) {
    AdjListNode* current = graph->adjLists[src_idx];
    while (current != NULL) {
        if (current->edge != NULL &&
            current->edge->target_id == dest_id &&
            current->edge->type == type) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// ID'ye göre düğüm indeksini bulan yardımcı fonksiyon
int find_node_index(Graph* graph, int id) {
    if (graph == NULL || graph->id_to_index == NULL) return -1;
    if (id < 0 || id >= graph->id_map_size) return -1;
    return graph->id_to_index[id];
}

// Graf oluşturma fonksiyonu
Graph* create_graph(int capacity) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;

    graph->node_count = 0;
    graph->capacity = capacity;

    graph->nodes = (Node**)malloc(capacity * sizeof(Node*));
    graph->adjLists = (AdjListNode**)malloc(capacity * sizeof(AdjListNode*));

    graph->id_map_size = capacity + 1;
    graph->id_to_index = (int*)malloc((size_t)graph->id_map_size * sizeof(int));
    if (!graph->nodes || !graph->adjLists || !graph->id_to_index) {
        free(graph->nodes);
        free(graph->adjLists);
        free(graph->id_to_index);
        free(graph);
        return NULL;
    }

    for (int i = 0; i < capacity; i++) {
        graph->adjLists[i] = NULL;
    }
    for (int i = 0; i < graph->id_map_size; i++) {
        graph->id_to_index[i] = -1;
    }
    return graph;
}

// Yeni bir düğümü graf listesine ekler
void add_node_to_graph(Graph* graph, Node* node) {
    if (graph == NULL || node == NULL) return;
    
    // Kapasite kontrolü ve genişletme
    if (graph->node_count >= graph->capacity) {
        int new_capacity = graph->capacity * 2;
        graph->nodes = (Node**)realloc(graph->nodes, new_capacity * sizeof(Node*));
        graph->adjLists = (AdjListNode**)realloc(graph->adjLists, new_capacity * sizeof(AdjListNode*));
        
        // Yeni ayrılan adjLists alanlarını NULL ile başlat
        for (int i = graph->capacity; i < new_capacity; i++) {
            graph->adjLists[i] = NULL;
        }
        graph->capacity = new_capacity;
    }

    ensure_id_map_capacity(graph, node->id);
    if (node->id < 0 || node->id >= graph->id_map_size) return;

    int index = graph->node_count;
    graph->nodes[index] = node;
    graph->id_to_index[node->id] = index;
    graph->node_count++;
}

// İki düğüm arasında bağ (Kenar) kurar
void add_edge(Graph* graph, int src_id, int dest_id, EdgeType type, bool is_directed) {
    int src_idx = find_node_index(graph, src_id);
    if (src_idx == -1) return;
    if (edge_exists(graph, src_idx, dest_id, type)) return;

    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    if (newNode == NULL) return;
    newNode->edge = (Edge*)malloc(sizeof(Edge));
    if (newNode->edge == NULL) {
        free(newNode);
        return;
    }

    newNode->edge->source_id = src_id;
    newNode->edge->target_id = dest_id;
    newNode->edge->type = type;
    newNode->edge->properties = NULL;
    newNode->edge->property_count = 0;
    add_edge_date_property(newNode->edge, src_id + dest_id + (int)type);

    newNode->next = graph->adjLists[src_idx];
    graph->adjLists[src_idx] = newNode;

    if (!is_directed) {
        add_edge(graph, dest_id, src_id, type, true);
    }
}

// Kenara dinamik özellik ekleme
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
        case TYPE_INTEGER: new_prop->value.i_val = *(int*)value; break;
        case TYPE_FLOAT:   new_prop->value.f_val = *(float*)value; break;
        case TYPE_BOOLEAN: new_prop->value.b_val = *(int*)value; break;
        case TYPE_STRING:
            #ifdef _MSC_VER
                new_prop->value.s_val = _strdup((char*)value);
            #else
                new_prop->value.s_val = strdup((char*)value);
            #endif
            break;
    }
    edge->property_count++;
}

static void add_edge_date_property(Edge* edge, int day_offset) {
    char date_buf[16];
    int month = 1 + (day_offset % 12);
    int day = 1 + (day_offset % 28);
    snprintf(date_buf, sizeof(date_buf), "2025-%02d-%02d", month, day);
    add_property_to_edge(edge, "since", TYPE_STRING, date_buf);
}

// Tüm grafı ve bağlı listeleri temizler
void free_graph(Graph* graph) {
    if (!graph) return;
    for (int i = 0; i < graph->node_count; i++) {
        AdjListNode* current = graph->adjLists[i];
        while (current != NULL) {
            AdjListNode* temp = current;
            current = current->next;

            // Kenar içindeki dinamik özellikleri temizle
            for(int j=0; j < temp->edge->property_count; j++) {
                free(temp->edge->properties[j].name);
                if(temp->edge->properties[j].type == TYPE_STRING) {
                    free(temp->edge->properties[j].value.s_val);
                }
            }
            free(temp->edge->properties);
            free(temp->edge);
            free(temp);
        }
    }
    free(graph->nodes);
    free(graph->adjLists);
    free(graph->id_to_index);
    free(graph);
}