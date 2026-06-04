

# Uludağ Propherty - Sosyal Ağ Analiz ve Görselleştirme Projesi

Bu proje, bir sosyal ağ ekosistemini (kullanıcılar, arkadaşlık ilişkileri, etkileşimler) modellemek, analiz etmek ve Raylib kullanarak görselleştirmek amacıyla **C programlama dili** ile geliştirilmiş kapsamlı bir uygulamadır. Proje aynı zamanda ağ analizi ve öngörüler sunmak için **Python tabanlı bir Yapay Zeka (AI) Mikroservisi** ile entegre çalışmaktadır.

---

## 📑 İçindekiler
1. [Proje Hakkında ve Temel İşleyiş](#proje-hakkında-ve-temel-işleyiş)
2. [Sistem Mimarisi ve Kullanılan Veri Yapıları](#sistem-mimarisi-ve-kullanılan-veri-yapıları)
3. [Arka Planda Neler Oluyor? (Etkileşim Senaryoları)](#arka-planda-neler-oluyor-etkileşim-senaryoları)
4. [Algoritmalar ve Analizler (Fonksiyon Detayları)](#algoritmalar-ve-analizler)
5. [Dizin Yapısı ve Dosyaların Görevleri](#dizin-yapısı-ve-dosyaların-görevleri)
6. [Kurulum ve Çalıştırma](#kurulum-ve-çalıştırma)

---

## 🚀 Proje Hakkında ve Temel İşleyiş

Proje, kurgusal veya gerçek verilerle beslenebilen bir sosyal ağ grafı (Graph) oluşturur. Kullanıcılar (Mavi), Fotoğraflar (Yeşil) ve Etkinlikler (Mor) birer **Düğüm (Node)** olarak; arkadaşlıklar, beğeniler ve katılımlar ise birer **Kenar (Edge)** olarak modellenmiştir. `main.c` programın giriş noktasıdır; grafik motorunu başlatır, boş veri yapılarını bellekte tahsis eder ve görsel arayüz döngüsünü (Game Loop) çalıştırır.

Kullanıcılar arayüz üzerinden ağa yeni veriler ekleyebilir, düğümleri sürükleyip bırakabilir, en kısa yol (shortest path) veya ortak arkadaş gibi karmaşık graf algoritmalarını tetikleyebilir.

---

## 🏗️ Sistem Mimarisi ve Kullanılan Veri Yapıları

Uygulamanın performanslı çalışabilmesi için standart C kütüphaneleri yerine bellek yönetimi optimize edilmiş özel veri yapıları inşa edilmiştir:

* **Graf (Graph - Adjacency List) (`graph_adj.c`):**
    Ağı bellekte tutan ana yapıdır. Düğümleri (Nodes) bir dizide tutarken, komşuluk ilişkilerini (Edges) her düğüme bağlı bir bağlı liste (Linked List) ile yönetir. Bu yapı bellek dostudur ve seyrek (sparse) graflar için idealdir. O(V+E) karmaşıklığı ile grafın taranmasını sağlar.
* **Hash Tablosu (Hash Table) (`hash_table.c`):**
    Düğüm ID'leri üzerinden `O(1)` karmaşıklığında ultra hızlı arama yapmak için kullanılır. Graf büyüdüğünde doğrusal arama (O(N)) yapmak yerine, kullanıcıya ID bazlı anında erişim sunar. Çakışmalar (collisions) bağlı liste (chaining) yöntemi ile çözülür.
* **Trie (Önek Ağacı) (`trie.c`):**
    Arama çubuğunda (Search Bar) kullanıcı adını yazarken otomatik tamamlama (auto-complete) için tasarlanmıştır. Veriler (isimler) harf harf ağaca dizilir. Arama işlemi ismin uzunluğu kadar, yani `O(L)` (L: kelime uzunluğu) sürer. Ekranda yazarken anında öneri çıkmasını sağlar.
* **Kuyruk (Queue) (`queue.c`):**
    Graf üzerindeki Genişlik Öncelikli Arama (BFS) ve "En Kısa Yol" (Shortest Path) hesaplamaları için First-In-First-Out (FIFO) prensibiyle çalışan yapıdır.

---

## ⚙️ Arka Planda Neler Oluyor? (Etkileşim Senaryoları)

Kullanıcı arayüzdeki butonlara bastığında veya etkileşime girdiğinde kod mimarisi şu şekilde çalışır:

### Senaryo 1: "Yapay Zeka: Kullanıcı/Fotoğraf/Etkinlik Ekle" Butonuna Basıldığında
1.  **UI Tetiklemesi:** Kullanıcı yan panelden (örneğin "Yapay Zeka: Kullanıcı Ekle") butonuna basar (`ui_integration.c` içerisindeki `GuiButton` tetiklenir).
2.  **Multithreading (Arka Plan İşlemi):** Arayüzün (UI) donmaması için bir iş parçacığı (thread) oluşturulur (`pthread_create(&t, NULL, fetch_ai_user_thread, NULL)`). Arayüz o sırada "Yapay Zeka Üretiyor..." şeklinde güncellenir.
3.  **Ağ İsteği (AI Client):** `ai_client.c` dosyası, libcurl kütüphanesini kullanarak arka plandaki Python (Flask/FastAPI) AI mikroservisine bir HTTP GET isteği gönderir (Örn: `http://ai_backend:5000/generate_users_bulk/5`).
4.  **Yanıtın Alınması:** Python AI, uydurma veya akıllı veriler oluşturup "|" karakteriyle ayırarak geri döner (Örn: "Ahmet|Mehmet|Ayşe"). C istemcisi bu yanıtı `ai_result_buffer` içine kopyalar ve `ai_new_data_ready` bayrağını (flag) kaldırır.
5.  **Verinin Sisteme İşlenmesi:** Ana C döngüsü (UI Thread) yeni verinin geldiğini fark eder. Gelen metni `strtok` fonksiyonu ile parçalar.
6.  **Graf Güncellemesi:** Her yeni veri için `create_node` çağrılır. Düğüm rastgele x,y koordinatlarına yerleştirilir. Yeni düğüm; Grafa (`graph_adj.c`), Hash Tablosuna (`hash_table.c`) ve Trie Ağacına (`trie.c`) senkron bir şekilde kaydedilir.
7.  **Bağlantıların Kurulması:** Yeni düğüm ile eski düğümler arasında rastgele (fakat düğüm tipine uygun) kenarlar oluşturulur. Örneğin bir kullanıcı eklendiğinde `FRIEND` kenarı, fotoğraf eklendiğinde `LIKES` kenarı kurulur.
8.  **Son Analiz (Faz 3):** Ekleme işlemi bittikten sonra `queries.c` içindeki `find_most_active_node(my_graph)` otomatik çalıştırılır ve ağın o anki en popüler düğümü terminale basılır.

### Senaryo 2: Arama Çubuğuna Bir İsim Yazıldığında
1.  Arama çubuğuna harfler girildikçe (`search_text_buffer`), kelime Trie ağacına gönderilir (`trie.c`).
2.  Ağaçta girilen harfler kadar derinliğe inilir. Eşleşen kelimeler anında alt alta bir "Dropdown" (açılır liste) olarak çizilir (`ui_integration.c`).
3.  Kullanıcı listeden bir isim seçtiğinde veya Enter'a bastığında, Trie doğrudan eşleşen düğümün (Node) bellek adresini (`Node*`) döndürür.
4.  Kamera seçilen düğüme odaklanır (`Boran_focus_on_node` - `ui_render.c`) ve düğümün çevresinde neon bir parlama efekti başlatılır. Seçilen düğümün tüm detayları, giriş ve çıkış bağlantıları yan panele yazdırılır (`Boran_format_side_panel_text`).

### Senaryo 3: Ekranda Görselleştirme (Rendering) ve Sürükleme
Ağda binlerce düğüm olduğunda uygulamanın kasmasını önlemek için `ui_render.c` içinde **Culling (Görüş Alanı Kontrolü)** uygulanır.
1.  Kamera koordinatları ve ekran sınırları (top_left, bottom_right) hesaplanır.
2.  Çizim döngüsü her düğümün koordinatını kontrol eder. Sınırların dışında kalan düğümler ve kenarlar hiç çizilmez, ağır matematiksel işlemler (okların açısını `atan2f` ile hesaplama) atlanır. Bu sayede 60 FPS garantilenir.
3.  Fare ile bir düğüme tıklandığında `CheckCollisionPointCircle` ile temas kontrolü yapılır. Basılı tutulduğunda fare deltası (konum değişimi) kadar düğümün X ve Y koordinatları güncellenir.

---

## 🧮 Algoritmalar ve Analizler (Fonksiyon Detayları)

Projede uygulanan algoritmalar (`algorithms.c` ve `queries.c`):

* **`bfs_and_find_degrees` (BFS):** Belirtilen bir başlangıç düğümünden dışa doğru katman katman arama yapar. Düğümler arası uzaklıkları (dereceleri) hesaplar.
* **`dfs` ve `dfs_full_network` (DFS):** Ağdaki düğümlerin derinlemesine taranmasını sağlar. Ağda birbirinden kopuk gruplar (bağlantı bileşenleri) olup olmadığını tespit eder.
* **`find_shortest_path`:** İki kullanıcı arasındaki en kısa yolu (ortak arkadaş zincirini) bulur. Ekmek kırıntısı (parent tracking) yöntemiyle yolu geriye doğru takip ederek terminale yazdırır (Örn: `Ahmet -> Mehmet -> Ayşe`).
* **`find_friends_events_photos`:** "Kullanıcının arkadaşlarının katıldığı etkinliklerde çekilmiş fotoğraflar" gibi çok katmanlı, zincirleme graf sorgularını gerçekleştirir.

---

Detaylı proje rapor pdfsi: [VERİ YAPILARI PROJE RAPORU.pdf](https://github.com/user-attachments/files/28591954/VERI.YAPILARI.PROJE.RAPORU.pdf)


## 📂 Dizin Yapısı ve Dosyaların Görevleri

```text
├── .gitignore               # Git takibine alınmayacak dosyalar
├── Makefile                 # C projesinin derleme yönergeleri
├── docker-compose.yml       # Frontend ve AI servisini aynı anda ayağa kaldıran yapılandırma
├── Dockerfile.frontend      # C uygulamasının Docker imaj dosyası
├── Roboto-Regular.ttf       # Arayüz (UI) için kullanılan yazı tipi
│
├── ai_service/              # Yapay Zeka Mikroservis Klasörü
│   ├── app.py               # Python AI API servisi (Flask/FastAPI)
│   ├── requirements.txt     # Python bağımlılıkları
│   └── Dockerfile           # Python servisinin Docker imajı
│
└── [C Kaynak Kodları]       # Çekirdek Sistem Dosyaları
    ├── main.c               # Programın başlatıcısı, bellek ayırmaları ve ana grafik döngüsü.
    ├── graph_adj.h/.c       # Adjacency List veri yapısı, node ve kenar ekleme mantıkları.
    ├── hash_table.h/.c      # Hash tablosu algoritmaları (ID bazlı O(1) erişim).
    ├── trie.h/.c            # Trie (Önek Ağacı) arama ve otomatik tamamlama algoritmaları.
    ├── queue.h/.c           # BFS ve En Kısa Yol için Queue (Kuyruk) implementasyonu.
    ├── algorithms.h/.c      # BFS, DFS, Arkadaş Önerisi ve En Kısa Yol hesaplamaları.
    ├── queries.h/.c         # Kompleks ağ sorguları ve Merkezilik (en aktif düğüm) analizleri.
    ├── ai_client.c          # C'den Python AI servisine Curl ile asenkron HTTP istekleri atan modül.
    ├── ui_integration.h/.c  # UI butonları, arayüz panelleri ve AI entegrasyon bağlayıcıları.
    ├── ui_render.h/.c       # Raylib ile ekrana çizim (culling, neon, oklar) ve kamera kontrolleri.
    └── raylib.h, raygui.h, raymath.h # Raylib harici UI kütüphaneleri

🛠️ Kurulum ve Çalıştırma
Projeyi ayağa kaldırmanın iki farklı yolu bulunmaktadır.

Yöntem 1: Docker Compose ile Çalıştırma (Önerilen)
Sisteminizde Docker ve Docker Compose kuruluysa:

Bash


docker-compose up --build
Bu komut hem Python AI servisini hem de C tabanlı Frontend'i izole ortamda ayağa kaldırır.

Yöntem 2: Manuel Derleme (Makefile ile Local Kurulum)
Sisteminizde gcc, make, raylib ve python3 yüklü olmalıdır.

AI Servisini Başlatın:

Bash


cd ai_service
pip install -r requirements.txt
python app.py
C Uygulamasını Derleyin ve Çalıştırın:
Yeni bir terminal açıp ana dizinde:

Bash


make clean
make
./program_adi
