
#include <c_types.h>
#include <osapi.h>
#include "at_custom.h"
#include <at_push.h>
#include <os_type.h>
#include <spi_flash.h>
#include <eagle_soc.h>
#include <gpio.h>

#include "espush.h"

static uint8 suffix_flag = 1;


void showbuf(uint8* buf, uint32 len)
{
	int i;
	for(i=0; i!=len; ++i) {
		char buf[3];
		os_sprintf(buf, "%02x ", buf[i]);
		at_response(buf);
	}

	at_response("\r\n");
}

void ICACHE_FLASH_ATTR at_recv_push_msg_cb(uint8* pdata, uint32 len)
{
	char buf[16] = { 0 };
	if(suffix_flag) {
		os_sprintf(buf, "\r\n+MSG,%d:", len);
		at_response(buf);
	}
	//uart0_tx_buffer(pdata, len);

	uint8_t tmp = pdata[len];
	pdata[len] = 0;
    at_response(pdata);
    pdata[len] = tmp;

    if(suffix_flag) {
		at_response("\r\n");
	}
}


void ICACHE_FLASH_ATTR atcmd_callback(uint8* atcmd, uint32 len)
{
	at_response((const char*)atcmd);

	if(atcmd[0] == 'A' && atcmd[1] == 'T') {
		at_cmdProcess(atcmd + 2);
	} else {
		at_response("ERROR AT CMD\r\n");
	}
}

void ICACHE_FLASH_ATTR at_queryCmdPushStatus(uint8_t id)
{
	char buf[8] = { 0 };
	sint8 status= espush_server_connect_status();

	os_sprintf(buf, "%d\n", status);
	at_response(buf);
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
	save_espush_cfg(appid_val, appkey, "AT_DEV_ANONYMOUS");
	espush_register(appid_val, appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);

	at_response_ok();
}


