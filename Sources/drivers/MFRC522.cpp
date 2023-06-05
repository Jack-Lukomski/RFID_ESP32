/*
* MFRC522.cpp - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
* NOTE: Please also check the comments in MFRC522.h - they provide useful hints and background information.
* Released into the public domain.
*/

#include "MFRC522.hpp"

/////////////////////////////////////////////////////////////////////////////////////
// Functions for setting up the Arduino
/////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
MFRC522::MFRC522() {};

/**
 * Constructor.
 * Prepares the output pins.
 */
MFRC522::MFRC522(spi_device_handle_t spiHandle, uint8_t rstPin) 
{
	_spiHandle = spiHandle;
	_rstPin = rstPin;
}

/////////////////////////////////////////////////////////////////////////////////////
// Basic interface functions for communicating with the MFRC522
/////////////////////////////////////////////////////////////////////////////////////

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */
// void MFRC522::PCD_WriteRegister(PCD_Register reg, uint8_t value) 
// {
//     esp_err_t retVal;
//     uint8_t txData[2] = {(reg & 0x7F), value};

//     spi_transaction_t currTrans = {
//         .length = 16,
//         .tx_buffer = txData,
//         .flags = 0,
//     };

//     retVal = spi_device_transmit(_spiHandle, &currTrans);
// } // End PCD_WriteRegister()

// /**
//  * Writes a number of bytes to the specified register in the MFRC522 chip.
//  * The interface is described in the datasheet section 8.1.2.
//  */
// void MFRC522::PCD_WriteRegister(	PCD_Register reg,	///< The register to write to. One of the PCD_Register enums.
// 									byte count,			///< The number of bytes to write to the register
// 									byte *values		///< The values to write. Byte array.
// 								) {
// 	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
// 	digitalWrite(_chipSelectPin, LOW);		// Select slave
// 	SPI.transfer(reg);						// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
// 	for (byte index = 0; index < count; index++) {
// 		SPI.transfer(values[index]);
// 	}
// 	digitalWrite(_chipSelectPin, HIGH);		// Release slave again
// 	SPI.endTransaction(); // Stop using the SPI bus
// } // End PCD_WriteRegister()

// /**
//  * Reads a byte from the specified register in the MFRC522 chip.
//  * The interface is described in the datasheet section 8.1.2.
//  */
// byte MFRC522::PCD_ReadRegister(	PCD_Register reg	///< The register to read from. One of the PCD_Register enums.
// 								) {
// 	byte value;
// 	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
// 	digitalWrite(_chipSelectPin, LOW);			// Select slave
// 	SPI.transfer(0x80 | reg);					// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
// 	value = SPI.transfer(0);					// Read the value back. Send 0 to stop reading.
// 	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
// 	SPI.endTransaction(); // Stop using the SPI bus
// 	return value;
// } // End PCD_ReadRegister()

// /**
//  * Reads a number of bytes from the specified register in the MFRC522 chip.
//  * The interface is described in the datasheet section 8.1.2.
//  */
// void MFRC522::PCD_ReadRegister(	PCD_Register reg,	///< The register to read from. One of the PCD_Register enums.
// 								byte count,			///< The number of bytes to read
// 								byte *values,		///< Byte array to store the values in.
// 								byte rxAlign		///< Only bit positions rxAlign..7 in values[0] are updated.
// 								) {
// 	if (count == 0) {
// 		return;
// 	}
// 	//Serial.print(F("Reading ")); 	Serial.print(count); Serial.println(F(" bytes from register."));
// 	byte address = 0x80 | reg;				// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
// 	byte index = 0;							// Index in values array.
// 	SPI.beginTransaction(SPISettings(MFRC522_SPICLOCK, MSBFIRST, SPI_MODE0));	// Set the settings to work with SPI bus
// 	digitalWrite(_chipSelectPin, LOW);		// Select slave
// 	count--;								// One read is performed outside of the loop
// 	SPI.transfer(address);					// Tell MFRC522 which address we want to read
// 	if (rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
// 		// Create bit mask for bit positions rxAlign..7
// 		byte mask = (0xFF << rxAlign) & 0xFF;
// 		// Read value and tell that we want to read the same address again.
// 		byte value = SPI.transfer(address);
// 		// Apply mask to both current value of values[0] and the new data in value.
// 		values[0] = (values[0] & ~mask) | (value & mask);
// 		index++;
// 	}
// 	while (index < count) {
// 		values[index] = SPI.transfer(address);	// Read value and tell that we want to read the same address again.
// 		index++;
// 	}
// 	values[index] = SPI.transfer(0);			// Read the final byte. Send 0 to stop reading.
// 	digitalWrite(_chipSelectPin, HIGH);			// Release slave again
// 	SPI.endTransaction(); // Stop using the SPI bus
// } // End PCD_ReadRegister()

// /**
//  * Sets the bits given in mask in register reg.
//  */

