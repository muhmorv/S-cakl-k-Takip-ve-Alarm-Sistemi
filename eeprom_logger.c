/*
 * eeprom_logger.c
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */

#include "eeprom_logger.h"
#include "uart_handler.h"
#include <stdio.h>
#include <stdbool.h>

// main.c'de tanımlı olan I2C ve RTC handle'larına erişim
extern I2C_HandleTypeDef hi2c1;
extern RTC_HandleTypeDef hrtc;

// AT24C serisi EEPROM'ların genel I2C adresi
#define EEPROM_I2C_ADDR 0xA0

typedef struct {
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	float temp;
} LogEntry ;

static uint16_t log_count=0;

bool eeprom_init(void){
	 if (HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_I2C_ADDR, 3, 10) == HAL_OK)
	    {
	        // Cihaz hazırsa, true döndür.
	        return true;
	    }
	    else
	    {
	        // Değilse, false döndür.
	        return false;
	    }
}

void eeprom_log_data(float temperature, RTC_TimeTypeDef* time, RTC_DateTypeDef* date){

	LogEntry log;
	log.time=*time;
	log.date=*date;
	log.temp=temperature;

	HAL_I2C_Mem_Write(&hi2c1,EEPROM_I2C_ADDR,log_count*sizeof(LogEntry),I2C_MEMADD_SIZE_16BIT,(uint8_t*)&log,sizeof(LogEntry),100);

	log_count++;

}

void eeprom_get_all_logs(void){
	if(log_count==0){
		uart_send_string("NO RECORD FOUND\r\n");
	}else {
		uart_send_string("----REGISTRATION START----\r\n")  ;
		for( uint16_t i =0;i<log_count;i++){
			LogEntry current_log;
			char buffer[120];

			HAL_I2C_Mem_Read(&hi2c1, EEPROM_I2C_ADDR, i*sizeof(LogEntry), I2C_MEMADD_SIZE_16BIT, (uint8_t*)&current_log, sizeof(LogEntry), 100);

			sprintf(buffer,"Record %d:[%02d/%02d] %02d:%02d:%02d - %.1f C\r\n", i+1,
					current_log.date.Date,current_log.date.Month,current_log.time.Hours,
					current_log.time.Minutes,current_log.time.Seconds, current_log.temp );

			uart_send_string(buffer);

			HAL_Delay(100);
		}
		uart_send_string("-----END OF THE RECORD----\r\n");
	}
}
