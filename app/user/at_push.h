/*
 * at_push.h
 *
 *  Created on: 2015Äê5ÔÂ25ÈÕ
 *      Author: Sunday
 */

#ifndef APP_USER_AT_PUSH_H_
#define APP_USER_AT_PUSH_H_

#include <c_types.h>

void ICACHE_FLASH_ATTR at_queryCmdPushStatus(uint8_t id);
void ICACHE_FLASH_ATTR at_execUnPushRegist(uint8_t id);
void ICACHE_FLASH_ATTR at_execPushFlagSwitch(uint8_t id);

void ICACHE_FLASH_ATTR at_setupCmdPushMessage(uint8_t id, char* pPara);
void ICACHE_FLASH_ATTR at_setupCmdPushRegistDef(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_setupCmdPushRegistCur(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR regist_push_from_read_flash();

#endif /* APP_USER_AT_PUSH_H_ */
