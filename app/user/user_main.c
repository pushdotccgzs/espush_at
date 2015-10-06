#include "osapi.h"
#include "at_custom.h"
#include "user_interface.h"
#include "at_push.h"
#include "driver/uart.h"

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
	{"+VDD33U", 7, NULL, at_query_ADCU, NULL, NULL},
	{"+HOSTNAME", 9, NULL, at_queryHostname, at_setupHostName, NULL},
	{"+GPIO", 5, NULL, at_query_gpio, NULL, NULL},
	{"+INFO", 5, NULL, at_queryInfo, NULL, NULL},
	{"+INTERVAL", 9, NULL, NULL, at_setupInterval, NULL},
};

void ICACHE_FLASH_ATTR user_rf_pre_init(void)
{
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);

    char* ver = "espush.cn " "20150821-master-b288c4c5";
    at_customLinkMax = 5;
    at_init();
    at_set_custom_info(ver);
    at_port_print("\r\nready\r\n");
    at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_funcationType));

    system_init_done_cb((init_done_cb_t)regist_push_from_read_flash);
}
