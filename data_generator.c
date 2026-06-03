#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "graph_adj.h"
#include "hash_table.h"
#include "trie.h"
#include "data_generator.h"

static const char* USER_FIRST_NAMES[] = {
    "Ali", "Mehmet", "Ayse", "Fatma", "Emre", "Zeynep", "Can", "Elif",
    "Burak", "Selin", "Omer", "Deniz", "Hakan", "Merve", "Kerem", "Ece",
    "Serkan", "Gamze", "Tolga", "Pinar", "Cem", "Seda", "Baris", "Irem",
    "Kaan", "Yasemin", "Murat", "Derya", "Onur", "Asli", "Volkan", "Nazli",
    "Sinan", "Gizem", "Umut", "Buse", "Arda", "Cansu", "Koray", "Melis"
};
static const int USER_FIRST_COUNT = (int)(sizeof(USER_FIRST_NAMES) / sizeof(USER_FIRST_NAMES[0]));

static const char* USER_LAST_NAMES[] = {
    "Yilmaz", "Kaya", "Demir", "Sahin", "Celik", "Ozturk", "Arslan", "Dogan",
    "Kilic", "Aslan", "Aydin", "Polat", "Koc", "Kurt", "Ozdemir", "Yildiz",
    "Aksoy", "Erdem", "Tas", "Gunes", "Unal", "Tekin", "Bozkurt", "Cetin"
};
static const int USER_LAST_COUNT = (int)(sizeof(USER_LAST_NAMES) / sizeof(USER_LAST_NAMES[0]));

static const char* PHOTO_TITLES[] = {
    "Uludag Manzarasi", "Kamp Atesi", "Mezuniyet Anisi", "Konser Gecesi",
    "Gol Kenari", "Dogum Gunu", "Spor Salonu", "Kutuphane Calismasi",
    "Piknik Alani", "Sehir Turu"
};
static const int PHOTO_TITLE_COUNT = (int)(sizeof(PHOTO_TITLES) / sizeof(PHOTO_TITLES[0]));

static const char* EVENT_TITLES[] = {
    "Uludag Yaz Festivali", "Kampus Bahar Senligi", "Fotograf Sergisi",
    "Girisimcilik Zirvesi", "Muzik Gecesi", "Dogada Yoga",
    "Kariyer Gunu", "Kodlama Hackathonu", "Kitap Kulubu Bulusmasi",
    "Gastronomi Workshopu"
};
static const int EVENT_TITLE_COUNT = (int)(sizeof(EVENT_TITLES) / sizeof(EVENT_TITLES[0]));

static void build_user_name(int index, char* out, int out_len) {
    const char* first = USER_FIRST_NAMES[index % USER_FIRST_COUNT];
    const char* last = USER_LAST_NAMES[(index / USER_FIRST_COUNT) % USER_LAST_COUNT];
    snprintf(out, (size_t)out_len, "%s %s", first, last);
    if (index >= USER_FIRST_COUNT * USER_LAST_COUNT) {
        char suffix[16];
        snprintf(suffix, sizeof(suffix), " %d", (index / (USER_FIRST_COUNT * USER_LAST_COUNT)) + 1);
        strncat(out, suffix, (size_t)out_len - strlen(out) - 1);
    }
}

static void place_nodes_in_clusters(Graph* net, int user_count, int photo_start_id, int photo_count,
                                    int event_start_id, int event_count) {
    for (int i = 0; i < net->node_count; i++) {
        Node* n = net->nodes[i];
        if (n->type == USER) {
            float angle = (float)(n->id % user_count) / (float)user_count * 6.28318f;
            n->x = cosf(angle) * 320.0f;
            n->y = sinf(angle) * 220.0f;
        } else if (n->type == PHOTO) {
            int p = n->id - photo_start_id;
            n->x = -520.0f + (float)p * 55.0f;
            n->y = -120.0f + (float)(p % 3) * 90.0f;
        } else if (n->type == EVENT) {
            int e = n->id - event_start_id;
            n->x = 520.0f + (float)e * 55.0f;
            n->y = -120.0f + (float)(e % 3) * 90.0f;
        }
    }
    (void)photo_count;
    (void)event_count;
}

