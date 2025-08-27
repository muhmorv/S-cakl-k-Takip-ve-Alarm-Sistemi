# S-cakl-k-Takip-ve-Alarm-Sistemi
Bu proje, bir dijital sensörle periyodik olarak sıcaklık ölçen , bu veriyi kullanıcı tarafından UART üzerinden ayarlanabilen eşiklerle karşılaştırıp görsel bir alarmı tetikleyen ve tüm ölçümleri zaman damgasıyla kalıcı hafızaya (EEPROM) kaydeden bir sıcaklık takip ve alarm sistemidir.
# Sıcaklık Takip, Alarm ve Kayıt Sistemi

Bu proje, bir STM32F072RB mikrokontrolcüsü kullanılarak geliştirilmiş gömülü bir sıcaklık izleme sistemidir.

## 1. Projeye Genel Bakış

[cite_start]Bu proje, bir dijital sensörle periyodik olarak sıcaklık ölçen, bu veriyi kullanıcı tarafından UART üzerinden ayarlanabilen eşiklerle karşılaştırarak görsel bir alarmı tetikleyen ve tüm ölçümleri zaman damgasıyla kalıcı hafızaya (EEPROM) kaydeden bir sıcaklık izleme sistemidir [cite: 4-7, 72].

[cite_start]Sistem, modüler bir yazılım mimarisiyle tasarlanmış olup, donanım sürücüleri, iletişim protokolleri ve ana uygulama mantığı birbirinden net bir şekilde ayrılmıştır[cite: 24].

## 2. Özellikler

* [cite_start]**Periyodik Sıcaklık Okuma:** Sistem, her 1 saniyede bir sıcaklık verisini okur ve durumunu raporlar[cite: 10].
* **Yapılandırılabilir Alarmlar:** Kullanıcı tarafından UART üzerinden ayarlanabilen alt ve üst sıcaklık eşikleri bulunmaktadır. [cite_start]Bu eşikler aşıldığında ilgili LED'ler yanarak görsel alarm üretilir [cite: 12-13, 15-16, 71].
* [cite_start]**Zaman Damgalı Veri Kaydı:** Her ölçüm, pil destekli RTC'den (Gerçek Zamanlı Saat) alınan o anki zaman bilgisi ile birlikte harici EEPROM'a kaydedilir[cite: 5, 11, 72].
* **Pil Destekli Gerçek Zamanlı Saat:** `VBAT` pinine bağlanan harici bir pil sayesinde, sistemin ana gücü kesildiğinde bile RTC zamanı saymaya devam eder.
* [cite_start]**Etkileşimli UART Arayüzü:** Kullanıcı, bir seri port terminali üzerinden sisteme komutlar gönderebilir, ayarları değiştirebilir ve kayıtlı verileri talep edebilir[cite: 7, 73].
* **Akıllı Veri Akışı:** Kullanıcı terminale bir komut yazmaya başladığı anda, sistemin otomatik olarak gönderdiği anlık durum mesajları duraklatılır ve komut işlendikten sonra tekrar başlatılır.
* **Hata Yönetimi:** Sensör veya EEPROM gibi harici donanımların bağlı olmaması durumunda sistem çökmeyecek şekilde tasarlanmıştır.

## 3. Donanım Gereksinimleri

