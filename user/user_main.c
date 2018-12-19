
// Internal 
#include "partition.h"
#include "wifi.h"
#include "config.h"
#include "io_config.h"

// SDK
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <mem.h>
#include <user_interface.h>
#include <driver/uart.h> 
#include <upgrade.h>

// LIB: EasyQ
#include "easyq.h" 
#include "debug.h"


#define STATUS_INTERVAL		200	
#define VERSION				"2.2.1"

LOCAL EasyQSession eq;

enum direction {
	MOTOR_FORWARD,
	MOTOR_BACKWARD
};

LOCAL enum direction current_direction;

void
fota_report_status(const char *q) {
	char str[50];
	float vdd = system_get_vdd33() / 1024.0;

	uint8_t image = system_upgrade_userbin_check();
	os_sprintf(str, "Image: %s Version: "VERSION" VDD: %d.%03d", 
			(UPGRADE_FW_BIN1 == image)? "FOTA": "APP",
			(int)vdd, 
			(int)(vdd*1000)%1000
		);
	easyq_push(&eq, q, str);
}


void ICACHE_FLASH_ATTR
motor_stopped(const char* msg) {
	GPIO_OUTPUT_SET(GPIO_ID_PIN(EN_NUM), 1);
}

void ICACHE_FLASH_ATTR
motor_update_by_message(const char* msg) {
	GPIO_OUTPUT_SET(GPIO_ID_PIN(EN_NUM), 0);
	int steps = atoi(msg);
	//uint32_t steps = motor_change_direction(atoi(msg));
	motor_rotate(steps);
}


void ICACHE_FLASH_ATTR
easyq_message_cb(void *arg, const char *queue, const char *msg, 
		uint16_t message_len) {
	//INFO("EASYQ: Message: %s From: %s\r\n", msg, queue);

	if (strcmp(queue, MOTOR_QUEUE) == 0) { 
		motor_update_by_message(msg);
	}
	else if (strcmp(queue, FOTA_QUEUE) == 0) {
		if (msg[0] == 'R') {
			INFO("Rebooting to FOTA ROM\r\n");
			system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
			system_upgrade_reboot();
		}
		else if (msg[0] == 'I') {
			fota_report_status(FOTA_STATUS_QUEUE);
		}

	}
}


void ICACHE_FLASH_ATTR
easyq_connect_cb(void *arg) {
	INFO("EASYQ: Connected to %s:%d\r\n", eq.hostname, eq.port);
	INFO("\r\n***** Smart Outlet "VERSION"****\r\n");
	const char * queues[] = {MOTOR_QUEUE, FOTA_QUEUE};
	easyq_pull_all(&eq, queues, 2);
}


void ICACHE_FLASH_ATTR
easyq_connection_error_cb(void *arg) {
	EasyQSession *e = (EasyQSession*) arg;
	INFO("EASYQ: Connection error: %s:%d\r\n", e->hostname, e->port);
	INFO("EASYQ: Reconnecting to %s:%d\r\n", e->hostname, e->port);
}


void easyq_disconnect_cb(void *arg)
{
	EasyQSession *e = (EasyQSession*) arg;
	INFO("EASYQ: Disconnected from %s:%d\r\n", e->hostname, e->port);
	easyq_delete(&eq);
}


void wifi_connect_cb(uint8_t status) {
    if(status == STATION_GOT_IP) {
        easyq_connect(&eq);
    } else {
        easyq_disconnect(&eq);
    }
}


void user_init(void) {
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    os_delay_us(60000);

	// Motor 
	PIN_FUNC_SELECT(DIR_MUX, DIR_FUNC);
	PIN_PULLUP_EN(DIR_MUX);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(DIR_NUM), 0);

	PIN_FUNC_SELECT(STEP_MUX, STEP_FUNC);
	PIN_PULLUP_EN(STEP_MUX);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(STEP_NUM), 0);
	
	PIN_FUNC_SELECT(EN_MUX, EN_FUNC);
	PIN_PULLUP_EN(EN_MUX);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(EN_NUM), 1);

    //GPIO_REG_WRITE(
	//		GPIO_PIN_ADDR(GPIO_ID_PIN(RELAY1_NUM)), 
	//		GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(RELAY1_NUM))) 
	//		| GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)
	//	); //open drain;
		
	EasyQError err = easyq_init(&eq, EASYQ_HOSTNAME, EASYQ_PORT, EASYQ_LOGIN);
	if (err != EASYQ_OK) {
		ERROR("EASYQ INIT ERROR: %d\r\n", err);
		return;
	}
	eq.onconnect = easyq_connect_cb;
	eq.ondisconnect = easyq_disconnect_cb;
	eq.onconnectionerror = easyq_connection_error_cb;
	eq.onmessage = easyq_message_cb;
	motor_set_stop_callback(motor_stopped);
	motor_init();
    WIFI_Connect(WIFI_SSID, WIFI_PSK, wifi_connect_cb);
    INFO("System started ...\r\n");
}


void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, 
				sizeof(at_partition_table)/sizeof(at_partition_table[0]),
				SPI_FLASH_SIZE_MAP)) {
		FATAL("system_partition_table_regist fail\r\n");
		while(1);
	}
}

