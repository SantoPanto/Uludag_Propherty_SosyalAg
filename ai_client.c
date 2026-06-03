#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>

// BUFFER BÜYÜTÜLDÜ (8192 Karakter - Yüzlerce isim sığar)
char ai_result_buffer[8192] = "";
int ai_is_fetching = 0; // 0: Bitti, 1: Tekli Yükleniyor, 2: Toplu Yükleniyor
int ai_new_data_ready = 0; 
int ai_bulk_count = 1;  // Yeni: Toplu ekleme miktarını tutar

size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    size_t realsize = size * nmemb;
    strncat(data, (char *)ptr, realsize);
    return realsize;
}

void* fetch_ai_user_thread(void* arg) {
    (void)arg; 
    
    CURL *curl;
    CURLcode res;
    
    curl = curl_easy_init();
    if(curl) {
        char url[256];
        // Hangi modda istek atacağımıza karar veriyoruz
        if (ai_is_fetching == 2) {
            snprintf(url, sizeof(url), "http://ai_backend:5000/generate_users_bulk/%d", ai_bulk_count);
        } else {
            snprintf(url, sizeof(url), "http://ai_backend:5000/generate_user_simple");
        }
        
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        
        memset(ai_result_buffer, 0, sizeof(ai_result_buffer)); 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, ai_result_buffer);
        
        res = curl_easy_perform(curl);
        if(res == CURLE_OK) {
            ai_new_data_ready = ai_is_fetching; // 1 veya 2 dönecek
        } else {
            strcpy(ai_result_buffer, "Hata");
            ai_new_data_ready = ai_is_fetching;
        }
        curl_easy_cleanup(curl);
    }
    ai_is_fetching = 0; // İşlem bitti
    return NULL;
}