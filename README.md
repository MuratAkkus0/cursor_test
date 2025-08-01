# CryptoBreaker

**Frekans Analizi ile Klasik Şifre Kırma Algoritması**

## Proje Özeti

CryptoBreaker, klasik monoalfabetik ve polialfabetik şifrelemelerini (Caesar Cipher, Substitution Cipher, Vigenère Cipher) frekans analizi yöntemleriyle kırmaya yönelik bir C++ uygulamasıdır.

**Versiyon:** 1.0  
**Platform:** Cross-platform (Windows, Linux, macOS)  
**Programlama Dili:** C++17

## Desteklenen Şifreleme Türleri

- ✅ **Caesar Cipher** - Frekans analizi ile kırma
- ✅ **Monoalfabetik Substitution Cipher** - Bigram/trigram analizi ile kırma
- ✅ **Vigenère Cipher** - Kasiski yöntemi + frekans analizi ile kırma

## Özellikler

- 🔍 Otomatik frekans analizi
- 🌍 İngilizce ve Türkçe dil desteği
- 📊 İstatistiksel analiz ve raporlama
- ⚡ Çoklu dosya işleme
- 🖥️ Kullanıcı dostu CLI arayüzü

## Sistem Gereksinimleri

### Minimum Sistem
- **RAM:** 512 MB
- **Disk Alanı:** 100 MB
- **İşlemci:** 1 GHz (tek çekirdek)

### Önerilen Sistem
- **RAM:** 2 GB+
- **Disk Alanı:** 500 MB
- **İşlemci:** 2 GHz+ (çok çekirdekli)

## Kurulum

### Gereksinimler
- C++ derleyici (GCC 7+ veya Clang 6+)
- CMake 3.10+
- Git

### Derleme Adımları

```bash
# Repository'yi klonla
git clone <repository-url>
cd CryptoBreaker

# Build dizinini oluştur
mkdir build
cd build

# CMake ile projeyi yapılandır
cmake ..

# Projeyi derle
make

# Testleri çalıştır
make test
```

## Kullanım

### Ana Menü
```bash
./cryptobreaker
```

### Komut Satırı Kullanımı
```bash
# Caesar cipher kırma
./cryptobreaker -type caesar -input "KHOOR ZRUOG" -output result.txt

# Otomatik cipher tespit
./cryptobreaker -type auto -file encrypted.txt -verbose

# Vigenère cipher kırma
./cryptobreaker -type vigenere -input "ATTACKATDAWN" -keylength 6
```

## Proje Yapısı

```
CryptoBreaker/
├── src/                    # Ana kaynak kodlar
│   ├── main.cpp
│   ├── FrequencyAnalyzer.cpp
│   ├── CaesarBreaker.cpp
│   ├── SubstitutionBreaker.cpp
│   ├── VigenereBreaker.cpp
│   ├── LanguageDetector.cpp
│   └── Utils.cpp
├── include/               # Header dosyaları
│   ├── FrequencyAnalyzer.h
│   ├── CipherBreaker.h
│   ├── LanguageDetector.h
│   └── Utils.h
├── data/                  # Frekans tabloları
│   ├── english_freq.txt
│   ├── turkish_freq.txt
│   └── common_words.txt
├── tests/                 # Test dosyaları
│   ├── test_samples/
│   └── unit_tests.cpp
└── docs/                  # Dokümantasyon
    └── algorithm_explanation.md
```

## Performans

- **Caesar Cipher:** < 100ms (1KB metin)
- **Substitution Cipher:** < 5 saniye (1KB metin)
- **Vigenère Cipher:** < 10 saniye (1KB metin)
- **Frekans Analizi:** < 50ms (herhangi boyut)

## Doğruluk Oranları

- **Caesar Cipher:** %98+ (İngilizce standart metinlerde)
- **Substitution Cipher:** %85+ (yeterli metin uzunluğunda)
- **Vigenère Cipher:** %75+ (anahtar uzunluğu ≤ 12)

## Katkıda Bulunma

1. Fork edin
2. Feature branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Commit edin (`git commit -m 'Add amazing feature'`)
4. Push edin (`git push origin feature/amazing-feature`)
5. Pull Request açın

## Lisans

Bu proje eğitim amaçlı geliştirilmiştir.

## İletişim

**Geliştirici:** Siber Güvenlik Ekibi  
**E-posta:** [email]  
**Versiyon:** 1.0  

---

⚠️ **Uyarı:** Bu araç yalnızca eğitim amaçlıdır. Modern şifreleme algoritmalarını kırmak için kullanılamaz.