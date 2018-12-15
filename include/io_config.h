#ifndef _IO_CONFIG_H__
#define _IO_CONFIG_H__

#define EASYQ_RECV_BUFFER_SIZE  4096
#define EASYQ_SEND_BUFFER_SIZE  512 
#define EASYQ_HOSTNAME			"192.168.8.44"
#define EASYQ_PORT				1085

#define DEVICE_NAME				"st"
#define EASYQ_LOGIN				DEVICE_NAME
#define RELAY1_QUEUE			DEVICE_NAME":1"
#define FOTA_QUEUE				DEVICE_NAME":fota"
#define FOTA_STATUS_QUEUE		DEVICE_NAME":fota:status"

/* GPIO */

// Relay #1
#define RELAY1_MUX		PERIPHS_IO_MUX_GPIO5_U
#define RELAY1_NUM		5
#define RELAY1_FUNC		FUNC_GPIO5


#endif

