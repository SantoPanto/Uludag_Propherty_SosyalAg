# Uludag_Propherty_SosyalA-
Uludağ Üniversitesi verileri yapıları dersi proje ödevi

## Görevli-2: Veri Analizi ve Sorgu Algoritmaları (Emine Taban)

Bu bölümde, sosyal ağ grafı üzerinde çok adımlı veri filtreleme ve merkezilik (popülerlik) analizleri gerçekleştirilmiştir. Algoritmaların karmaşıklık analizleri aşağıda verilmiştir:

### 1. Çok Adımlı Filtreleme Algoritması (find_friends_events_photos)
Belirli bir kullanıcının ağındaki "Arkadaş -> Etkinlik -> Fotoğraf" zincirini tarayan filtredir.
* **Zaman Karmaşıklığı (Time Complexity):** $O(V \cdot D^2 + D^3)$
  * **Açıklama:** Düğüm bulma işlemi (`find_node_index`) doğrusal arama yaptığı için $O(V)$ zaman alır. Ağacın derinliği 3 seviye olduğu için (Arkadaş, Etkinlik, Fotoğraf) ve her seviyede ortalama $D$ (Maksimum Derece/Bağlantı) kadar komşu tarandığı için iç içe döngüler bu karmaşıklığı üretir.
* **Uzay Karmaşıklığı (Space Complexity):** $O(1)$
  * **Açıklama:** Sadece mevcut graf üzerinde işaretçiler (pointer) ile gezinti yapılmış, ekstra bir bellek (yeni bir dizi, matris vb.) kullanılmamıştır. Bellek açısından kusursuzdur.

### 2. Merkezilik Analizi (find_most_active_node)
Ağdaki en fazla bağlantıya (dereceye) sahip, en aktif/popüler kullanıcıyı bulan algoritmadır.
* **Zaman Karmaşıklığı (Time Complexity):** $O(V + E)$
  * **Açıklama:** Tüm graf yapısı dolaşılarak $V$ (düğüm/kullanıcı sayısı) ve her bir düğümün bağlantıları $E$ (kenar sayısı) tek tek sayılmıştır. Veri yapısı olarak Bağlı Listeler (Adjacency List) kullanıldığı için teorik olarak ulaşılabilecek en optimum sürede çalışır.
* **Uzay Karmaşıklığı (Space Complexity):** $O(1)$
  * **Açıklama:** En yüksek dereceyi ve o kullanıcının ID'sini tutmak için sadece iki adet tam sayı değişkeni kullanılmış olup, ek bellek tahsisi yapılmamıştır.
