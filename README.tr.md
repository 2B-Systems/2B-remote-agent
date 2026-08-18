> **⚠️ Sorumluluk Reddi ve Güvenlik Uyarısı**
> **2B-remote-agent**, alt seviye sistem programlama pratiği yapmak amacıyla geliştirilmiş deneysel, eğitim ve araştırma odaklı bir projedir. Klavyeden veri yakalama işlevselliği (`WH_KEYBOARD_LL`) içerir.
> **Açık bir izin veya yetkiniz olmadan, sahip olmadığınız veya yönetmediğiniz sistemlere bu yazılımı yüklemeyin veya çalıştırmayın.** Şifreleme, kimlik doğrulama ve güvenlik sıkılaştırması içermediğinden üretim ortamları veya halka açık ağlar için güvenli değildir.

---

# 2B-remote-agent

WinAPI C istemcisi ve Python `asyncio` sunucusundan oluşan, eşzamansız bir uzaktan sistem yönetimi ve telemetri altyapısı.

## Genel Bakış

Bu altyapı, TCP bağlantıları üzerinden alt seviye sistem olaylarını yakalama ve telemetri verilerini toplama imkanı sunar.

```text
┌──────────────────────────┐          TCP          ┌──────────────────────────┐
│     Windows İstemcisi    │ ────────────────────> │      Python Sunucusu     │
│        (C / Win32)       │ Port 8080 (Varsayılan)│        (asyncio)         │
│ • Klavye olay kancası    │                       │ • Çoklu istemci yönetici │
│ • Düzen dönüştürme       │                       │ • Dosya tabanlı günlük   │
└──────────────────────────┘                       └──────────────────────────┘

```

---

## Sistem Bileşenleri

### Windows İstemcisi (`C / Win32`)

* **Ağ Bağlantısı:** Yapılandırılabilir IP/port hedeflerine yönelik Winsock soket yönetimi (varsayılan `127.0.0.1:8080`).
* **Girdi Yakalama:** Değiştirici tuş ve kısayol tespiti içeren alt seviye klavye olay kancası (`WH_KEYBOARD_LL`).
* **Kodlama:** Win32 API aracılığıyla Unicode-UTF-8 karakter işleme ve klavye düzeni çözümleme.
* **Üstveri:** Bağlantı başlangıcında yerel sistem kullanıcı adını iletir.

### Python Sunucusu (`asyncio`)

* **Eşzamanlılık:** `asyncio.start_server` kullanan eşzamansız TCP dinleyicisi.
* **Telemetri Alımı:** Gelen telemetri akışlarını çözer ve zaman damgalarını kaydeder.
* **Günlükleme (Logging):** `aiofiles` aracılığıyla yönetilen, istemciye özel yapılandırılmış dosya çıktısı.

### Telemetri Simülatörü

C istemci ortamına ihtiyaç duymadan sunucuyu test etmek için geliştirilmiş bağımsız bir Python istemcisidir (`run_simulator.py`).

* **Desteklenen Olaylar:** Kalp atışı (Heartbeat), CPU/Bellek/Disk/Ağ kullanımı, Süreç yaşam döngüsü, Servis durumu, Sistem sağlığı (sıcaklık, pil) ve hata günlükleri.

---

## Depo Yapısı

```text
2B-remote-agent/
├── client/
│   └── src/
│       ├── client.c
│       └── structure.h
├── server/
│   ├── src/
│   │   ├── tests/
│   │   │   └── client_simulator.py
│   │   ├── config_setup.py
│   │   ├── default_config.py
│   │   └── server.py
│   ├── requirements.txt
│   ├── run_server.py
│   └── run_simulator.py
└── README.md

```

---

## Başlarken

### Önkoşullar

* **İstemci:** Windows İşletim Sistemi, Win32 ve Winsock desteğine sahip C derleyicisi (`ws2_32.lib`).
* **Sunucu:** Python 3.8+ ve `aiofiles` kütüphanesi.

### 1. Sunucuyu Çalıştırma

```bash
cd server
pip install -r requirements.txt
python run_server.py

```

*Boş bırakılırsa varsayılan olarak `127.0.0.1:8080` kullanılır.*

### 2. Telemetri Simülatörünü Çalıştırma (İsteğe Bağlı)

```bash
cd server
python run_simulator.py

```

### 3. İstemciyi Derleme ve Çalıştırma

Win32 uyumlu bir derleyici (örneğin MinGW üzerinden `gcc` veya `ws2_32.lib` bağlayan MSVC `cl.exe`) kullanarak `client/src/client.c` dosyasını derleyin ve ardından çalıştırın:

```powershell
gcc client/src/client.c -o client.exe -lws2_32
.\client.exe

```

---

## Mevcut Güvenlik Sınırlamaları

Bu sürüm henüz erken aşamada bir araştırma prototipidir. Bilinen sınırlamalar şunlardır:

* **Şifreleme Yok:** Veri iletimi şifresiz düz metin (plaintext) olarak yapılır (TLS/SSL bulunmamaktadır).
* **Kimlik Doğrulama Yok:** İstemci bağlantıları için kimlik doğrulama veya jeton (token) değişimi yoktur.
* **Çerçevelenmemiş Protokoller:** Uygulama katmanında resmi bir mesaj çerçeveleme (framing) ve teslimat doğrulama mekanizması yoktur.

---

## Geliştirme Yol Haritası

| Aşama | Temel Hedef | Temel Çıktılar |
| --- | --- | --- |
| **Aşama 1** | Ağ Bağlantısı & Telemetri | TLS uygulaması, yapılandırılmış paket çerçeveleme, CI/CD süreçleri |
| **Aşama 2** | İşletim Sistemi İç Yapısı & Altyapı | WDK çekirdek sürücüleri, IOCTL entegrasyonu, Terraform test ortamları |
| **Aşama 3** | Korumalı Alan (Sandboxing) & Donanım | Gömülü UART desteği, Docker tabanlı korumalı alan, sistem çağrısı izleme |

---

## Geliştiriciler

* **Burak Yiğit Gerengi** — [burakyigitgerengi@gmail.com](https://www.google.com/search?q=mailto%3Aburakyigitgerengi%40gmail.com)
* **Berat Vargeloğlu** — [bv.sosyalmedya@gmail.com](https://www.google.com/search?q=mailto%3Abv.sosyalmedya%40gmail.com)

**GitHub Deposu:** [2B-Systems/2B-remote-agent](https://github.com/2B-Systems/2B-remote-agent)
