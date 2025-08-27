/*
 * system_logic.h
 *
 *  Created on: Jul 30, 2025
 *      Author: Acer
 */

#ifndef SYSTEM_LOGIC_H_
#define SYSTEM_LOGIC_H_

#include "main.h"

void system_logic_init(void);
void system_logic_run_main_loop(void);

//Kullanıcıdan gelen komutları işler
void system_logic_parse_command(char* cmd);
void system_logic_enable_auto_send(void);
void system_logic_disable_auto_send(void);
#endif /* SYSTEM_LOGIC_H_ */
