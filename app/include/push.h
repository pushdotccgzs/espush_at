/*
 * driver.h
 *
 *  Created on: 2015年5月16日
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



enum VERTYPE {
	VER_UNKNOWN = 0,
	VER_AT = 1,
	VER_NODEMCU = 2,
	VER_SDK = 3,
	VER_OTHER = 4,
};

/*
 * 数据回调
 */
typedef void(*msg_cb)(uint8* pdata, uint32 len);

/*
 * AT指令远程执行回调
 */
typedef void(*atcmd_cb)(uint8* atcmd, uint32 len);
void ICACHE_FLASH_ATTR espush_atcmd_cb(atcmd_cb func);

/*
 * Lua远程执行回调
 */
typedef void(*luafile_cb)(uint8* filebuf, uint32 len);
void ICACHE_FLASH_ATTR espush_luafile_cb(luafile_cb func);

/*
 * 注册到espush平台，appid与appkey为设备分类标识，必填，其中appkey为32字节字符串格式
 * vertype参数从 enum VERTYPE中选填，VER_AT与VER_NODEMCU为 AT固件与NodeMCU固件专用，混用可能导致错误
 * devid参数为设备唯一识别码，可以使用芯片的chipid或自行定义，若传入NULL则忽略，系统会自动使用芯片chipid作为唯一识别码
 * msgcb参数为收到数据的回调。
 */
void ICACHE_FLASH_ATTR push_register(uint32 appid, char appkey[32], char devid[32], enum VERTYPE type, msg_cb msgcb);

/*
 * 从平台断开连接，连接断开后将无法再次发送数据，也将不会再收到平台的通知，要启用平台，可重新使用push_regist重新登入。
 */
void ICACHE_FLASH_ATTR push_unregister();

/*
 * 主动的数据推送
 */
sint8 ICACHE_FLASH_ATTR push_msg(uint8* buf, uint16 len);

/*
 * 或者连接状态, 参考 enum CONN_STATUS 的定义。
 */
sint8 ICACHE_FLASH_ATTR push_server_connect_status();


#endif /* APP_INCLUDE_PUSH_H_ */