* STM32 NUCLEO-F072RB Geliştirme Kartı
* DS18B20 Dijital Sıcaklık Sensörü
* AT24C256 I2C EEPROM Modülü
* 3V Saat Pili (örn: CR2032) ve Pil Yuvası
* 32.768 kHz Harici Kristal (LSE için)
* 2 adet LED (Yüksek ve Düşük Alarmlar için)
* 2 adet 220Ω (veya 330Ω) Direnç (LED'ler için)
* 1 adet 4.7kΩ Direnç (DS18B20 için Pull-up)
* Breadboard ve Jumper Kablolar

## 4. Yazılım Kurulumu ve Derleme

1.  **Gerekli Yazılımlar:**
    * STM32CubeIDE (v1.10.0 veya üstü)
    * Tera Term, PuTTY gibi bir seri port terminal programı
2.  **Kurulum:**
    * Bu projeyi bilgisayarınıza klonlayın veya indirin.
    * STM32CubeIDE'yi açın ve `File > Open Projects from File System...` seçeneği ile proje klasörünü seçin.
    * Proje başarıyla yüklendikten sonra, üst menüden `Project > Build All` (veya çekiç ikonu) ile projeyi derleyin.
    * NUCLEO kartını USB ile bilgisayara bağlayın ve `Run > Run` (veya yeşil oynat butonu) ile kodu karta yükleyin.

## 5. Kullanım

1.  **Donanım Bağlantısı:** Yukarıdaki donanım listesindeki tüm bileşenleri şemaya uygun olarak NUCLEO kartına bağlayın.
2.  **Seri Terminal Bağlantısı:**
    * NUCLEO kartını USB ile bilgisayara bağlayın.
    * Tera Term'i açın ve doğru COM portunu seçin.
    * Bağlantı ayarlarını şu şekilde yapın:
        * **Baud rate:** 115200
        * **Data:** 8 bit
        * **Parity:** none
        * **Stop:** 1 bit
        * **Flow control:** none
3.  **Komut Listesi:** Sistem çalışmaya başladığında, anlık sıcaklık verisi ekrana akmaya başlayacaktır. Komut yazmak için herhangi bir tuşa bastığınızda bu akış durur.

| Komut | Parametre | Açıklama |
| :--- | :--- | :--- |
| `SET HIGH` | `<sıcaklık>` | Yüksek alarm eşiğini ayarlar. Örnek: `SET HIGH 30.5` |
| `SET LOW` | `<sıcaklık>` | Düşük alarm eşiğini ayarlar. Örnek: `SET LOW 15.0` |
| `GET LOGS`| Yok | EEPROM'daki tüm kayıtları listeler. |
| `SET TIME`| `HH:MM:SS` | RTC saatini ayarlar. Örnek: `SET TIME 14:30:00` |
| `SET DATE`| `GG:AA:YY` | RTC tarihini ayarlar. Örnek: `SET DATE 27:08:25` |

## 6. Kod Yapısı

Proje, sorumlulukların ayrılması prensibine göre modüler bir yapıda tasarlanmıştır:

* **`main.c`**: Ana başlangıç dosyası. Donanımı ve modülleri başlatır, ana döngüyü çalıştırır.
* **`system_logic.c / .h`**: Projenin beyni. Ana iş akışını, periyodik görevleri, alarm mantığını ve komut yorumlamayı yönetir.
* **`uart_handler.c / .h`**: Kesme tabanlı UART iletişimini ve ham veri alım/gönderimini yönetir.
* **`sensor_manager.c / .h`**: Sıcaklık sensörüyle ilgili fonksiyonları içerir. Mevcut versiyonda, donanım sorunları nedeniyle gerçek sensör yerine simülasyon verisi (rastgele sayı) üretir.
* **`eeprom_logger.c / .h`**: I2C üzerinden harici EEPROM'a zaman damgalı veri yazma ve okuma işlemlerini yönetir.

## 7. Proje Durumu ve Bilinen Sorunlar

* Projenin tüm yazılımsal gereksinimleri başarıyla karşılanmıştır.
* Geliştirme sırasında kullanılan DS18B20 sensörü ile yapılan donanım testlerinde kararlı bir iletişim kurulamamıştır. Sorunun fiziksel sensörün arızalı olmasından kaynaklandığı düşünülmektedir.
* Bu nedenle, projenin tam fonksiyonelliğini göstermek amacıyla `sensor_manager` modülü, gerçek sensör verisi yerine **dinamik olarak rastgele sıcaklık verisi üreten bir simülasyon modunda** çalışmaktadır.