uint8 ICACHE_FLASH_ATTR regist_push_from_read_flash()
{
	espush_cfg_s info;

	if(!read_espush_cfg(&info)) {
		at_response("INVALID CONFIG\r\n");
		return 1;
	}

	espush_register(info.app_id, info.appkey, "AT_DEV_ANONYMOUS", VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);
	return 0;
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
		{2, FUNC_GPIO2, PERIPHS_IO_MUX_GPIO2_U},	//uart1 RX口
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


void ICACHE_FLASH_ATTR at_setupServerHost(uint8_t id, char *pPara)
{
	espush_set_server_host(atoi(++pPara));
	at_response_ok();
}


void ICACHE_FLASH_ATTR at_queryServerHost(uint8_t id)
{
	char outStr[16] = { 0 };
	os_sprintf(outStr, "%d", espush_get_server_host());

	at_response(outStr);
	at_response_ok();
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

	sint8 iRet = espush_msg(pPara, strlen(pPara));

	if(iRet == 0) {
		at_response_ok();
	} else if (iRet == 1) {
		at_response("CACHED\r\n");
	} else 	{
		at_response_error();
	}
}


void ICACHE_FLASH_ATTR at_execPushInitial(uint8_t id)
{
	if(regist_push_from_read_flash()) {
		at_response_error();
		return;
	}

	at_response_ok();
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


void ICACHE_FLASH_ATTR at_exec_NetworkCfgTouch(uint8_t id)
{
	espush_network_cfg_by_smartconfig();

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_exec_ListOfflineMsg(uint8_t id)
{
	send_clear_flash_queue();

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_query_ADCU(uint8_t id)
{
	char buf[16] = { 0 };
	os_sprintf(buf, "%d\r\n", system_adc_read());
	at_response(buf);

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_query_espush_apps(uint8_t id)
{
	ESP_DBG("[%s], [%d]\n", __func__, id);

	if(espush_server_connect_status() == STATUS_CONNECTED) {
		push_config* pcfg = (push_config*)espush_get_pushcfg();

		char appkey[APPKEY_LENGTH + 1];
		appkey[APPKEY_LENGTH] = 0;
		os_memcpy(appkey, pcfg->appkey, APPKEY_LENGTH);

		char out[64] = { 0 };
		os_sprintf(out, "+ID:%d\r\n+KEY:%s\r\n", pcfg->appid, appkey);
		at_response(out);
		at_response_ok();
	} else {
		at_response("NOT CONNECTED\r\n");
		at_response_ok();
	}

}


void ICACHE_FLASH_ATTR at_setupHostName(uint8_t id, char* pPara)
{
	wifi_station_set_hostname(++pPara);

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_queryHostname(uint8_t id)
{
	char* hostname = (char*)wifi_station_get_hostname();
	at_response(hostname);

	at_response_ok();
}

void ICACHE_FLASH_ATTR at_query_gpio(uint8_t id)
{
	int i, tmp;
	uint8 length = sizeof(gl_gpio_map) / sizeof(gpio_map_s);

	uint8 gpios[length + 1];
	for(i=0; i!=length; ++i) {
		gpios[i] = '0';
	}
	gpios[length] = 0;

	for(i=0; i!=length; ++i) {
		tmp = 0x1 & GPIO_INPUT_GET(gl_gpio_map[i].pin);
		if(tmp) {
			gpios[i] = '1';
		}
	}

	at_response(gpios);
	at_response("\r\n");

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_queryInfo(uint8_t id)
{
	char out[256] = { 0 };
	uint8 cpuFreq = system_get_cpu_freq();
	uint8 flashMap = system_get_flash_size_map();
	uint8 bootVer = system_get_boot_version();
	uint8 bootMode = system_get_boot_mode();
	uint32 chipid = system_get_chip_id();
	uint32 userbinAddr = system_get_userbin_addr();
	uint32 flashId = spi_flash_get_id();
	uint32 sysTime = system_get_time();
	uint32 rtcTime = system_get_rtc_time();
	uint32 freeHeap = system_get_free_heap_size();

	system_print_meminfo();

	os_sprintf(out, "CPU freq: [%d]\r\n"
			"Flash map: [%d]\r\n"
			"Flash id: [%d]\r\n"
			"Boot version: [%d]\r\n"
			"Boot mode: [%d]\r\n"
			"Chip id: [%d]\r\n"
			"App current: [%d]\r\n"
			"Sys time: [%d]\r\n"
			"Rtc time: [%d]\r\n"
			"Free memory: [%d]\r\n",
			cpuFreq, flashMap, flashId,\
			bootVer, bootMode, chipid,\
			userbinAddr, sysTime, rtcTime,\
			freeHeap);

	at_response(out);

	at_response_ok();
}


void ICACHE_FLASH_ATTR at_setupInterval(uint8_t id, char *pPara)
{
	uint8 interval = atoi(++pPara);
	if(interval < 30) {
		at_response_error();
		return;
	}

	espush_set_heartbeat(interval);
	at_response_ok();
}


void at_uart_trans_rx_intr(uint8* data, int32 len)
{
	ESP_DBG("uart recv: [%s], [%d]\n", data, len);
	if(len >=3 && os_memcmp(data, "+++", 3) == 0) {
		at_register_uart_rx_intr(NULL);
		at_response("\r\nOK\r\n");
	} else {
		uart_stream(data, len);
	}
}


/*
 * 云端透传
 */
void ICACHE_FLASH_ATTR at_exec_UartTrans(uint8_t id)
{
	if(espush_server_connect_status() != 2) {
		at_response("NOT CONNECTED\r\n");
		at_response_ok();

		return;
	}

	at_register_uart_rx_intr(at_uart_trans_rx_intr);
	at_response(">");
}


/*
 * 单设备注册，直接生成DEVID，使用现有的CHIPID注册上去
 * 服务端返回APPID与APPKEY，需要使用额外的命令将其保存
 */
void ICACHE_FLASH_ATTR at_exec_espush_init(uint8_t id)
{
	//char* devid, enum VERTYPE type, msg_cb msgcb
	UUID uuid;
	uint8 devid[DEVID_LENGTH];

	create_uuid(&uuid);
	uuid_to_string(&uuid, devid);

	espush_single_device_init(devid, VER_AT, at_recv_push_msg_cb);
	espush_atcmd_cb(atcmd_callback);

	at_response_ok();
}

/*
 * 将当前APPID与APPKEY写入flash
 */
void ICACHE_FLASH_ATTR at_exec_espush_save(uint8_t id)
{
	if(espush_server_connect_status() == STATUS_CONNECTED) {
		push_config* pcfg = (push_config*)espush_get_pushcfg();
		save_espush_cfg(pcfg->appid, pcfg->appkey, "AT_PLUS SINGLE DEVICES");
	} else {
		at_response("NOT CONNECTED\r\n");
	}

	at_response_ok();
}
