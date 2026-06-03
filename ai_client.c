#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>

// Global değişkenler (Thread ve Ana döngü arasında haberleşmek için)
char ai_result_buffer[256] = "";
int ai_is_fetching = 0; // 1 ise yükleniyor, 0 ise bitti
int ai_new_data_ready = 0; // Yeni veri geldiyse 1 olur

// CURL'den gelen veriyi belleğe yazma fonksiyonu
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    size_t realsize = size * nmemb;
    strncat(data, (char *)ptr, realsize);
    return realsize;
}

// Arka planda çalışacak Thread (İş parçacığı) fonksiyonu
void* fetch_ai_user_thread(void* arg) {
    (void)arg; // Kullanılmayan parametre uyarısını sustur
    
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        // Python servisimizin adresi (Docker içi ağda localhost üzerinden)
        curl_easy_setopt(curl, CURLOPT_URL, "http://ai_backend:5000/generate_user_simple");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        
        // Gelen veriyi ai_result_buffer içine yazacağız
        memset(ai_result_buffer, 0, sizeof(ai_result_buffer)); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ai_result_buffer);
        
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            ai_new_data_ready = 1; // Başarılı! Ana döngüye haber ver
        } else {
            strcpy(ai_result_buffer, "Hata|0|Baglanti Koptu");
            ai_new_data_ready = 1;
        }
        curl_easy_cleanup(curl);
    }
    ai_is_fetching = 0; // İşlem bitti
    return NULL;
}