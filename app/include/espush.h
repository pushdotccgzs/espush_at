/*
 * driver.h
 *
 *  Created on: 2015年5月16日
 *      Author: Sunday
 */

#ifndef APP_INCLUDE_PUSH_H_
#define APP_INCLUDE_PUSH_H_

#include <c_types.h>

#define ESPUSH_VERSION "20150822-master-80bd4f98"


/*
 * 大小端转换，ESP8266属小端序，协议规定为大端序（主要为方便服务端开发）
 */
#define my_htons(_n)  ((uint16)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define my_ntohs(_n)  ((uint16)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#define my_htonl(_n)  ((uint32)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))
#define my_ntohl(_n)  ((uint32)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))


#define read_u32(x) my_htonl(x)
#define read_u16(x) my_htons(x)

#define write_u32(x) my_ntohl(x)
#define write_u16(x) my_ntohs(x)

/*
 * 客户端能力值，uint8型，不得设置值超过255，否则无效。
 */
enum VERTYPE {
	VER_UNKNOWN = 0,
	VER_AT = 1,
	VER_NODEMCU = 2,
	VER_SDK = 3,
	VER_OTHER = 4,
	VER_AT_PLUS = 5,
};

/*
 * UUID
 */
typedef struct {
	uint8 _buf[16];
} UUID;
void ICACHE_FLASH_ATTR uuid_to_string(UUID* puuid, char buf[32]);
void ICACHE_FLASH_ATTR create_uuid(UUID* puuid);
void ICACHE_FLASH_ATTR show_uuid(UUID* puuid);
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
 * 实时状态获取回调
 */
typedef void(*rt_status_cb)(uint32 msgid, char* key, int16_t length);
void ICACHE_FLASH_ATTR espush_rtstatus_cb(rt_status_cb func);
void ICACHE_FLASH_ATTR espush_rtstatus_ret_to_gateway(uint32 cur_msgid, const char* buf, uint8_t length);
void ICACHE_FLASH_ATTR espush_return_to_gateway(uint16 msgtype, uint32 cur_msgid, uint8 opr_type, const char* buf, uint8_t length);

typedef void(*custom_msg_cb)(uint32 cur_msgid, uint8 msgtype, uint16 length, uint8* buf);
void ICACHE_FLASH_ATTR espush_custom_msg_cb(custom_msg_cb func);

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


/*
 * 保存appid、appkey、devid等信息
 * 因为flash的特性，未读写过的空间里也是有内容的
 * 所以需要设置一个hash校验
 * 若校验通过则证明是人工写入的
 */
#define APPKEY_LENGTH 32
#define DEVID_LENGTH 32
#define SSID_MAX_LENGTH 32
#define SSID_PASSWORD_MAX_LENGTH 64
typedef uint32 HASH_CLS;
typedef struct {
	uint32 app_id;
	uint8 appkey[APPKEY_LENGTH];
	char devid[DEVID_LENGTH];
	HASH_CLS hashval;
} __attribute__ ((packed)) espush_cfg_s;;



/*
 * 所有ENUM仅限在uint8的范围内
 */
enum CONN_STATUS {
	STATUS_CONNECTING = 0,
	STATUS_DNS_LOOKUP = 1,
	STATUS_CONNECTED = 2,
	STATUS_DISCONNECTED = 3
};


enum SECOND_BOOT {
	BOOT_UNKNOWN = 0,
	BOOT_USING = 1
};

enum BOOT_APP {
	APP_UNKNOWN = 0,
	APP_USER1 = 1,
	APP_USER2 = 2,
};

enum ESPUSH_CLOUD_CONN_MODE {
	CONN_APP = 0,
	CONN_SINGLE_DEV,
};
/*
 * flash map, 2nd boot, user app
 * flash map的值为 system_get_flash_map的值+1，0 留给 UNKNOWN
 * 2nd boot 与 user app 使用enum，谨慎起见只有确认可以升级才给升级
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
void ICACHE_FLASH_ATTR begin_espush_connect();
/*
 * 单设备注册
 */
void ICACHE_FLASH_ATTR espush_single_device_init(char* devid, enum VERTYPE type, msg_cb msgcb);

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
sint8 ICACHE_FLASH_ATTR espush_msg_plan(uint8* buf, uint16 len, uint32 _timestamp);

/*
 * 或者连接状态, 参考 enum CONN_STATUS 的定义。
 */
sint8 ICACHE_FLASH_ATTR espush_server_connect_status();


//smartconfig 回调
typedef void(*smartconfig_succ_cb)();
void ICACHE_FLASH_ATTR espush_network_cfg_by_smartconfig();
void ICACHE_FLASH_ATTR espush_network_cfg_by_smartconfig_with_callback(smartconfig_succ_cb fn);

void ICACHE_FLASH_ATTR uart_stream(uint8* pdata, uint32 len);

void ICACHE_FLASH_ATTR show_systime();

void ICACHE_FLASH_ATTR espush_set_server_host(uint32 addr);
uint32 ICACHE_FLASH_ATTR espush_get_server_host();


//save espush config
void ICACHE_FLASH_ATTR save_espush_cfg(uint32 app_id, uint8* appkey, uint8* devid);
bool ICACHE_FLASH_ATTR read_espush_cfg(espush_cfg_s* info);
push_config* ICACHE_FLASH_ATTR espush_get_pushcfg();

/*
 * 连接后可获得当前时间
 * 使用unix时间戳表示
 * 返回0 则代表还未连接
 * 留意：连上后，再次断开，时间戳会得到保留。
 */
uint32 ICACHE_FLASH_ATTR get_timestamp();

uint8 ICACHE_FLASH_ATTR set_gpio_edge(uint8 pin, uint8 edge);
void ICACHE_FLASH_ATTR get_gpio_edge_to_buf(uint8 buf[12]);

/*
 * 调试信息开关
 * 与os_printf唯一的区别在于可以输出时间戳
 * 输出系统启动时间
 */
#define ESP_DEBUG 1

#ifdef ESP_DEBUG
#define ESP_DBG(fmt, ...) do {	\
	show_systime();	\
	os_printf(fmt, ##__VA_ARGS__);	\
	}while(0)

#else
#define ESP_DBG
#endif


#endif /* APP_INCLUDE_PUSH_H_ */
