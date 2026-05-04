#include <stdio.h>
#include <stdlib.h>
#include "queue.h" // Güncellendi: Artık queue.h dosyasını çağırıyor

// Kuyruğu başlatır
Queue* create_queue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) return NULL;
    q->front = q->rear = NULL;
    return q;
}

// Kuyruğun boş olup olmadığını kontrol eder
bool is_empty(Queue* q) {
    return (q->front == NULL);
}

// Kuyruğa eleman ekler (Sona ekleme - FIFO)
void enqueue(Queue* q, Node* node) {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    if (!temp) {
        fprintf(stderr, "Hata: Kuyruk elemani icin bellek yok!\n");
        return;
    }
    temp->data = node;
    temp->next = NULL;

    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;
}

// Kuyruktan eleman çıkarır (Baştan çıkarma - FIFO)
Node* dequeue(Queue* q) {
    if (is_empty(q)) return NULL;

    QueueNode* temp = q->front;
    Node* node_data = temp->data;

    q->front = q->front->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp); // Sadece kuyruk kapsülünü sil, asıl Node grafın içinde güvende
    return node_data;
}

// Kuyruğu tamamen temizler
void free_queue(Queue* q) {
    while (!is_empty(q)) {
        dequeue(q);
    }
    free(q);
}