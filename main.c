#include <stdio.h>
#include <stdlib.h>
#include "graph_adj.h"

int main() {
    printf("--- Sosyal Ag Graf Modeli Testi ---\n\n");

    // 1. Grafı oluştur
    Graph* net = create_graph(5);

    // 2. Düğümler oluştur
    Node* user1 = create_node(1, USER);
    Node* photo1 = create_node(101, PHOTO);

    // 3. Düğümleri grafa ekle
    add_node_to_graph(net, user1);
    add_node_to_graph(net, photo1);

    // 4. Kenar ekle (User Photo'yu beğendi)
    add_edge(net, 1, 101, LIKES, true);

    // 5. Kenara özellik ekle (Beğeni tarihi)
    char* date = "2026-04-27";
    // Not: add_edge doğrudan Edge dönmediği için head üzerinden erişiyoruz
    if(net->adjLists[0] != NULL) {
        add_property_to_edge(&(net->adjLists[0]->edge), "LikeDate", TYPE_STRING, date);
    }

    printf("Dugumler ve Kenarlar basariyla baglandi!\n");
    printf("User(ID:%d) -> Photo(ID:%d) iliskisi kuruldu.\n\n", user1->id, photo1->id);

    // 6. Belleği temizle
    free_graph(net);
    printf("Bellek temizlendi, program sorunsuz kapandi.\n");

    return 0;
}
