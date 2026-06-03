#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash_table.h"

static unsigned int next_random_u32(unsigned int* state) {
    *state = (*state * 1664525u) + 1013904223u;
    return *state;
}

static void populate_table(HashTable* ht, int count) {
    for (int i = 1; i <= count; i++) {
        Node* node = create_node(i, USER);
        insert_to_hash(ht, node);
    }
}

static double run_lookup_benchmark(HashTable* ht, int lookup_count, int max_key, int* found_count) {
    clock_t start = clock();
    int hits = 0;
    unsigned int rng_state = 12345u;
    unsigned int range = (unsigned int)(max_key * 2);

    for (int i = 0; i < lookup_count; i++) {
        int key = (int)(next_random_u32(&rng_state) % range) + 1;
        if (get_from_hash(ht, key) != NULL) {
            hits++;
        }
    }

    clock_t end = clock();
    if (found_count != NULL) {
        *found_count = hits;
    }

    return (double)(end - start) / CLOCKS_PER_SEC;
}

static void run_case(int node_count, int lookup_count) {
    HashTable* ht = create_hash_table(node_count * 2);
    if (ht == NULL) {
        printf("%d: hash table olusturulamadi\n", node_count);
        return;
    }

    populate_table(ht, node_count);

    int found_count = 0;
    double elapsed = run_lookup_benchmark(ht, lookup_count, node_count, &found_count);
    double avg_microseconds = (elapsed * 1000000.0) / lookup_count;
    double load_factor = (double)ht->count / (double)ht->size;

    printf("%d dugum | tablo=%d | load=%.2f | %d arama | %.6f sn | %.3f us/arama | hit=%d\n",
           node_count, ht->size, load_factor, lookup_count, elapsed, avg_microseconds, found_count);

    free_hash_table(ht);
}

int main(void) {
    srand(12345);

    printf("Hash table O(1) benchmark\n");
    printf("---------------------------------------------\n");

    run_case(1000, 200000);
    run_case(10000, 200000);
    run_case(100000, 200000);

    return 0;
}