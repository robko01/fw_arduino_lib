/*
	Copyright (c) [2019] [Orlin Dimitrov]

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "GeneralHelper.h"

uint8_t bit_array_to_uint8(bool arr[], uint8_t count)
{
	static uint8_t ResultL;
	static uint8_t TemporalL;

  ResultL = 0;
  TemporalL = 0;
  
	for (uint8_t index = 0; index < count; index++)
	{
		TemporalL = arr[index];
		ResultL |= TemporalL << (count - index - 1);
	}

	return ResultL;
}


/** @brief Get MAC address.
 *  @return String, Returns the string of MAC address.
 */
String mac2str(const uint8_t * mac) {

	char MACStrL[WL_MAC_ADDR_LENGTH * 3] = { 0 };

	sprintf(MACStrL, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return  String(MACStrL);
}

/** @brief Convert a single hex digit character to its integer value. Based on https://code.google.com/p/avr-netino/
 *  @return String, Returns the string of unified URL string.
 */
unsigned char hex2dec(char c) {

	if (c >= '0' && c <= '9') {
		return((unsigned char)c - '0');
	}
	if (c >= 'a' && c <= 'f') {
		return((unsigned char)c - 'a' + 10);
	}
	if (c >= 'A' && c <= 'F') {
		return((unsigned char)c - 'A' + 10);
	}
	return(0);
}

/** @brief Converts binary array to heximal string.
 *  @param uint8_t * input, Binary input.
 *  @param unsigned int input_size, Binary input size.
 *  @param char * output, Output result.
 *  @return boolean, Returns the true if value is in the range.
 */
void bin_to_strhex(uint8_t *input, unsigned int input_size, uint8_t *output)
{
	for (unsigned int index = 0; index < input_size; index++)
	{
		output[index * 2 + 0] = hex2dec((input[index] >> 4) & 0x0F);
		output[index * 2 + 1] = hex2dec((input[index]) & 0x0F);
	}
}

/** @brief Check the Values is between: [0 - 255].
 *  @param value String, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(String value) {
	return check_octet_range(value.toInt());
}

/** @brief Check the Values is between: [0 - 255].
 *  @param value int, Value of the octet.
 *  @return boolean, Returns the true if value is in the range.
 */
boolean check_octet_range(int value) {
	return (value > 0 && value < 256);
}

/** @brief Convert size to nice text type.
 *  @param bytes, size_t Size of the file.
 *  @return String, Nice formated size text.
 */
String formatBytes(size_t bytes) {
	if (bytes < 1024UL) {
		return String(bytes) + "B";
	}
	else if (bytes < (1048576UL)) {
		return String(bytes / 1024UL) + "KB";
	}
	else if (bytes < (1073741824UL)) {
		return String(bytes / 1048576UL) + "MB";
	}
	else {
		return String(bytes / 1073741824UL) + "GB";
	}
}

/** @brief Convert many minutes to ending minutes.
 *  @param int minutes, Minutest.
 *  @return int, Ending minutes.
 */
int to_minutes(int minutes)
{
	return minutes % 60;
}

/** @brief Convert many minutes to hours.
 *  @param int minutes, Minutest.
 *  @return int, Hours.
 */
int to_hours(int minutes)
{
	return minutes / 60;
}

#ifdef BATT_MONITOR

/** @brief Read battery voltage.
 *  @return float voltage.
 */
float battery_voltage(int pin)
{
	static float batt_voltage = 0.0;

	float batt_voltage = (float)(analogRead(pin)/4095)*2*3.3*1.1;

	return batt_voltage;
}

#endif

/**
 * @brief Generate cookie string.
 * 
 * @return String cookie string.
 */
String generate_random_str(uint8_t size)
{
  static uint8_t chars[30];

  for (uint8_t index = 0; index < size; index++)
  {
    chars[index] = (uint8_t)random(48,122);
  }
  chars[size] = (uint8_t)0;

  return String(*chars);
}

