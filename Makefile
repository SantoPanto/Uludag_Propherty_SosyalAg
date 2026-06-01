# --- Derleyici ve Derleme Bayrakları ---
CC = gcc
CFLAGS = -Wall -Wextra -g

# --- Windows için Raylib Kütüphaneleri (YENİ EKLENDİ) ---
LDFLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

# --- Projedeki Tüm Kaynak Dosyalar (YENİ DOSYALAR EKLENDİ) ---
SRCS = main.c graph_models.c graph_adj.c hash_table.c trie.c queue.c algorithms.c data_generator.c ui_render.c ui_integration.c

# --- Çıktı (Çalıştırılabilir) Dosya Adı ---
# Windows ortamı için .exe uzantısı eklendi
TARGET = program_adi.exe

# --- KURALLAR (RULES) ---

# Sadece 'make' yazıldığında çalışacak varsayılan kural
all: $(TARGET)

# Kodları birleştirip derleme kuralı (LDFLAGS sona eklendi)
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "\n[+] Derleme basarili! Calistirmak icin 'make run' yazabilirsin."

# Programı hızlıca çalıştırmak için (Windows formatına çevrildi)
run: $(TARGET)
	@echo "\n--- PROGRAM BASLATILIYOR ---\n"
	$(TARGET)

# Oluşan derlenmiş dosyaları silip klasörü temizlemek için (Windows CMD uyumlu del komutu eklendi)
clean:
	del /f /q $(TARGET)
	@echo "\n[+] Temizlik basarili! Derlenmis '$(TARGET)' dosyasi silindi."