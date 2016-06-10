#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"
#include "at_push.h"
#include "driver/key.h"
#include "push.h"

at_funcationType at_custom_cmd[] = {
	{"+PUSH", 5, NULL, at_queryCmdPushStatus, NULL, NULL},
	{"+PUSH_DEF", 9, NULL, NULL, at_setupCmdPushRegistDef, NULL},
	{"+PUSH_CUR", 9, NULL, NULL, at_setupCmdPushRegistCur, NULL},
	{"+PUSHMSG", 8, NULL, NULL, at_setupCmdPushMessage, NULL},
	{"+PUSHCLOSE", 10, NULL, NULL, NULL, at_execUnPushRegist},
	{"+PUSH_INIT", 10, NULL, NULL, NULL, at_execPushInitial},
	{"+PUSH_FLAG", 10, NULL, NULL, NULL, at_execPushFlagSwitch},
	{"+GPIO_HIGH", 10, NULL, NULL,at_setupGPIOEdgeLow, NULL},
	{"+GPIO_LOW", 9, NULL, NULL,at_setupGPIOEdgeHigh, NULL},
	{"+N_AP", 5, NULL, NULL, NULL, at_exec_NetworkCfgAp},
	{"+N_SMC", 6, NULL, NULL, NULL, at_exec_NetworkCfgTouch},
	{"+OFFLINES", 9, NULL, NULL, NULL, at_exec_ListOfflineMsg},
	{"+ADCU", 5, NULL, at_query_ADCU, NULL, NULL},
	{"+HOSTNAME", 9, NULL, at_queryHostname, at_setupHostName, NULL},
	{"+GPIO", 5, NULL, at_query_gpio, NULL, NULL},
	{"+INFO", 5, NULL, at_queryInfo, NULL, NULL},
	{"+INTERVAL", 9, NULL, NULL, at_setupInterval, NULL},
	{"+UARTRANS", 9, NULL, NULL, NULL, at_exec_UartTrans},
	{"+PUSH_CONN", 10, NULL, NULL, NULL, at_exec_espush_init},
	{"+PUSH_SAVE", 10, NULL, NULL, NULL, at_exec_espush_save},
	{"+PUSH_APPS", 10, NULL, at_query_espush_apps, NULL, NULL},
	{"+ESPUSH_TEST", 12, NULL, at_queryServerHost, at_setupServerHost, NULL},
};

void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}


LOCAL void ICACHE_FLASH_ATTR settings_btn_long_press(void)
{
	ESP_DBG("long btn\n");
	at_response_ok();
	espush_network_cfg_by_smartconfig();
}


LOCAL void ICACHE_FLASH_ATTR settings_btn_short_press(void)
{
	ESP_DBG("short btn\n");
	at_response_ok();
}


LOCAL void ICACHE_FLASH_ATTR local_settings_btn_long_press(void)
{
	ESP_DBG("long btn\n");
	at_response_ok();
	//espush_local_init("iEspush", "12345678", VER_AT, NULL);
}


void settings_key_init()
{
	static struct keys_param keys;
	static struct single_key_param *keys_param[2];
	keys_param[0] = key_init_single(4, PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, local_settings_btn_long_press, settings_btn_short_press);
	keys_param[1] = key_init_single(5, PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5, settings_btn_long_press, settings_btn_short_press);
    keys.key_num = 2;
    keys.single_key = keys_param;
    key_init(&keys);
}


void ICACHE_FLASH_ATTR user_init(void)
{
//	uart_init(BIT_RATE_115200, BIT_RATE_115200);

    char* ver = "espush.cn ";
    at_customLinkMax = 5;
    at_init();
    at_set_custom_info(ver);
    at_port_print("\r\nready\r\n");
    at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_funcationType));

//    settings_key_init();
    system_init_done_cb((init_done_cb_t)regist_push_from_read_flash);
}
