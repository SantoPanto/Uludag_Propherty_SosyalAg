# Uludağ Propherty - Sosyal Ağ Analiz ve Görselleştirme Projesi

Bu proje, bir sosyal ağ ekosistemini (kullanıcılar, arkadaşlık ilişkileri, etkileşimler) modellemek, analiz etmek ve Raylib kullanarak görselleştirmek amacıyla **C programlama dili** ile geliştirilmiş kapsamlı bir uygulamadır. Proje aynı zamanda ağ analizi ve öngörüler sunmak için **Python tabanlı bir Yapay Zeka (AI) Mikroservisi** ile entegre çalışmaktadır.

---

## 📑 İçindekiler
1. [Proje Hakkında](#proje-hakkında)
2. [Sistem Mimarisi ve Yapılar](#sistem-mimarisi-ve-yapılar)
3. [Kullanılan Veri Yapıları](#kullanılan-veri-yapıları)
4. [Proje Raporu ve Analizler](#proje-raporu-ve-analizler)
5. [Kurulum ve Çalıştırma (Nasıl Ayağa Kaldırılır?)](#kurulum-ve-çalıştırma)
6. [Dizin Yapısı ve Dosyaların Görevleri](#dizin-yapısı)
7. [Kullanım](#kullanım)

---

## 🚀 Proje Hakkında
Proje, kurgusal veya gerçek verilerle beslenebilen bir sosyal ağ grafı oluşturur. Kullanıcılar arası en kısa yol (shortest path), etki alanı analizi, ortak arkadaş bulma gibi algoritmik problemleri çözer. Tüm bu süreç, Raylib motoru ile interaktif bir arayüzde (GUI) görselleştirilir. Ayrıca, karmaşık ağ sorgularını doğal dil ile çözümlemek için Python (Flask/FastAPI) ile yazılmış bir yapay zeka servisiyle (AI API) iletişim kurar.

## 🏗️ Sistem Mimarisi ve Yapılar

Proje mimarisi **3 ana bileşenden** oluşmaktadır:

1. **Çekirdek Sistem (Core / Backend - C):**
   - Tüm veri yönetimi, bellek işlemleri ve algoritmaların çalıştığı bölümdür. 
   - Mock veri üretimi (`data_generator.c`) ve ağ üzerindeki sorgular (`queries.c`, `algorithms.c`) burada işlenir.
2. **Görselleştirme ve Arayüz (Frontend - C & Raylib):**
   - Kullanıcı etkileşimleri ve graf (düğüm/kenar) çizimleri Raylib ve Raygui kütüphaneleri kullanılarak yapılır. (`ui_render.c`, `ui_integration.c`)
3. **AI Mikroservisi (Python):**
   - `ai_service/app.py` içerisinde çalışan, C uygulamasından gelen verileri (`ai_client.c` üzerinden) alıp AI API'lerine (Örn: OpenAI/Gemini) göndererek anlamlı çıktılar üreten bağımsız servistir.

---

## 🧮 Kullanılan Veri Yapıları

Uygulamanın performanslı çalışabilmesi için standart kütüphaneler yerine amaca özel veri yapıları inşa edilmiştir:

* **Graf (Graph - Adjacency List):** Sosyal ağdaki kullanıcıları (düğümler) ve aralarındaki arkadaşlıkları (kenarlar) tutar. (`graph_adj.c`, `graph_models.c`)
* **Hash Tablosu (Hash Table):** Kullanıcı ID'leri veya kullanıcı adları üzerinden `O(1)` karmaşıklığında ultra hızlı arama yapmak için kullanılır. (`hash_table.c`)
* **Trie (Önek Ağacı):** Arama çubuğunda (Search Bar) kullanıcı adını yazarken otomatik tamamlama (auto-complete) ve hızlı prefix eşleştirmesi yapmak için kullanılır. `O(L)` (L: kelime uzunluğu) karmaşıklığında çalışır. (`trie.c`)
* **Kuyruk (Queue):** Graf üzerindeki Breadth-First Search (BFS) algoritmaları ve "En Kısa Yol" (Shortest Path) hesaplamaları için kullanılır. (`queue.c`)

---

## 📊 Proje Raporu ve Analizler

> **Not:** Projenin UML diyagramlarını, veri yapılarının zaman karmaşıklığı (Big-O) analizlerini ve AI API'sine gönderilen prompt'ların dökümünü içeren kapsamlı rapor.

*(Eğer bu detayları içeren ekstra bir PDF/Docx dosyanız varsa, buraya linkini veya dosya yolunu ekleyebilirsiniz. Örn: `[Kapsamlı Raporu Görüntüle](./Rapor.pdf)`)*

---

## 🛠️ Kurulum ve Çalıştırma

Projeyi ayağa kaldırmanın **iki farklı yolu** bulunmaktadır. 

### Yöntem 1: Docker Compose ile Çalıştırma (Önerilen)
Sisteminizde `Docker` ve `Docker Compose` kuruluysa, tüm projeyi (C Frontend UI ve Python AI Servisi) tek komutla izole bir şekilde ayağa kaldırabilirsiniz.

1. Proje dizinine gidin.
2. Terminalde aşağıdaki komutu çalıştırın:
   ```bash
   docker-compose up --build
3. Bu komut;
   - `ai_service/Dockerfile` dosyasını okuyarak Python AI API'sini başlatır.
   - `Dockerfile.frontend` dosyasını okuyarak C uygulamasını derler ve çalıştırır. *(Not: GUI barındıran Docker container'larının ekranda görüntülenebilmesi için X11 Server ayarlarının yapılmış olması gerekebilir).*

### Yöntem 2: Manuel Derleme (Makefile ile Local Kurulum)
Eğer uygulamayı direkt kendi işletim sisteminizde çalıştırmak istiyorsanız:

**Gereksinimler:**
* `gcc` derleyicisi
* `make` aracı
* `raylib` kütüphanesi (Sisteminizde yüklü olmalıdır)
* `python3` ve `pip` (AI servisi için)

**Adımlar:**

1. **AI Servisini Başlatın:**
   ```bash
   cd ai_service
   pip install -r requirements.txt
   python app.py

2. **C Uygulamasını Derleyin ve Çalıştırın:**
   Yeni bir terminal sekmesi açıp ana dizine dönün:
   ```bash
   make clean
   make
   ./program_adi

   (Alternatif olarak Code::Blocks kullanıyorsanız SosyalAg.cbp proje dosyasını açıp derleyebilirsiniz).

## 📂 Dizin Yapısı ve Dosyaların Görevleri

```text
├── .gitignore               # Git takibine alınmayacak dosyalar
├── Makefile                 # C projesinin derleme yönergeleri
├── docker-compose.yml       # Frontend ve AI servisini aynı anda ayağa kaldıran yapılandırma
├── Dockerfile.frontend      # C uygulamasının Docker imaj dosyası
├── program_adi              # Derlenmiş çalıştırılabilir C dosyası
├── Roboto-Regular.ttf       # Arayüz (UI) için kullanılan yazı tipi
│
├── ai_service/              # Yapay Zeka Mikroservis Klasörü
│   ├── app.py               # Python AI API servisi (Flask/FastAPI)
│   ├── requirements.txt     # Python bağımlılıkları
│   └── Dockerfile           # Python servisinin Docker imajı
│
├── SosyalAg/                # IDE (Code::Blocks) Proje Dosyaları
│   ├── SosyalAg.cbp
│   └── SosyalAg.depend
│
└── [C Kaynak Kodları]       # Çekirdek Sistem Dosyaları
    ├── main.c               # Programın ana giriş noktası (Entry point)
    ├── data_generator.h/.c  # Test amaçlı rastgele sosyal ağ verisi üreten sistem
    ├── graph_models.h/.c    # Graf üzerindeki Düğüm (Node) ve Kenar (Edge) modelleri
    ├── graph_adj.h/.c       # Adjacency List (Komşuluk Listesi) implementasyonu
    ├── hash_table.h/.c      # Hash Tablosu veri yapısı ve arama işlemleri
    ├── hash_table_benchmark.c # Hash Tablosu performans testleri
    ├── trie.h/.c            # Trie (Önek Ağacı) veri yapısı
    ├── queue.h/.c           # Kuyruk (Queue) veri yapısı
    ├── algorithms.h/.c      # BFS, DFS vb. graf algoritmaları
    ├── queries.h/.c         # Ortak arkadaş, bağlantı vb. sosyal ağ sorguları
    ├── ai_client.c          # C'den Python AI servisine HTTP istekleri atan istemci
    ├── ui_integration.h/.c  # UI ile C mantıksal arka planını bağlayan entegrasyon
    ├── ui_render.h/.c       # Raylib ile ekrana çizim işlemlerini yapan motor
    └── raylib.h, raygui.h, raymath.h # Raylib harici UI kütüphaneleri


## 🎮 Kullanım

Uygulama başarıyla başlatıldığında karşınıza Raylib ile oluşturulmuş görsel bir ekran gelecektir.

* Ekranda sosyal ağdaki kişileri temsil eden düğümler (nodlar) ve aralarındaki arkadaşlık bağlarını görebilirsiniz.
* Sol/Sağ panellerdeki arayüz butonlarını (raygui) kullanarak kullanıcı arayabilir, ortak arkadaşları filtreleyebilir veya AI analiz butonlarına tıklayarak seçili graf verisi hakkında yapay zekadan yorum alabilirsiniz.