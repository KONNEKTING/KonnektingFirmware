/*Multi Interface header file
Author: E.Burkowski / e.burkowski@konnekting.de, GPL Licensed


Multi Interface has 6 SERCOMs, but each Sercom can be used only once!
this 3 Sercoms are reserved for system and can't be changed!
Sercom3: I�C EEPROM / external I�C header
Sercom4: SPI-Flash
Sercom5: Serial for KXN-Tranceiver

Sercom0 is reserved for "Serial1" (D0,D1) if you use Serial1.begin(..), but this pins are only internaly available

Sercom1,2 are free for use.
*/

/*if you want to use SPI please uncomments following line
#define useSPI1 //uses sercom1
#define useSPI2 //uses sercom2
#define useSerial2 //uses sercom2
#define useSerial3 //uses sercom1
*/

#define useExternalEEPROM // comment if internal Flash EEPROM is used

//don't change this lines!
#include <Wire.h>
#include <Arduino.h>
#include "wiring_private.h" // for pinPeripheral() function


#ifdef useSPI1
#define PERIPH_SPI1   sercom1
#define PIN_SPI1_MOSI 11
#define PIN_SPI1_MISO 12
#define PIN_SPI1_SCK  13
SPIClass SPI1 (&PERIPH_SPI1, PIN_SPI1_MISO, PIN_SPI1_SCK, PIN_SPI1_MOSI, SPI_PAD_0_SCK_1, SERCOM_RX_PAD_3);
//call this function in setup
void SPI1_init(){
	SPI1.begin()
	pinPeripheral(PIN_SPI1_MOSI, PIO_SERCOM);
	pinPeripheral(PIN_SPI1_MISO, PIO_SERCOM);
	pinPeripheral(PIN_SPI1_SCK, PIO_SERCOM);
}
#endif

#ifdef useSPI2
#define PERIPH_SPI2   sercom2
#define PIN_SPI2_MOSI 4
#define PIN_SPI2_MISO 3
#define PIN_SPI2_SCK  5  //alternatively pin 0, but you have to use SPI_PAD_0_SCK_3 intead of SPI_PAD_0_SCK_3 and it will break UART on D0/D1
SPIClass SPI1 (&PERIPH_SPI2, PIN_SPI2_MISO, PIN_SPI2_SCK, PIN_SPI2_MOSI, SPI_PAD_0_SCK_3, SERCOM_RX_PAD_1);
//call this function in setup
void SPI2_init(){
	SPI2.begin();
	pinPeripheral(PIN_SPI1_MOSI, PIO_SERCOM_ALT);
	pinPeripheral(PIN_SPI1_MISO, PIO_SERCOM_ALT);
	pinPeripheral(PIN_SPI1_SCK, PIO_SERCOM);
}
#endif

#ifdef useSerial2
#define PERIPH_SERIAL2 sercom2
#define SERIAL2_RX     5
#define SERIAL2_TX     2
Uart Serial2 (&PERIPH_SERIAL2, SERIAL2_RX, SERIAL2_TX, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM2_Handler()
{
  Serial2.IrqHandler();
}
//call this function in setup() after Serial2.begin(..)
void Serial2_init(){
	pinPeripheral(SERIAL2_RX, PIO_SERCOM);
	pinPeripheral(SERIAL2_TX, PIO_SERCOM);
}
#endif

#ifdef useSerial3
#define PERIPH_SERIAL3 sercom1
#define SERIAL3_RX     13
#define SERIAL3_TX     11
Uart Serial3 (&PERIPH_SERIAL3, SERIAL3_RX, SERIAL3_TX, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM1_Handler()
{
  Serial3.IrqHandler();
}
//call this function in setup() after Serial3.begin(..)
void Serial3_init(){
	pinPeripheral(SERIAL3_RX, PIO_SERCOM);
	pinPeripheral(SERIAL3_TX, PIO_SERCOM);
}
#endif

#define PROG_LED_PIN A5
#define PROG_BUTTON_PIN 38
#define KNX_SERIAL Serial
#define FLASH_CS 9 

#define OWD3 3
#define OWD3PullUp A3
#define OWD4 4
#define OWD4PullUp A4

#ifdef useExternalEEPROM
// MI uses 24AA256 I2C EEPROM
int readMemory(int index) {
    byte data = 0xFF;
    if(index >= 0 && index < 32768){
        Wire.beginTransmission(0x50);
        Wire.write((int) (index >> 8));
        Wire.write((int) (index & 0xFF));
        Wire.endTransmission();
        Wire.requestFrom(0x50, 1);
        if (Wire.available()) {
            data = Wire.read();
        }
    }
    return data;
}

void writeMemory(int index, int val) {
    if(index >= 0 && index < 32768 && val >= 0 && val < 256){
        Wire.beginTransmission(0x50);
        Wire.write((int) (index >> 8));
        Wire.write((int) (index & 0xFF));
        Wire.write(val);
        Wire.endTransmission();
        delay(5); //is it needed?!
    }
}

void updateMemory(int index, int val) {
    if (readMemory(index) != val) {
        writeMemory(index, val);
    }
}

void commitMemory() {
    // EEPROM needs no commit, so this function is intentionally left blank 
}
#else
#include "ZeroEEPROM.h" // download from connecting GIT and don't forget Emulated EEPROM lib referenced there

//  ZeroEEPROM.init(); ...call this function in setup() after Konnekting.setMemory...() functions

int readMemory(int index) {
	return ZeroEEPROM.read(index);
}
void writeMemory(int index, int val) {
	ZeroEEPROM.write(index, val);
}
void updateMemory(int index, int val) {
    if (readMemory(index) != val) {
        writeMemory(index, val);
    }
}
void commitMemory() {
	ZeroEEPROM.commit();
}
#endif
