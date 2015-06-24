
#include <c_types.h>
#include <osapi.h>
#include "at_custom.h"
#include <at_push.h>
#include <os_type.h>
#include <spi_flash.h>
#include <eagle_soc.h>
#include <gpio.h>

#include "push.h"

static uint8 suffix_flag = 1;


void showbuf(uint8* buf, uint32 len)
{
	int i;
	for(i=0; i!=len; ++i) {
		char buf[3];
		os_sprintf(buf, "%02x ", buf[i]);
		uart0_sendStr(buf);
	}

	uart0_sendStr("\r\n");
}

void ICACHE_FLASH_ATTR at_recv_push_msg_cb(uint8* pdata, uint32 len)
{
	char buf[16] = { 0 };
	if(suffix_flag) {
		os_sprintf(buf, "\r\n+MSG,%d:", len);
		uart0_sendStr(buf);
	}
	uart0_tx_buffer(pdata, len);
	if(suffix_flag) {
		uart0_sendStr("\r\n");
	}
}


void ICACHE_FLASH_ATTR atcmd_callback(uint8* atcmd, uint32 len)
{
	uart0_sendStr((const char*)atcmd);

	if(atcmd[0] == 'A' && atcmd[1] == 'T') {
		at_cmdProcess(atcmd + 2);
	} else {
		uart0_sendStr("ERROR AT CMD\r\n");
	}
}

void ICACHE_FLASH_ATTR at_queryCmdPushStatus(uint8_t id)
{
	char buf[8] = { 0 };
	sint8 status= espush_server_connect_status();

	os_sprintf(buf, "%d\n", status);
	uart0_sendStr(buf);
	at_response_ok();
}


void ICACHE_FLASH_ATTR at_setupCmdPushRegistCur(uint8_t id, char *pPara)
{
	char* param = pPara;
	char* appid = NULL;
	char* appkey = NULL;
	uint32 appid_val = 0;

	++param;
	while(*param) {
		if(*param == ',') {
			//找到
			*param = 0;
			appid = pPara + 1;
			appkey = param + 1;
			break;
		}
		param++;
	}

	if(!appid || !appkey) {
		at_response_error();
		return;
	}

	if(os_strlen(appkey) != 32) {
		at_response_error();
		return;
	}

	appid_val = atoi(appid);
	if(appid_val == 0) {
		at_response_error();
		return;
	}

	espush_register(appid_val, appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);

	at_response_ok();
}


typedef struct {
	uint32 app_id;
	uint8 appkey[32];
	uint32 hashval;
} push_info_s;


bool ICACHE_FLASH_ATTR check_push_info_hash(push_info_s* info)
{
	uint32 hash_val = 0x3c000;
	uint8 i;

	uint16 length = 36;
	for(i=0; i!=length; ++i) {
		hash_val += ((uint8*)info)[i];
	}

	return hash_val == info->hashval;
}


void ICACHE_FLASH_ATTR set_push_info_hash(push_info_s* info)
{
	uint32 hash_val = 0x3c000;
	uint8 i;

	uint16 length = 36;
	for(i=0; i!=length; ++i) {
		hash_val += ((uint8*)info)[i];
	}

	info->hashval = hash_val;
}


void ICACHE_FLASH_ATTR save_push_info(uint32 app_id, uint8* appkey)
{
	uint32 addr = 0x3C000;
	uint16 page_per = 4096;

	SpiFlashOpResult result = spi_flash_erase_sector(addr / page_per);
	if(result != SPI_FLASH_RESULT_OK) {
		uart0_sendStr("ERROR erase_sector\n");
		return;
	}

	push_info_s info;
	info.app_id = app_id;
	os_memcpy(info.appkey, appkey, os_strlen(appkey));
	set_push_info_hash(&info);

	result = spi_flash_write(addr, (uint32*)&info, sizeof(info));
	if(result != SPI_FLASH_RESULT_OK) {
		uart0_sendStr("ERROR write\n");
		return;
	}
}


bool ICACHE_FLASH_ATTR read_push_info(push_info_s* info)
{
	uint32 addr = 0x3C000;
	uint16 page_per = 4096;

	SpiFlashOpResult result = spi_flash_read(addr, (uint32*)info, sizeof(push_info_s));
	if(result != SPI_FLASH_RESULT_OK) {
		uart0_sendStr("ERROR read\n");
		return false;
	}

	if(!check_push_info_hash(info)) {
//		uart0_sendStr("check hash error\n");
		return false;
	}

	return true;
}


