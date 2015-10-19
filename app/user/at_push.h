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
uint8 ICACHE_FLASH_ATTR regist_push_from_read_flash();

void ICACHE_FLASH_ATTR at_setupGPIOEdgeLow(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_setupGPIOEdgeHigh(uint8_t id, char *pPara);

void ICACHE_FLASH_ATTR at_exec_NetworkCfgAp(uint8_t id);
void ICACHE_FLASH_ATTR at_exec_NetworkCfgTouch(uint8_t id);
void ICACHE_FLASH_ATTR at_exec_ListOfflineMsg(uint8_t id);


void ICACHE_FLASH_ATTR at_exec_UartTrans(uint8_t id);
void ICACHE_FLASH_ATTR at_exec_espush_init(uint8_t id);
void ICACHE_FLASH_ATTR at_exec_espush_save(uint8_t id);

void ICACHE_FLASH_ATTR at_query_ADCU(uint8_t id);
void ICACHE_FLASH_ATTR at_query_espush_apps(uint8_t id);

void ICACHE_FLASH_ATTR at_setupHostName(uint8_t id, char* pPara);

void ICACHE_FLASH_ATTR at_query_gpio(uint8_t id);
void ICACHE_FLASH_ATTR at_queryInfo(uint8_t id);
void ICACHE_FLASH_ATTR at_queryHostname(uint8_t id);


void ICACHE_FLASH_ATTR at_setupInterval(uint8_t id, char *pPara);
void ICACHE_FLASH_ATTR at_execPushInitial(uint8_t id);


#endif /* APP_USER_AT_PUSH_H_ */
