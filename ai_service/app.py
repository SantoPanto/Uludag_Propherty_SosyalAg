from flask import Flask, request, jsonify
from faker import Faker

app = Flask(__name__)
fake = Faker('tr_TR') 

@app.route('/analyze_graph', methods=['POST'])
def analyze_graph():
    data = request.json
    user_query = data.get('query', '')
    ai_response = f"Yapay Zeka Analizi: '{user_query}' sorgusu incelendi. Verilen graf bilgisine gore bu kullanici oldukca populer!"
    return jsonify({"success": True, "ai_insight": ai_response})

# Eski tekli rotan (Hala durabilir)
@app.route('/generate_user_simple', methods=['GET'])
def generate_user_simple():
    secilen_isim = fake.name()
    return f"{secilen_isim}", 200

# YENI: Toplu kullanıcı üretim rotası
@app.route('/generate_users_bulk/<int:count>', methods=['GET'])
def generate_users_bulk(count):
    # Güvenlik için en fazla 100 isim üretilmesine izin veriyoruz
    count = min(count, 100)
    isimler = [fake.name() for _ in range(count)]
    
    # İsimleri aralarına | koyarak tek bir string yapıyoruz
    return "|".join(isimler), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)