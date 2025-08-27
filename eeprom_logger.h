/*
 * eeprom_logger.h
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */

#ifndef EEPROM_LOGGER_H_
#define EEPROM_LOGGER_H_

#include "main.h"
#include <stdbool.h>

bool eeprom_init(void);
void eeprom_log_data(float temperature, RTC_TimeTypeDef* time, RTC_DateTypeDef* date);
void eeprom_get_all_logs(void);

#endif /* EEPROM_LOGGER_H_ */
