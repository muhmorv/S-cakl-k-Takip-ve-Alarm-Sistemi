/*
 * sensor_manager.c
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */

#include "sensor_manager.h"
#include "main.h"
#include <stdlib.h>


#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_PIN_8

// main.c dosyasında CubeMX tarafından oluşturulan timer handle'ına
// bu dosyadan da erişmek için 'extern' anahtar kelimesini kullanıyoruz.
extern TIM_HandleTypeDef htim2;
extern RTC_HandleTypeDef hrtc;

/*// 1 dakikalık (60 saniye) simülasyon için sıcaklık verileri (Look Up Table)
static const float temperature_lut[60] = {
    22.5, 22.6, 22.8, 23.1, 23.5, 24.0, 12, 25.3, 26.1, 27.0,
    28.0, 29.1, 30.2, 31.0, 31.5, 13, 32.1,95, 31.1, 30.5,
    29.8, 29.0, 28.1, 27.2, 26.3, 25.4, 24.5, 5.6, 22.7, 21.8,
    34, 20.0, 19.2, 18.5, 17.8, 17.0, 16.3, 9, 15.1, 14.8,
    40, 14.0, 5, 13.5, 13.8, 48.5, 14.7, 36, 16.0, 16.8,
    17.6, 18.5, 19.4, 41, 20.8, 52, 21.7, 22.0, 61, 37.9
};

// Look Up Table'da mevcut pozisyonumuzu tutan index
static uint8_t lut_index = 0;*/


/*static void delay_us(uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2,0); //Timerın sayacını sıfırladık

    // Sayaç, istenen mikrosaniye değerine ulaşana kadar bu döngüde bekle.
	while(__HAL_TIM_GET_COUNTER(&htim2) < us);

}

static void set_pin_output (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // Open-Drain modu
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

static void set_pin_input (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = DS18B20_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}*/
void sensor_init(void)
{
    RTC_TimeTypeDef sTime = {0};
    // RTC'den o anki zamanı al
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    // Rastgele sayı üretecini o anki saniye değeriyle başlat
    srand(sTime.Seconds);
}


/*uint8_t sensor_init(void)
{
	uint8_t response=0;
	//Reset Sinyali Gönder
	set_pin_output();// Pini ÇIKIŞ yap.
	HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN,GPIO_PIN_RESET); // Hattı LOW'a çek.
	delay_us(480); // Reset sinyali için 480µs bekle.

	//Sensörün cevabını dinle
	set_pin_input(); // Pini GİRİŞ yap ve hattı dinlemeye başla
	delay_us(80); // Sensörün cevap vermek için hattı LOW'a çekmesini bekle (60-240µs arası).

	//Cevabı kontrol et
	if(!(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)))
	{
		response=1;
	}

	delay_us(400);

	return response;

}*/
/*
static uint8_t ds18b20_read_byte(void){
	uint8_t data=0;
	set_pin_input();

	for(int i=0;i<8;i++){
		set_pin_output();
		HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
		set_pin_input();
		delay_us(1);
		if(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)){
			 data |= (1 << i);
			}
			delay_us(50);
	}
	return data;
}
static uint8_t ds18b20_write_byte(uint8_t data){
	set_pin_output();
	for(int i=0;i<8;i++){
		if((data & 0x01)){
			set_pin_output();
			HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
			delay_us(1);
			set_pin_input();
			delay_us(50);
		}
		else {
			set_pin_output();
			HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
			delay_us(60);
			set_pin_input();
		}
		data >>= 1;
	}
	return data;
}


float sensor_read_temperature(void){
    uint8_t scratchpad[9] = {0};
    int16_t raw_temp;
    float temperature;

    // --- Adım 1: Sıcaklık ölçümünü başlat ---
    if (sensor_init() == 0) // Sensör cevap vermiyorsa
    {
        return -99.9f; // Hata kodu olarak geçersiz bir sıcaklık döndür
    }
    ds18b20_write_byte(0xCC); // SKIP ROM: Adreslemeyi atla, tek sensör var
    ds18b20_write_byte(0x44); // CONVERT T: Sıcaklığı ölçmeye başla komutu

    // --- Adım 2: Sensörün ölçümü tamamlaması için bekle ---
    HAL_Delay(750);

    // --- Adım 3: Ölçülen veriyi sensörden iste ---
    if (sensor_init() == 0) // Tekrar 'init' ile iletişim başlat
    {
        return -99.9f; // Hata kodu
    }
    ds18b20_write_byte(0xCC); // SKIP ROM
    ds18b20_write_byte(0xBE); // READ SCRATCHPAD: Ölçüm defterini oku komutu

    // --- Adım 4: Sensörden gelen 9 byte'lık veriyi oku ---
    for (int i = 0; i < 9; i++)
    {
        scratchpad[i] = ds18b20_read_byte();
    }

    // --- Adım 5: Gelen ham veriyi sıcaklığa çevir ---
    raw_temp = (scratchpad[1] << 8) | scratchpad[0];
    temperature = (float)raw_temp / 16.0f;

    return temperature;
}

*/
float sensor_read_temperature(void){

	// Hedef aralığı belirle
	    float min_temp = -25.0f;
	    float max_temp = 125.0f;

	    // 1. rand() ile 0 ve RAND_MAX arasında rastgele bir tam sayı üret.
	    // 2. Bunu 0.0 ve 1.0 arasında bir ondalık sayıya çevir (ölçeklendir).
	    float scale = rand() / (float)RAND_MAX;

	    // 3. Bu ölçeklenmiş değeri, istediğimiz aralığa (-25.0 ila 125.0) yay.
	    float temp = min_temp + scale * (max_temp - min_temp);

	    return temp;

}




