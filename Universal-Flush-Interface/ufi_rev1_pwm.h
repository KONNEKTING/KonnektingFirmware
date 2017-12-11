/* Universal Flush Interface Rev1.x PWM Headerfile
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
E1_5:	D11		OWLeft	(1-Wire Left)
E1_6:	GND
E1_7:	D10
E1_8:	D9		A9
E1_9:	GND
E1_10:	D5
E2:
E2_1:	D6		BIN1
E2_2:	GND		
E2_3:	D13		BIN2
I3/E3:
E3_1:	D3		SLC		(Interrupt)
E3_2:	D2		SDA		(Interrupt)
E3_3:	VCC
E3_4:	GND
E4:
E4_1:	VCC
E4_2:	D18	A0	OWRight	(1-Wire right)
E4_3:	GND
I1:
I1_1:	D15		SCK
I1_2:	D16		MOSI
I1_3:	D14		MISO
I2:
I2_1:	VCC
I2_2:	D4		A6
I2_3:	GND
I4:
I4_1:	D22		A4
I4_2:	D21		A3
I4_3:	D20		A2

Enable 1-Wire left  PullUp: OWLeftPullUp
Enable 1-Wire right PullUp: OWRightPullUp

OneWire:
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
       E2    1 2 3 4 5 6 7
          
Binary:
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

PWM:
       _______USB__________
    E1/                    \
     X|                    |E4
     X|                    |X
     X|                    |X
     X|                    |GND
 PWM_1|                    |
   GND|                    |KNX+
 PWM_2|                    |KNX-
 PWM_3|                    |
   GND|                    |
 PWM_4|                    |
      \____________________/
          P G P P X X X
          W N W W
          M D M M
          _   _ _
          5   6 7 
               
PWM_1 11  OCR1C Timer1 16bit
PWM_2 10  OCR1B Timer1 16bit
PWM_3 9   OCR1A Timer1 16bit
PWM_4 5   OCR3A Timer3 16bit
PWM_5 6   OCR4D Timer4 10bit
PWM_6 13  OCR4A Timer4 10bit
PWM_7 3   OCR0B Timer0 8bit
*/


#define PROG_LED_PIN 8
#define PROG_BUTTON_PIN 7
#define KNX_SERIAL Serial1 

#define BIN_A 13
#define BIN_B 6
#define BIN_C 5
#define BIN_D 9
#define BIN_E 10
#define BIN_F 11
#define BIN_G 15
#define BIN_H 16
#define BIN_I 14
#define BIN_J 18

#define PWM_1 11
#define PWM_2 10
#define PWM_3 9
#define PWM_4 5
#define PWM_5 6
#define PWM_6 13
#define PWM_7 3

#define OWLeft 11
#define OWLeftPullUp 23
#define OWRight 18
#define OWRightPullUp 19

#define E1_1 14
#define E1_2 16
#define E1_3 15
#define E1_5 11

#define E1_7 10
#define E1_8 9
#define E1_8A A9
#define E1_9 5

#define E2_1 6
#define E2_1A A7
#define E2_3 13

#define E3_1 3
#define E3_2 2

#define E4_2 18
#define E4_2A A0

#define I1_1 15
#define I1_2 16
#define I1_3 14

#define I2_1 4
#define I2_1A A6

#define I4_1 22
#define I4_1A A4
#define I4_2 21
#define I4_2A A3
#define I4_3 20
#define I4_3A A2
