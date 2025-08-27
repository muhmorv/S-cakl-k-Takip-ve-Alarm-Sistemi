/*
 * system_logic.c
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */
#include "system_logic.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart_handler.h"
#include "sensor_manager.h"
#include "eeprom_logger.h"

extern RTC_HandleTypeDef hrtc;
#define HIGH_ALARM_LED_PORT GPIOA
#define HIGH_ALARM_LED_PIN GPIO_PIN_12
#define LOW_ALARM_LED_PORT GPIOA
#define LOW_ALARM_LED_PIN GPIO_PIN_11

static uint32_t last_measurement_tick=0;

// Eşik değerlerini saklamak için statik değişkenler
static float high_threshold = 30.0f;
static float low_threshold = 15.0f;
static bool send_flag =true;

void system_logic_enable_auto_send(void)
{
    send_flag = true;
}

void system_logic_disable_auto_send(void)
{
    send_flag = false;
}

void system_logic_parse_command(char* cmd)
{
    float temp_val;
    char response_str[64];
    // Gelen komut "SET HIGH" ile mi başlıyor?
    if (strncmp(cmd, "SET HIGH", 8) == 0)
    {
        if (sscanf(cmd + 8, "%f", &temp_val) == 1)
        {
            high_threshold = temp_val;
            sprintf(response_str, " Yuksek esik %.1f olarak ayarlandi.\r\n", high_threshold);
            uart_send_string(response_str);
        }
    }
    // Gelen komut "SET LOW" ile mi başlıyor?
    else if (strncmp(cmd, "SET LOW", 7) == 0)
    {    // Komutun sayı kısmını okumaya çalış
        if (sscanf(cmd + 7, "%f", &temp_val) == 1)
        {
            low_threshold = temp_val;
            sprintf(response_str, "888 Dusuk esik %.1f olarak ayarlandi.\r\n", low_threshold);
            uart_send_string(response_str);
        }
    }
    else if(strcmp(cmd,"GET LOGS")==0){
    	eeprom_get_all_logs();
    }

    else
    {
        uart_send_string("Hata: Bilinmeyen komut.\r\n");
    }
}
void system_logic_init(void)
{
    // Bu fonksiyon, program ilk çalıştığında bir kez çağrılır.

    uart_send_string("Sistem baslatildi. Komut bekleniyor...\r\n");
}

void system_logic_run_main_loop(void)
{
    // Bu fonksiyon, main.c'deki while(1) döngüsü tarafından sürekli çağrılacak.
    // Projenin tüm ana işleri burada dönecek.

    //  Gelen bir komut var mı diye UART modülünü kontrol et ve işle.
    uart_process_command();
    //  Her 1 saniyede bir sensör okuması ve alarm kontrolü yap.
     uint32_t current_tick = HAL_GetTick();
     if (current_tick - last_measurement_tick >= 1000)
     {
         // Son ölçüm zamanını güncelle
         last_measurement_tick = current_tick;


         float temp = sensor_read_temperature();

         RTC_TimeTypeDef sTime= {0};
         RTC_DateTypeDef sDate= {0};
         HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
         HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

         eeprom_log_data(temp,&sTime,&sDate);

         char alarm_status[20] = "NORMAL";

         // Alarmları kontrol et

        if (temp > high_threshold)
         {
             HAL_GPIO_WritePin(HIGH_ALARM_LED_PORT, HIGH_ALARM_LED_PIN, GPIO_PIN_SET); // Yüksek alarm LED'ini yak
             HAL_GPIO_WritePin(LOW_ALARM_LED_PORT, LOW_ALARM_LED_PIN, GPIO_PIN_RESET); // Düşük alarm LED'ini söndür
             strcpy(alarm_status, "YUKSEK ALARM!");
         }
         else if (temp < low_threshold)
         {
             HAL_GPIO_WritePin(HIGH_ALARM_LED_PORT, HIGH_ALARM_LED_PIN, GPIO_PIN_RESET);
             HAL_GPIO_WritePin(LOW_ALARM_LED_PORT, LOW_ALARM_LED_PIN, GPIO_PIN_SET); // Düşük alarm LED'ini yak
             strcpy(alarm_status, "DUSUK ALARM!");
         }
         else
         {
        	 if(temp==-99.9f)
        	          {
        	         	 uart_send_string("__SENSOR BAGLANTI HATASI___\n");
        	         	 strcpy(alarm_status,"SENSOR BULUNAMADI");
        	          }
             // Normal durum, tüm alarmları kapat
             HAL_GPIO_WritePin(HIGH_ALARM_LED_PORT, HIGH_ALARM_LED_PIN, GPIO_PIN_RESET);
             HAL_GPIO_WritePin(LOW_ALARM_LED_PORT, LOW_ALARM_LED_PIN, GPIO_PIN_RESET);
         }

         if (send_flag)
                {
                    char status_str[120];
                    sprintf(status_str, "[%02d:%02d:%02d] Sicaklik: %.1f C, Durum: %s\r\n",
                            sTime.Hours, sTime.Minutes, sTime.Seconds,
                            temp,
                            alarm_status);
                    uart_send_string(status_str);
                }

     	 	  }

}


