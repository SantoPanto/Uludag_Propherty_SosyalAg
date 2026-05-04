#ifndef QUEUE_H_INCLUDED
#define QUEUE_H_INCLUDED

#include "graph_models.h" // 1. kişinin Node yapısını kullanmak için
#include <stdbool.h>

// Kuyruk elemanı: Graf düğümünün adresini ve bir sonraki kuyruk elemanını tutar
typedef struct QueueNode {
    Node* data;
    struct QueueNode* next;
} QueueNode;

// Ana Kuyruk Yapısı
typedef struct {
    QueueNode* front; // Kuyruktan çıkış ucu
    QueueNode* rear;  // Kuyruğa giriş ucu
} Queue;

// Kuyruk Fonksiyon Prototipleri
Queue* create_queue();
bool is_empty(Queue* q);
void enqueue(Queue* q, Node* node);
Node* dequeue(Queue* q);
void free_queue(Queue* q);

#endif // QUEUE_H_INCLUDED