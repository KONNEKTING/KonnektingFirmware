/* Universal Flush Interface Rev1.x Headerfile
Author: E.Burkowski / e.burkowski@konnekting.de, GPL Licensed
E=External terminal block
I=Internal terminal block
       _______USB__________
    E1/                    \
     1|  I1            I4  |E4
     2|  1 2 3          1  |1
     3|                 2  |2
     4|                 3  |3
     5|                    |
     6|                    |KNX+
     7|                    |KNX-
     8| I2      I3/E3      |
     9| 1 2 3   1 2 3 4    |
    10|                    |
      \____________________/
    E2    1 2 3 4 5 6 7
E1: 
E1_1:	D14		MISO
E1_2:	D16		MOSI
E1_3:	D15		SCK
E1_4:	VCC
E1_5:	D4		OWLeft	(1-Wire Left)
E1_6:	GND
E1_7:	D6
E1_8:	D22		A4
E1_9:	GND
E1_10:D21		A3
E2:
E2_1:	D9		BIN2
E2_2:	GND		
E2_3:	D8		BIN1
I3/E3:
E3_1:	D3		SLC		(Interrupt)
E3_2:	D2		SDA		(Interrupt)
E3_3:	VCC
E3_4:	GND
E4:
E4_1:	VCC
E4_2:	D10		OWRight	(1-Wire right)
E4_3:	GND
I1:
I1_1:	D15		SCK
I1_2:	D16		MOSI
I1_3:	D14		MISO
I2:
I2_1:	VCC
I2_2:	D23		A5
I2_3:	GND
I4:
I4_1:	D20		A2
I4_2:	D19		A1
I4_3:	D18		A0
Enable 1-Wire left  PullUp: OWLeftPullUp
Enable 1-Wire right PullUp: OWRightPullUp
OneWire
       _______USB__________
    E1/                    \
     1|                    |E4
     2|                    |+3.3V
     3|                    |OWRight
 +3.3V|                    |GND
OWLeft|                    |
   GND|                    |KNX+
     7|                    |KNX-
     8|                    |
     9|                    |
    10|                    |
      \____________________/
      E2  1 2 3 4 5 6 7
	  
       _______USB__________
    E1/                    \
 BIN_I|                    |E4
 BIN_H|                    |X
 BIN_G|                    |BIN_J
     X|                    |GND
 BIN_F|                    |
   GND|                    |KNX+
 BIN_E|                    |KNX-
 BIN_D|                    |
   GND|                    |
 BIN_C|                    |
      \____________________/
          B G B
          I N I
          N D N
          _   _ 
          B   A 
*/


#define PROG_LED_PIN 13
#define PROG_BUTTON_PIN 7
#define KNX_SERIAL Serial1 

#define BIN_A 8
#define BIN_B 9
#define BIN_C 21
#define BIN_D 22
#define BIN_E 6
#define BIN_F 4
#define BIN_G 15
#define BIN_H 16
#define BIN_I 14
#define BIN_J 10

#define OWLeft 4
#define OWLeftPullUp 11
#define OWRight 10
#define OWRightPullUp 5

#define E1_1 14
#define E1_2 16
#define E1_3 15
#define E1_5 4
#define E1_5A A6

#define E1_7 6
#define E1_8 22
#define E1_8A A4

#define E1_10 21
#define E1_10A A3

#define E2_1 9
#define E2_1A A9
#define E2_3 8
#define E2_3A A8

#define E3_1 3
#define E3_2 2

#define E4_2 10
#define E4_2A A10

#define I1_1 15
#define I1_2 16
#define I1_3 14

#define I2_1 22
#define I2_1A A4

#define I4_1 20
#define I4_1A A2
#define I4_2 19
#define I4_2A A1
#define I4_3 18
#define I4_3A A0
