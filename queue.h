#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include "graph_models.h" // 1. kiþinin Node yapýsýný kullanmak için
#include <stdbool.h>

// Kuyruk elemaný: Graf düðümünün adresini ve bir sonraki kuyruk elemanýný tutar
typedef struct QueueNode {
    Node* data;
    struct QueueNode* next;
} QueueNode;

// Ana Kuyruk Yapýsý
typedef struct {
    QueueNode* front; // Kuyruktan çýkýþ ucu
    QueueNode* rear;  // Kuyruða giriþ ucu
} Queue;

// Kuyruk Fonksiyon Prototipleri
Queue* create_queue();
bool is_empty(Queue* q);
void enqueue(Queue* q, Node* node);
Node* dequeue(Queue* q);
void free_queue(Queue* q);

#endif // QUEUE_H_INCLUDED
