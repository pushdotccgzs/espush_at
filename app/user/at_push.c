
#include <c_types.h>
#include <osapi.h>
#include "at_custom.h"
#include <at_push.h>

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
	if(pdata[0] == 'A' && pdata[1] == 'T') {
		uart0_sendStr("AT CMD \r\n");
		showbuf(pdata, len);
		at_cmdProcess(pdata);

		return;
	}
	os_sprintf(buf, "\r\n+MSG,%d:", len);
	uart0_sendStr(buf);
	uart0_tx_buffer(pdata, len);
	uart0_sendStr("\r\n");
}


void ICACHE_FLASH_ATTR at_queryCmdPushStatus(uint8_t id)
{
	char buf[8] = { 0 };
	sint8 status= push_server_connect_status();

	os_sprintf(buf, "%d\n", status);
	uart0_sendStr(buf);
	at_response_ok();
}


void ICACHE_FLASH_ATTR at_setupCmdPushRegist(uint8_t id, char *pPara)
{
	char* param = pPara;
	char* appid = NULL;
	char* appkey = NULL;
	uint32 appid_val = 0;

	++param;
	while(*param) {
		if(*param == ',') {
			//’“µΩ
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
	push_register(appid_val, appkey, at_recv_push_msg_cb);

	at_response_ok();
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

