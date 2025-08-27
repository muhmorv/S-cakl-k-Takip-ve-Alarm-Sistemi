/*
 * uart_handler.c
 *
 *  Created on: Jul 30, 2025
 *      Author: Merve KUZ
 */

#include "uart_handler.h"
#include <string.h> // strlen gibi string fonksiyonları için
#include <stdio.h>
#include "system_logic.h"

// UART handle'ını saklamak için bir pointer. uart_init'te ayarlanacak.
static UART_HandleTypeDef *uart_handle;

// Gelen karakterleri biriktireceğimiz tampon (buffer)
static uint8_t rx_buffer[100];

// Bu tamponda şu anki pozisyonumuzu gösteren index
static uint8_t rx_index = 0;

// Tam bir komut geldiğinde 1 olacak olan bayrak (flag)
static volatile uint8_t command_ready = 0;

// HAL kütüphanesinin gelen tek bir byte'ı yazacağı geçici değişken
static uint8_t rx_data;

void uart_init(UART_HandleTypeDef *huart)
{
	uart_handle=huart; //Gelen UART handle'ını pointerımıza atayalım
	// UART'tan 1 byte'lık veri geldiğinde kesme (interrupt) oluşturulmasını etkinleştir.
    // Bu fonksiyon, kesme modunu "dinlemeye hazır" hale getirir.
    HAL_UART_Receive_IT(uart_handle, &rx_data, 1);

}

void uart_send_string(const char* str)
{
	// HAL fonksiyonunu kullanarak string'i gönder.
	HAL_UART_Transmit(uart_handle,(uint8_t*)str,strlen(str),100);
}
void uart_recieve_callback(void)
{
	 system_logic_disable_auto_send();
	if(rx_data=='\r'){
		// Komutun sonuna string'i sonlandıran null karakteri ekle.
	    rx_buffer[rx_index] = '\0';

	    // Komutun işlenmeye hazır olduğunu bildiren bayrağı ayarla.
	    command_ready = 1;

        // Bir sonraki komut için index'i sıfırla.
        rx_index = 0;

	}else {
        // Gelen karakter 'Enter' değilse, buffera ekle.
        // Buffer'ın taşmadığından emin olalım.
        if (rx_index < sizeof(rx_buffer) - 1)
        {
            rx_buffer[rx_index] = rx_data;
            rx_index++;
        }
	}

	//  Bir sonraki karakteri dinlemek için kesmeyi yeniden etkinleştir.
	HAL_UART_Receive_IT(uart_handle, &rx_data, 1);
}

//Ana döngüde çağırılacak komut işleme fonksiyonu
void uart_process_command(void)
{
	 // Komut hazır mı diye kontrol et.
	    if (command_ready == 1)
	    {
	        command_ready = 0; // Bayrağı sıfırla

	        // komutu işlenmesi için system_logic'e gönder.
	        system_logic_parse_command((char*)rx_buffer);

	        system_logic_enable_auto_send();
	    }
}



