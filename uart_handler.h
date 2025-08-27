/*
 * uart_handler.h
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */

#ifndef UART_HANDLER_H_
#define UART_HANDLER_H_

#include "main.h"


void uart_init(UART_HandleTypeDef *huart); // UART modülünü başlatır ve ilk karakteri dinlemek için kesmeyi (interrupt) aktif eder.
void uart_send_string(const char* str); //Seri porta bir metin (string) gönderir.
void uart_process_command (void);//Gelen komutları işlemek için ana döngüde çağrılacak fonksiyon.
void uart_recieve_callback(void); //Her bir karakter UART'tan geldiğinde HAL kütüphanesi tarafından çağrılacak olan callback fonksiyonu.



#endif /* UART_HANDLER_H_ */
