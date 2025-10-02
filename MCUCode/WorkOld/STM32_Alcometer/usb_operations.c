#include "usb_operations.h"

extern uint8_t buffer[64]; ///< USB buffer string


void usb_clearBuffer()
{
    for (int i = 0; buffer[i]; i++)
        buffer[i] = '\0';
}


int8_t usb_send(char * msg)
{
	uint8_t msg_crc_cpy[ strlen(msg) + 1 ];

	memcpy(msg_crc_cpy, msg, strlen(msg));

    msg_crc_cpy[ strlen( (char*)msg_crc_cpy) ] = crc8( msg, strlen(msg) );

#ifdef CMSIS_OS_H_
    taskENTER_CRITICAL(); // Start atomic operation
#endif // CMSIS_OS_H_

    int8_t result = CDC_Transmit_FS(msg_crc_cpy, strlen(msg_crc_cpy));

#ifdef CMSIS_OS_H_
    taskEXIT_CRITICAL(); // End atomic operation
#endif // CMSIS_OS_H_

    HAL_Delay( 1 + (int)(strlen(msg) / 8) );

	return result == USBD_OK;
}

int8_t usb_send_no_crc(char* msg)
{
#ifdef CMSIS_OS_H_
	taskENTER_CRITICAL(); // Start atomic operation
#endif // CMSIS_OS_H_

	int8_t result = CDC_Transmit_FS(msg, strlen(msg));

#ifdef CMSIS_OS_H_
	taskEXIT_CRITICAL(); // End atomic operation
#endif // CMSIS_OS_H_

	HAL_Delay( 1 + (int)(strlen(msg) / 8) );

	return result == USBD_OK;
}


int8_t usb_read(char * msg_buf)
{
    if (buffer[1] != '\0')
    {
    	if (verify_crc8(msg_buf, strlen(msg_buf)))
    	{
    		for (int i = 0; (buffer[i+1] != '\0') && (msg_buf[i]); i++)
    		{
    			msg_buf[i] = buffer[i+1];
    		}
    		return 1;
    	}
    }
    return 0; // No message found
}

int8_t usb_read_no_crc(char * msg_buf)
{
	if (buffer[1] != '\0')
	{
		for (int i = 0; (buffer[i] != '\0'); i++)
		{
			msg_buf[i] = buffer[i];
		}
		return 1;
	}
    return 0;
}

int send_json(char * msg)
{
    char crc_byte[4] = {'\0'};

    sprintf(crc_byte, "%d", crc8( (uint8_t*)msg, strlen(msg)));

    if (
		!usb_send_no_crc("{\"response\":\"") ||
		!usb_send_no_crc(msg) ||
		!usb_send_no_crc("\",\"CRC\":\"") ||
		!usb_send_no_crc(crc_byte) ||
		!usb_send_no_crc("\"}\n")
	)
    	return 0;

    return 1;
}
