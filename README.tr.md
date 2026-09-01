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
* **Yapılandırma:** `127.0.0.1:8080` varsayılanlarıyla etkileşimli IPv4/IPv6 ve port doğrulaması.
* **Telemetri Alımı:** Yeni satırla ayrılmış UTF-8 kayıtlarını okur ve ISO 8601 zaman damgaları ekler.
* **Günlükleme (Logging):** Kayıtları `aiofiles` ile `server/logs/` altındaki bağlantıya özel dosyalara eşzamansız olarak ekler.
* **Yaşam Döngüsü:** Bir istemci bağlantısında birden fazla kayıt alır; istemciyi ve dinleyiciyi düzenli biçimde kapatır.

### Telemetri Simülatörü

C istemci ortamına ihtiyaç duymadan sunucuyu test etmek için geliştirilmiş bağımsız bir Python istemcisidir (`run_simulator.py`).

* **Desteklenen Olaylar:** Kalp atışı (Heartbeat), CPU/Bellek/Disk/Ağ kullanımı, Süreç yaşam döngüsü, Servis durumu, Sistem sağlığı (sıcaklık, pil) ve hata günlükleri.
* **Bağlantı Davranışı:** Önce simülatör kullanıcı adını, ardından yeni satırla ayrılmış telemetri kayıtlarını tek ve kalıcı bir bağlantı üzerinden gönderir.
* **Mesaj Gecikmesi:** Mesajları aralıksız gönderebilir veya mesajlar arasında rastgele bir süre bekleyebilir.

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
│   │   ├── server.py
│   │   ├── server_client_connection.py
│   │   └── server_config.py
│   ├── logs/                 # Üretilen bağlantı günlükleri (Git tarafından yok sayılır)
│   ├── requirements.txt
│   ├── run_server.py
│   └── run_simulator.py
├── README.md
└── README.tr.md

```

---

## Başlarken

### Önkoşullar

* **İstemci:** Windows İşletim Sistemi, Win32 ve Winsock desteğine sahip C derleyicisi (`ws2_32.lib`).
* **Sunucu:** Python 3.10+ ve `aiofiles` kütüphanesi.

### 1. Sunucuyu Çalıştırma

```bash
cd server
pip install -r requirements.txt
python run_server.py

```

*Boş bırakılırsa varsayılan olarak `127.0.0.1:8080` kullanılır.*

Alınan kayıtlar `server/logs/` dizinine yazılır. Günlük dosyası adlarında istemcinin güvenli hâle getirilmiş IP adresi ve kaynak portu kullanılır.

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
* **Asgari Çerçeveleme:** Kayıtlar yeni satırlarla ayrılır; ancak protokolde uzunluk öneki, şema doğrulaması, bütünlük kontrolü veya teslimat doğrulaması yoktur.

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
