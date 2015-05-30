/*
 * at_push.h
 *
 *  Created on: 2015Äê5ÔÂ25ÈÕ
 *      Author: Sunday
 */

#ifndef APP_USER_AT_PUSH_H_
#define APP_USER_AT_PUSH_H_

#include <c_types.h>

void at_queryCmdPushStatus(uint8_t id);
void at_setupCmdPushRegist(uint8_t id, char *pPara);

void at_setupCmdPushMessage(uint8_t id, char* pPara);
void at_execUnPushRegist(uint8_t id);
#endif /* APP_USER_AT_PUSH_H_ */
