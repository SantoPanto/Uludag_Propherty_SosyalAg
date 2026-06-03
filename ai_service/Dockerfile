# Hafif bir Python imajı kullanıyoruz
FROM python:3.10-slim

# Konteyner içindeki çalışma dizinini belirliyoruz
WORKDIR /app

# Önce requirements.txt'yi kopyalayıp kütüphaneleri kuruyoruz (Önbellek avantajı için)
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Şimdi tüm kodlarımızı kopyalıyoruz
COPY . .

# Flask'ın çalışacağı portu dışa açıyoruz
EXPOSE 5000

# Konteyner çalıştığında ayağa kalkacak komut
CMD ["python", "app.py"]