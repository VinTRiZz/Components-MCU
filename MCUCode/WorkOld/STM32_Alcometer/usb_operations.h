#pragma once

#ifndef USB_OPERATIONS_H
#define USB_OPERATIONS_H

#include "usb_device.h"

/** @brief Clears USB buffer
 */
void usb_clearBuffer();

/** @brief Sends data by USB wirh CRC byte in end of message
 * @param msg Message to send
 * @retval 0 if error, 1 if succeed
 */
int8_t usb_send(char * msg);

/** @brief Sends data by USB wirhout CRC byte in end of message
 * @param msg Message to send
 * @retval 0 if error, 1 if succeed
 */
int8_t usb_send_no_crc(char* msg);

/** @brief Checks if there's data in buffer to read
 * @param msg_buf Buffer to write msg data
 * @retval 0 if error, 1 if succeed
 */
int8_t usb_read(char * msg_buf);

/** @brief Checks if there's data in buffer to read, ignoring CRC byte
 * @param msg_buf Buffer to write msg data
 * @retval 0 if error, 1 if succeed
 */
int8_t usb_read_no_crc(char * msg_buf);

/** @brief Sends data in JSON format
 * @param msg Message to send
 * @retval 0 if error, 1 if succeed
 * @note Example: {"response":"0,0","CRC":"126"}
 */
int send_json(char * msg);

#endif // USB_OPERATIONS_H
