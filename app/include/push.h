/*
 * driver.h
 *
 *  Created on: 2015年5月16日
 *      Author: Sunday
 */

#ifndef APP_INCLUDE_PUSH_H_
#define APP_INCLUDE_PUSH_H_

#include <c_types.h>


/*
 * 客户端能力值，uint8型，不得设置值超过255，否则无效。
 */
enum VERTYPE {
	VER_UNKNOWN = 0,
	VER_AT = 1,
	VER_NODEMCU = 2,
	VER_SDK = 3,
	VER_OTHER = 4,
};


/*
 * 数据回调, pdata为网络数据缓冲区，len为数据长度，回调函数在收到网络数据后被异步调用。
 */
typedef void(*msg_cb)(uint8* pdata, uint32 len);

/*
 * AT指令远程执行回调, 收到AT指令后目前是同步执行
 */
typedef void(*atcmd_cb)(uint8* atcmd, uint32 len);
void ICACHE_FLASH_ATTR espush_atcmd_cb(atcmd_cb func);

/*
 * Lua远程执行回调
 */
typedef void(*luafile_cb)(uint8* filebuf, uint32 len);
void ICACHE_FLASH_ATTR espush_luafile_cb(luafile_cb func);


/*
 * appid 与 appkey为平台申请值
 * devid 为设备唯一标志码，32字节，可使用uuid自动生成
 * 或递增出现，为您业务标志，可使用服务端SDK对单个设备进行唯一定位
 */
typedef struct push_config_t {
	uint32 appid;
	uint8 appkey[32];
	uint8 devid[32];
	enum VERTYPE vertype;
	msg_cb msgcb;
}push_config;


enum CONN_STATUS {
	STATUS_CONNECTING = 0,
	STATUS_DNS_LOOKUP = 1,
	STATUS_CONNECTED = 2,
	STATUS_DISCONNECTED = 3
};


/* flash map, 2nd boot, user app
 */
typedef struct regist_info_t {
	uint8 flashmap;
	uint8 second_boot;
	uint8 boot_app;
	uint8 _pad;
}regist_info_s;


/*
 * 注册到espush平台，appid与appkey为设备分类标识，必填，其中appkey为32字节字符串格式
 * vertype参数从 enum VERTYPE中选填，VER_AT与VER_NODEMCU为 AT固件与NodeMCU固件专用，混用可能导致错误
 * devid参数为设备唯一识别码，可以使用芯片的chipid或自行定义，若传入NULL则忽略，系统会自动使用芯片chipid作为唯一识别码
 * msgcb参数为收到数据的回调。
 */
void ICACHE_FLASH_ATTR espush_register(uint32 appid, char appkey[32], char devid[32], enum VERTYPE type, msg_cb msgcb);


/*
 * 推送的固件注册数据初始化，参考 regist_info_s 的定义。
 * 主要是固件 2nd boot，固件SPI_SIZE等。
 */
void ICACHE_FLASH_ATTR espush_init_regist_info(regist_info_s* info);


/*
 * 从平台断开连接，连接断开后将无法再次发送数据，也将不会再收到平台的通知，要启用平台，可重新使用push_regist重新登入。
 */
void ICACHE_FLASH_ATTR espush_unregister();

/*
 * 主动的数据推送
 */
sint8 ICACHE_FLASH_ATTR espush_msg(uint8* buf, uint16 len);

/*
 * 或者连接状态, 参考 enum CONN_STATUS 的定义。
 */
sint8 ICACHE_FLASH_ATTR espush_server_connect_status();


#endif /* APP_INCLUDE_PUSH_H_ */
