from flask import Flask, request, jsonify
from faker import Faker
import random # Yeni ekledik (Etkinlik isimlerini çeşitlendirmek için)

app = Flask(__name__)
fake = Faker('tr_TR') 

@app.route('/analyze_graph', methods=['POST'])
def analyze_graph():
    data = request.json
    user_query = data.get('query', '')
    return jsonify({"success": True, "ai_insight": "Sorgu incelendi."})

@app.route('/generate_user_simple', methods=['GET'])
def generate_user_simple():
    return f"{fake.name()}", 200

# 1. KULLANICI ROTASI
@app.route('/generate_users_bulk/<int:count>', methods=['GET'])
def generate_users_bulk(count):
    count = min(count, 100)
    isimler = [fake.name() for _ in range(count)]
    return "|".join(isimler), 200

# 2. FOTOĞRAF ROTASI (GELİŞMİŞ KOMBİNASYONLAR)
@app.route('/generate_photos_bulk/<int:count>', methods=['GET'])
def generate_photos_bulk(count):
    count = min(count, 100)
    fotograflar = []
    
    for _ in range(count):
        # Farklı şablonlar (pattern) oluşturuyoruz. random.choice ile her seferinde rastgele bir şablon seçilecek.
        sablonlar = [
            f"{fake.city()} {fake.street_name()} Gezisi",                  # Örn: Bursa Atatürk Caddesi Gezisi
            f"{fake.first_name()} ile {fake.city()} Hatırası",             # Örn: Ayşe ile İzmir Hatırası
            f"{fake.company()} Ofis Çekimi",                               # Örn: Yılmazlar A.Ş. Ofis Çekimi
            f"{fake.month_name()} Ayında {fake.country()} Tatili",         # Örn: Haziran Ayında İtalya Tatili
            f"{fake.color_name().capitalize()} Konseptli Stüdyo Çekimi",   # Örn: Kırmızı Konseptli Stüdyo Çekimi
            f"{fake.last_name()} Ailesi Doğa Yürüyüşü"                     # Örn: Kaya Ailesi Doğa Yürüyüşü
        ]
        fotograflar.append(random.choice(sablonlar))
        
    return "|".join(fotograflar), 200

# 3. ETKİNLİK ROTASI (GELİŞMİŞ KOMBİNASYONLAR)
@app.route('/generate_events_bulk/<int:count>', methods=['GET'])
def generate_events_bulk(count):
    count = min(count, 100)
    etkinlikler = []
    
    for _ in range(count):
        sablonlar = [
            f"{fake.job()} Zirvesi {fake.year()}",                         # Örn: Bilgisayar Mühendisi Zirvesi 2024
            f"{fake.company()} Yıl Sonu Konferansı",                       # Örn: Demir Lojistik Yıl Sonu Konferansı
            f"Geleneksel {fake.city()} {fake.street_name()} Şenliği",      # Örn: Antalya Cumhuriyet Caddesi Şenliği
            f"{fake.first_name()} {fake.last_name()} Açık Hava Konseri",   # Örn: Tarkan Tevetoğlu Açık Hava Konseri
            f"Uluslararası {fake.job()} Sempozyumu",                       # Örn: Uluslararası Avukat Sempozyumu
            f"{fake.country()} Kültür ve Sanat Festivali"                  # Örn: Japonya Kültür ve Sanat Festivali
        ]
        etkinlikler.append(random.choice(sablonlar))
        
    return "|".join(etkinlikler), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)