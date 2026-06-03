from flask import Flask, request, jsonify
import random

app = Flask(__name__)

# 1. Mevcut analiz rotan
@app.route('/analyze_graph', methods=['POST'])
def analyze_graph():
    data = request.json
    user_query = data.get('query', '')
    ai_response = f"Yapay Zeka Analizi: '{user_query}' sorgusu incelendi. Verilen graf bilgisine gore bu kullanici oldukca populer!"
    return jsonify({"success": True, "ai_insight": ai_response})

# 2. Yeni kullanıcı üretim rotan (Artık en doğru yerde!)
@app.route('/generate_user_simple', methods=['GET'])
def generate_user_simple():
    # İsim havuzunu genişlettik
    isimler = ["Ayşe Yılmaz", "Boran Kaya", "Fatih Şahin", "Emine Taban", "Süha Demir", 
               "Ahmet Çelik", "Zeynep Demir", "Caner Arı", "Deniz Yıldız", "Merve Kılıç"]
    
    secilen_isim = random.choice(isimler)
    
    # Meslek ve Yaş istenmediği için sadece ismi döndürüyoruz
    # Format: İsim
    return f"{secilen_isim}", 200

# 3. Server başlatma komutu EN SONDA olmalı
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)