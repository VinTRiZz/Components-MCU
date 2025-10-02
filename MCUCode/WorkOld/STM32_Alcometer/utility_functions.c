#include "utility_functions.h"

// Calculation of CRC for 8-bit message (char)
// Works only if no spaces in message
int8_t crc8(uint8_t *data, uint32_t length)
{
  int crc = 0;
  uint8_t polynomial = 0x07;

  for (uint32_t i = 0; i < length; i++)
  {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++)
    {
      if (crc & 0x80)
      {
        crc = (crc << 1) ^ polynomial;
      }
      else
      {
        crc <<= 1;
      }
    }
  }

  return crc;
}

// Check of CRC in 8-bit message (char)
// Works only if no spaces in message
int8_t verify_crc8(unsigned char *data, int length)
{
    int crc = crc8(data, length - 1);

    return crc == data[length - 1];
}

void float_to_char(float num, char *str, int precision)
{
    int whole_part = (int) num;
    float fractional_part = num - whole_part;
    int pos = 0;

    // Convert the whole part of the float to a string
    sprintf(str, "%d", whole_part);
    pos = strlen(str);

    if (precision > 0)
    {
        str[pos++] = '.';

        int digital_part = 0;

        if (fractional_part < 0)
            fractional_part *= -1.0;

        for (int i = 0; i < precision; i++)
        {
            fractional_part *= 10;

            digital_part = (int)(fractional_part + 0.05) % 10;

            fractional_part -= digital_part;

            str[pos++] = digital_part + '0';
        }
    }
    // Clear string from garbage
    for (int i = pos; str[i]; i++)
    	str[i] = '\0';
}


int8_t parse_json_key(char * key, char * output_buf, char * input_json_string)
{
    if (input_json_string[0] != '{' || !input_json_string[0])
    {
        return -1;
    }

    int8_t is_command = 0;
    int command_size = 0;

    for (int i = 1; input_json_string[i] && (input_json_string[i] != '}'); i++)
    {
        if (input_json_string[i] == '\"')
        {
            i++;
            if (!is_command)
            {
                if (
                    checkStrEqual_fromTo(input_json_string, key, i, i+strlen(key) )
                    )
                {
                    i+= strlen(key);
                    is_command = 1;
                }
            }
            else
            {
                // Get size of command to read
                for (int pos = 0; input_json_string[i+pos] && (input_json_string[i+pos] != '\"'); pos++ )
                {
                    command_size = pos + 1;
                }

                // Set buffer to command text
                for (int pos = 0; (pos < command_size) && (input_json_string[i+pos]); pos++ )
                {
                    output_buf[pos] = input_json_string[i+pos];
                }
                output_buf[command_size] = '\0';

                i+=command_size;

                is_command = 0;

                return 1;
                // Backup
                // break;
            }
        }
    }
    return 0;
}



int8_t checkStrEqual_fromTo(char * main_arr, char * internal_arr, int from, int to)
{
    int8_t result = 1;

    for (int check_pos = from; main_arr[check_pos] && internal_arr[check_pos - from] && result; check_pos++)
    {
        result = main_arr[check_pos] == internal_arr[check_pos - from];
    }
    return result;
}