void init_synthetic_data(Graph* net, HashTable* ht, TrieNode* trie_root,
                         int user_count, int photo_count, int event_count) {

    printf("[*] Property graph veri uretimi basliyor (%d kullanici, %d fotograf, %d etkinlik)...\n",
           user_count, photo_count, event_count);

    srand((unsigned int)time(NULL));

    int current_id = 1;
    int photo_start_id = 0;
    int event_start_id = 0;

    for (int i = 0; i < user_count; i++) {
        Node* u = create_node(current_id, USER);
        char name[64];
        build_user_name(i, name, sizeof(name));
        add_property_to_node(u, "Name", TYPE_STRING, name);
        int age = 18 + (rand() % 40);
        add_property_to_node(u, "Age", TYPE_INTEGER, &age);

        add_node_to_graph(net, u);
        insert_to_hash(ht, u);
        Boran_insertToTrie(trie_root, name, u);
        current_id++;
    }

    photo_start_id = current_id;
    for (int i = 0; i < photo_count; i++) {
        Node* p = create_node(current_id, PHOTO);
        const char* title = PHOTO_TITLES[i % PHOTO_TITLE_COUNT];
        add_property_to_node(p, "Title", TYPE_STRING, (void*)title);
        char owner[32];
        int owner_id = (rand() % user_count) + 1;
        snprintf(owner, sizeof(owner), "user:%d", owner_id);
        add_property_to_node(p, "Owner", TYPE_STRING, owner);

        add_node_to_graph(net, p);
        insert_to_hash(ht, p);
        Boran_insertToTrie(trie_root, title, p);
        current_id++;
    }

    event_start_id = current_id;
    for (int i = 0; i < event_count; i++) {
        Node* e = create_node(current_id, EVENT);
        const char* title = EVENT_TITLES[i % EVENT_TITLE_COUNT];
        add_property_to_node(e, "Title", TYPE_STRING, (void*)title);
        const char* city = "Bursa";
        add_property_to_node(e, "City", TYPE_STRING, (void*)city);

        add_node_to_graph(net, e);
        insert_to_hash(ht, e);
        Boran_insertToTrie(trie_root, title, e);
        current_id++;
    }

    place_nodes_in_clusters(net, user_count, photo_start_id, photo_count, event_start_id, event_count);

    for (int i = 0; i < user_count * 4; i++) {
        int src = (rand() % user_count) + 1;
        int dest = (rand() % user_count) + 1;
        if (src != dest) {
            add_edge(net, src, dest, FRIEND, false);
        }
    }

    for (int p = 0; p < photo_count; p++) {
        int photo_id = photo_start_id + p;
        int likes_target = user_count / (photo_count > 0 ? photo_count : 1) + 8;
        if (likes_target < 5) likes_target = 5;
        for (int i = 0; i < likes_target; i++) {
            int user_id = (rand() % user_count) + 1;
            add_edge(net, user_id, photo_id, LIKES, true);
        }
    }

    for (int e = 0; e < event_count; e++) {
        int event_id = event_start_id + e;
        int attends_target = user_count / (event_count > 0 ? event_count : 1) + 10;
        if (attends_target < 8) attends_target = 8;
        for (int i = 0; i < attends_target; i++) {
            int user_id = (rand() % user_count) + 1;
            add_edge(net, user_id, event_id, ATTENDS, true);
        }
    }

    for (int e = 0; e < event_count; e++) {
        int event_id = event_start_id + e;
        if (photo_count > 0) {
            int photo_id = photo_start_id + (e % photo_count);
            add_edge(net, event_id, photo_id, HAS_PHOTO, true);
            if (photo_count > 1) {
                int photo_id2 = photo_start_id + ((e + 1) % photo_count);
                add_edge(net, event_id, photo_id2, HAS_PHOTO, true);
            }
        }
    }

    printf("[+] %d kullanici, %d fotograf, %d etkinlik ve iliskiler yuklendi.\n",
           user_count, photo_count, event_count);
    printf("[i] Ornek: Dugume tiklayarak ozellikleri ve baglantilari panelde gorebilirsiniz.\n");
    printf("[i] Arama: isim yazip Enter (ornek: ali, mehmet, Uludag)\n");
}
