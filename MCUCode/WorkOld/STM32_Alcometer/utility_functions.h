#pragma once

#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#include <stdint.h>

/**
 * @brief CRC calculation function for 8-bit values
 *
 * @param data Pointer to data string
 * @param length Length of data string
 *
 * @retval CRC value calculted
 */
int8_t crc8(uint8_t *data, uint32_t length);


/**
 * @brief Check of CRC in 8-bit message
 *
 * @param data Pointer to data string
 * @param length Length of data string
 *
 * @retval 1 if valid, 0 if invalid
 */
int8_t verify_crc8(unsigned char *data, int length);

/**
 * @brief Float value to char string converter
 *
 * @param num Number to convert
 * @param str Pointer to char array for writing data in
 * @param precision Precision of converting
 */
void float_to_char(float num, char *str, int precision);

/**
 * @brief Searches for key in JSON format string
 *
 * @param key Key value to find
 * @param output_buf Buffer for value with key @ref key found
 * @param input_json_string String in JSON format to parse
 *
 * @retval 1 if key found, -1 if error, 0 if no such key found
 */
int8_t parse_json_key(char * key, char * output_buf, char * input_json_string);

/**
 * @brief Check if strings are equal
 *
 * @param main_arr Array to ckeck in
 * @param internal_arr Array to check with
 * @param from Position in @ref main_arr to start from
 * @param to Position in @ref main_arr to end at
 */
int8_t checkStrEqual_fromTo(char * main_arr, char * internal_arr, int from, int to);

#endif // UTILITY_FUNCTIONS_H
