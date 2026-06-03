<<<<<<< HEAD
# --- Derleyici ve Derleme Bayrakları ---
CC = gcc
# -Wall ve -Wextra tüm uyarıları açar, -g ise Valgrind için hata ayıklama (debug) bilgisini ekler
CFLAGS = -Wall -Wextra -g

# --- Projedeki Tüm Kaynak Dosyalar ---
# Ekibindeki herkesin yazdığı .c dosyaları burada listelenir
SRCS = main.c graph_models.c graph_adj.c hash_table.c trie.c queue.c

# --- Çıktı (Çalıştırılabilir) Dosya Adı ---
TARGET = program_adi

# --- KURUALLAR (RULES) ---

# Sadece 'make' yazıldığında çalışacak varsayılan kural
all: $(TARGET)

# Kodları birleştirip derleme kuralı
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)
	@echo "\n[+] Derleme basarili! Calistirmak icin 'make run' yazabilirsin."

# Programı hızlıca çalıştırmak için
run: $(TARGET)
	@echo "\n--- PROGRAM BASLATILIYOR ---\n"
	./$(TARGET)

# Valgrind ile bellek sızıntısı testi yapmak için
valgrind: $(TARGET)
	@echo "\n--- VALGRIND BELLEK TESTI BASLATILIYOR ---\n"
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Oluşan derlenmiş dosyaları silip klasörü temizlemek için
clean:
	rm -f $(TARGET)
=======
# --- Derleyici ve Derleme Bayrakları ---
CC = gcc
# -Wno-unused-parameter eklenerek raygui.h içindeki gereksiz uyarılar susturuldu
CFLAGS = -Wall -Wextra -g -Wno-unused-parameter

# --- Linux için Raylib Kütüphaneleri ---
LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -lcurl

# --- Projedeki Tüm Kaynak Dosyalar ---
SRCS = main.c graph_models.c graph_adj.c hash_table.c trie.c queue.c algorithms.c data_generator.c ui_render.c ui_integration.c queries.c ai_client.c

# --- Çıktı (Çalıştırılabilir) Dosya Adı ---
# Linux ortamı için .exe uzantısı kaldırıldı
TARGET = program_adi

# --- KURALLAR (RULES) ---

# Sadece 'make' yazıldığında çalışacak varsayılan kural
all: $(TARGET)

# Kodları birleştirip derleme kuralı
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)
	@echo "\n[+] Derleme basarili! Calistirmak icin 'make run' yazabilirsin."

# Programı hızlıca çalıştırmak için
run: $(TARGET)
	@echo "\n--- PROGRAM BASLATILIYOR ---\n"
	./$(TARGET)

# Oluşan derlenmiş dosyaları silip klasörü temizlemek için (Linux rm komutu)
clean:
	rm -f $(TARGET)
>>>>>>> main
	@echo "\n[+] Temizlik basarili! Derlenmis '$(TARGET)' dosyasi silindi."