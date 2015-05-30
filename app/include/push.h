/*
 * push.h
 *
 *  Created on: 2015年5月16日
 *      Author: Sunday
 */
#include <c_types.h>

#ifndef APP_INCLUDE_PUSH_H_
#define APP_INCLUDE_PUSH_H_

typedef void(*msg_cb)(uint8* pdata, uint32 len);

void ICACHE_FLASH_ATTR push_register(uint32 appid, uint8* appkey, msg_cb msgcb);
void ICACHE_FLASH_ATTR push_unregister();

//主动上传消息
sint8 ICACHE_FLASH_ATTR push_msg(uint8* buf, uint16 len);


sint8 ICACHE_FLASH_ATTR push_server_connect_status();


#endif /* APP_INCLUDE_PUSH_H_ */