void ICACHE_FLASH_ATTR at_setupCmdPushRegistDef(uint8_t id, char *pPara)
{
	char* param = pPara;
	char* appid = NULL;
	char* appkey = NULL;
	uint32 appid_val = 0;

	++param;
	while(*param) {
		if(*param == ',') {
			//找到
			*param = 0;
			appid = pPara + 1;
			appkey = param + 1;
			break;
		}
		param++;
	}

	if(!appid || !appkey) {
		at_response_error();
		return;
	}

	appid_val = atoi(appid);
	save_push_info(appid_val, appkey);
	espush_register(appid_val, appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);

	at_response_ok();
}


void ICACHE_FLASH_ATTR regist_push_from_read_flash()
{
	push_info_s info;

	if(!read_push_info(&info)) {
//		uart0_sendStr("read flash info error\n");
		return;
	}

	espush_register(info.app_id, info.appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);
}


typedef struct {
	uint8 pin;
	uint8 func_name;
	uint32 pin_name;
}gpio_map_s;

gpio_map_s gl_gpio_map[] = {
		//0 ~ 5
		{0, FUNC_GPIO0, PERIPHS_IO_MUX_GPIO0_U},
		{1, FUNC_GPIO1, PERIPHS_IO_MUX_U0TXD_U},	//同是串口tx口
		{2, FUNC_GPIO2, PERIPHS_IO_MUX_GPIO2_U},
		{3, FUNC_GPIO3, PERIPHS_IO_MUX_U0RXD_U},	//串口RX口
		{4, FUNC_GPIO4, PERIPHS_IO_MUX_GPIO4_U},
		{5, FUNC_GPIO5, PERIPHS_IO_MUX_GPIO5_U},
		//9 ~ 10
		{9, FUNC_GPIO9, PERIPHS_IO_MUX_SD_DATA2_U},
		{10, FUNC_GPIO10, PERIPHS_IO_MUX_SD_DATA3_U},
		//12~15
		{12, FUNC_GPIO12, PERIPHS_IO_MUX_MTDI_U},
		{13, FUNC_GPIO13, PERIPHS_IO_MUX_MTCK_U},
		{14, FUNC_GPIO14, PERIPHS_IO_MUX_MTMS_U},
		{15, FUNC_GPIO15, PERIPHS_IO_MUX_MTDO_U},
};


uint8 ICACHE_FLASH_ATTR set_gpio_edge(uint8 pin, uint8 edge)
{
	uint8 i;
	uint8 length = sizeof(gl_gpio_map) / sizeof(gpio_map_s);
	for(i=0; i != length; ++i) {
		if(gl_gpio_map[i].pin == pin) {
			PIN_FUNC_SELECT(gl_gpio_map[i].pin_name, gl_gpio_map[i].func_name);
			GPIO_OUTPUT_SET(GPIO_ID_PIN(pin), edge);
			return 0;
		}
	}

	return 1;
}


void ICACHE_FLASH_ATTR at_setupGPIOEdgeLow(uint8_t id, char *pPara)
{
	uint8 pin = atoi(++pPara);
	uint8 length = sizeof(gl_gpio_map) / sizeof(gpio_map_s);
	uint8 max_pin = gl_gpio_map[length - 1].pin;
	if(pin > max_pin) {
		at_response_error();
		return;
	}

	if(set_gpio_edge(pin, 0)) {
		at_response_error();
	} else {
		at_response_ok();
	}
}


void ICACHE_FLASH_ATTR at_setupGPIOEdgeHigh(uint8_t id, char *pPara)
{
	uint8 pin = atoi(++pPara);
	uint8 length = sizeof(gl_gpio_map) / sizeof(gpio_map_s);
	uint8 max_pin = gl_gpio_map[length - 1].pin;
	if(pin > max_pin) {
		at_response_error();
		return;
	}

	if(set_gpio_edge(pin, 1)) {
		at_response_error();
	} else {
		at_response_ok();
	}
}


void ICACHE_FLASH_ATTR at_setupCmdPushMessage(uint8_t id, char* pPara)
{
	++pPara;

	if(espush_msg(pPara, strlen(pPara)) == 0) {
		at_response_ok();
	} else {
		at_response_error();
	}
}


void ICACHE_FLASH_ATTR at_execUnPushRegist(uint8_t id)
{
	espush_unregister();

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_execPushFlagSwitch(uint8_t id)
{
	suffix_flag = !suffix_flag;

	at_response_ok();
}


/*
 * TODO:
 * [√] APPID于APPKEY的值合法性判定
 * [√] GPIO 控制指令
 */
