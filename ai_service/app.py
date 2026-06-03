from flask import Flask, request, jsonify
# import google.generativeai as genai # Gemini kullanacaksanız
import os

app = Flask(__name__)

# NOT: Gerçek projede API key'i koda gömmeyin, .env dosyasından çekin.
# genai.configure(api_key=os.environ.get("API_KEY"))

@app.route('/health', methods=['GET'])
def health_check():
    return jsonify({"status": "Servis calisiyor", "version": "1.0"})

@app.route('/analyze_graph', methods=['POST'])
def analyze_graph():
    data = request.json
    
    if not data or 'query' not in data:
        return jsonify({"error": "Lutfen 'query' parametresi gonderin."}), 400

    user_query = data['query']
    graph_context = data.get('context', 'Graf bilgisi verilmedi.')

    # --- YAPAY ZEKA KODU BURAYA GELECEK ---
    # Örnek prompt: "Sen bir sosyal ağ analistisin. Şu graf verisine bak: {graph_context}. Soru: {user_query}"
    # response = model.generate_content(prompt)
    
    # Şimdilik API'ye bağlanmadan sahte (mock) bir yanıt döndürüyoruz test için:
    ai_response = f"Yapay Zeka Analizi: '{user_query}' sorgusu incelendi. Verilen graf bilgisine gore bu kullanici oldukca populer!"

    return jsonify({
        "success": True,
        "ai_insight": ai_response
    })

if __name__ == '__main__':
    # Sunucuyu tüm IP'lere (0.0.0.0) açıyoruz ki Docker dışından C programı erişebilsin
    app.run(host='0.0.0.0', port=5000)