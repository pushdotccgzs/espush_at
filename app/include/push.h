/*
 * driver.h
 *
 *  Created on: 2015Äê5ÔÂ16ÈÕ
 *      Author: Sunday
 */

#ifndef APP_INCLUDE_PUSH_H_
#define APP_INCLUDE_PUSH_H_

#include <c_types.h>


enum CONN_STATUS {
	STATUS_CONNECTING = 0,
	STATUS_DNS_LOOKUP = 1,
	STATUS_CONNECTED = 2,
	STATUS_DISCONNECTED = 3
};



enum PUSH_VERSION {
	VER_AT = 0,
	VER_NODEMCU = 1,
	VER_SDK = 2,
};


typedef void(*msg_cb)(uint8* pdata, uint32 len);

void ICACHE_FLASH_ATTR push_register(uint32 appid, char appkey[32], msg_cb msgcb);
void ICACHE_FLASH_ATTR push_unregister();
sint8 ICACHE_FLASH_ATTR push_msg(uint8* buf, uint16 len);
sint8 ICACHE_FLASH_ATTR push_server_connect_status();


#endif /* APP_INCLUDE_PUSH_H_ */
