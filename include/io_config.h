#ifndef _IO_CONFIG_H__
#define _IO_CONFIG_H__

#define EASYQ_RECV_BUFFER_SIZE  4096
#define EASYQ_SEND_BUFFER_SIZE  512 
#define EASYQ_HOSTNAME			"192.168.8.44"
#define EASYQ_PORT				1085

#define DEVICE_NAME				"st"
#define EASYQ_LOGIN				DEVICE_NAME
#define MOTOR_QUEUE				DEVICE_NAME":motor"
#define FOTA_QUEUE				DEVICE_NAME":fota"
#define FOTA_STATUS_QUEUE		DEVICE_NAME":fota:status"

/* GPIO */

// Direction 
#define DIR_MUX			PERIPHS_IO_MUX_GPIO5_U
#define DIR_NUM			5
#define DIR_FUNC		FUNC_GPIO5

// Step
#define STEP_MUX		PERIPHS_IO_MUX_GPIO4_U
#define STEP_NUM		4
#define STEP_FUNC		FUNC_GPIO4

// EN
#define EN_MUX		PERIPHS_IO_MUX_GPIO0_U
#define EN_NUM		0
#define EN_FUNC		FUNC_GPIO0


#endif

