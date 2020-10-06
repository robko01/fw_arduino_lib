// EEP24C32.h

#ifndef _EEP24C32_h
#define _EEP24C32_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

/**************************************************************************//**
 * \class EEP24C32Class
 *
 * \brief EEPROM 24C32 / 24C64 memory driver.
 *
 * This driver is designed for 24C32 and 24C64 EEPROM memories.
 ******************************************************************************/
class EEP24C32Class
{
public:

	/******************************************************************//**
	 * \fn EEP24C32Class(byte deviceAddress)
	 *
	 * \brief Constructor.
	 *
	 * \param   deviceAddress   EEPROM address on TWI bus.
	 **********************************************************************/
	EEP24C32Class
	(
		byte deviceAddress
	);

	/******************************************************************//**
	 * \fn void initialize()
	 *
	 * \brief Initialize library abnd TWI bus.
	 *
	 * If several devices are connected to TWI bus, this method mustn't be
	 * called. TWI bus must be initialized out of this library using
	 * Wire.begin() method.
	 **********************************************************************/
	void
		initialize();

	/******************************************************************//**
	 * \fn void writeByte(
	 * word address,
	 * byte data)
	 *
	 * \brief Write a byte in EEPROM memory.
	 *
	 * \remarks A delay of 10 ms is required after write cycle.
	 *
	 * \param   address Address.
	 * \param   data    Byte to write.
	 **********************************************************************/
	void
		writeByte
		(
			word    address,
			byte    data
		);

	/******************************************************************//**
	 * \fn void writeBytes(
	 * word     address,
	 * word     length,
	 * byte*    p_data)
	 *
	 * \brief Write bytes in EEPROM memory.
	 *
	 * \param       address Start address.
	 * \param       length  Number of bytes to write.
	 * \param[in]   p_data  Bytes to write.
	 **********************************************************************/
	void
		writeBytes
		(
			word    address,
			word    length,
			byte* p_data
		);

	/******************************************************************//**
	 * \fn byte readByte(word address)
	 *
	 * \brief Read a byte in EEPROM memory.
	 *
	 * \param   address Address.
	 *
	 * \return Read byte.
	 **********************************************************************/
	byte
		readByte
		(
			word    address
		);

	/******************************************************************//**
	 * \fn void readBytes(
	 * word     address,
	 * word     length,
	 * byte*    p_data)
	 *
	 * \brief Read bytes in EEPROM memory.
	 *
	 * \param       address Start address.
	 * \param       length  Number of bytes to read.
	 * \patam[in]   p_data  Byte array to fill with read bytes.
	 **********************************************************************/
	void
		readBytes
		(
			word    address,
			word    length,
			byte* p_buffer
		);

private:

	byte m_deviceAddress;

	/******************************************************************//**
	 * \fn void writePage(
	 * word     address,
	 * byte     length,
	 * byte*    p_data)
	 *
	 * \brief Write page in EEPROM memory.
	 *
	 * \param       address Start address.
	 * \param       length  Number of bytes (64 bytes max).
	 * \param[in]   p_data  Data.
	 **********************************************************************/
	void
		writePage
		(
			word    address,
			byte    length,
			byte* p_data
		);

	/******************************************************************//**
	 * \fn void writeBuffer(
	 * word     address,
	 * byte     length,
	 * byte*    p_data)
	 *
	 * \brief Write bytes into memory.
	 *
	 * \param       address Start address.
	 * \param       length  Number of bytes (30 bytes max).
	 * \param[in]   p_date  Data.
	 **********************************************************************/
	void
		writeBuffer
		(
			word    address,
			byte    length,
			byte* p_data
		);

	/******************************************************************//**
	 * \fn void readBuffer(
	 * word     address,
	 * byte     length,
	 * byte*    p_data)
	 *
	 * \brief Read bytes in memory.
	 *
	 * \param       address Start address.
	 * \param       length  Number of bytes to read (32 bytes max).
	 * \param[in]   p_data  Buffer to fill with read bytes.
	 **********************************************************************/
	void
		readBuffer
		(
			word    address,
			byte    length,
			byte* p_data
		);
};

//extern EEP24C32Class EEP24C32;

#endif

