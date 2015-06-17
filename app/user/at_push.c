
#include <c_types.h>
#include <osapi.h>
#include "at_custom.h"
#include <at_push.h>
#include <os_type.h>
#include <spi_flash.h>

#include "push.h"

static uint8 suffix_flag = 1;


void showbuf(uint8* buf, uint32 len)
{
	int i;
	for(i=0; i!=len; ++i) {
		char buf[3];
		os_sprintf(buf, "%02X ", buf[i]);
		uart0_sendStr(buf);
	}
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
	if(atcmd[0] == 'A' && atcmd[1] == 'T') {
		char cmdbuf[len + 3]; // \r\n\0
		os_memcpy(cmdbuf, atcmd, len);
		os_memcpy(cmdbuf + len, "\r\n\0", 3);
		at_cmdProcess(cmdbuf);
	} else {
		uart0_sendStr("ERROR AT CMD\r\n");
	}
}

void ICACHE_FLASH_ATTR at_queryCmdPushStatus(uint8_t id)
{
	char buf[8] = { 0 };
	sint8 status= push_server_connect_status();

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

	push_register(appid_val, appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
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
	push_register(appid_val, appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
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

	push_register(info.app_id, info.appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);
}


void ICACHE_FLASH_ATTR at_setupCmdPushMessage(uint8_t id, char* pPara)
{
	++pPara;

	if(push_msg(pPara, strlen(pPara)) == 0) {
		at_response_ok();
	} else {
		at_response_error();
	}
}


void ICACHE_FLASH_ATTR at_execUnPushRegist(uint8_t id)
{
	push_unregister();

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_execPushFlagSwitch(uint8_t id)
{
	suffix_flag = !suffix_flag;

	at_response_ok();
}


/*
 * TODO:
 * [ ] APPID于APPKEY的值合法性判定
 */
