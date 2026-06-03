from flask import Flask, request, jsonify
from faker import Faker

app = Flask(__name__)
fake = Faker('tr_TR') 

# 1. Mevcut analiz rotan
@app.route('/analyze_graph', methods=['POST'])
def analyze_graph():
    data = request.json
    user_query = data.get('query', '')
    ai_response = f"Yapay Zeka Analizi: '{user_query}' sorgusu incelendi. Verilen graf bilgisine gore bu kullanici oldukca populer!"
    return jsonify({"success": True, "ai_insight": ai_response})

# 2. Yeni kullanıcı üretim rotan
@app.route('/generate_user_simple', methods=['GET'])
def generate_user_simple():
    # Benzersiz bir isim üretiyoruz
    secilen_isim = fake.name()
    
    # C kodundaki ui_integration.c "strtok" kullandığı için SAF METİN döndürüyoruz
    return f"{secilen_isim}", 200

# 3. Server başlatma komutu
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)