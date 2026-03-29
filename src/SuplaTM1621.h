

 // edition shimano73 dariuszjszymanski@gmail.com
 
/*
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SUPLA_TM1621_h
#define SUPLA_TM1621_h
#include <Arduino.h>

//define TH1621 for Sonoff Pow Elite

#define GPIO_TM1621_DAT 14
#define GPIO_TM1621_RD  26
#define GPIO_TM1621_CS  25
#define GPIO_TM1621_WR  27

#define XDRV_87              87

#define TM1621_ROTATE        5     // Seconds display rotation speed
#define TM1621_MAX_VALUES    8     // Default 8 x two different lines

#define TM1621_PULSE_WIDTH   10    // microseconds (Sonoff = 100)

#define TM1621_SYS_EN        0x01  // 0b00000001
#define TM1621_LCD_ON        0x03  // 0b00000011
#define TM1621_TIMER_DIS     0x04  // 0b00000100
#define TM1621_WDT_DIS       0x05  // 0b00000101
#define TM1621_TONE_OFF      0x08  // 0b00001000
#define TM1621_BIAS          0x29  // 0b00101001 = LCD 1/3 bias 4 commons option
#define TM1621_IRQ_DIS       0x80  // 0b100x0xxx

enum Tm1621Device { TM1621_USER, TM1621_POWR316D, TM1621_THR316D };


enum Tm1621Units  { TM1621_NONE, TM1621_TEMPERATURE, TM1621_HUMIDITY, TM1621_VOLTAGE_CURRENT, TM1621_ENERGY_POWER };

const uint8_t tm1621_commands[] = { TM1621_SYS_EN, TM1621_LCD_ON, TM1621_BIAS, TM1621_TIMER_DIS, TM1621_WDT_DIS, TM1621_TONE_OFF, TM1621_IRQ_DIS };

struct _Tm1621 {
  uint8_t buffer[8];
  char row[2][12];
  uint8_t pin_da;
  uint8_t pin_cs;
  uint8_t pin_rd;
  uint8_t pin_wr;
  uint8_t state;
  uint8_t device;
  uint8_t display_rotate;
  uint8_t temp_sensors;
  uint8_t temp_sensors_rotate;
  bool celsius;
  bool fahrenheit;
  bool humidity;
  bool voltage;
  bool kwh;
  bool present;
} ;

extern _Tm1621 Tm1621;

enum DisplayUnit {
  UNIT_NONE,
  UNIT_VOLT,
  UNIT_KWH
};
extern bool dot1 ;
extern bool dot2 ;

int Pin(uint32_t gpio);
int GetCommandCode(char* destination, size_t destination_size, const char* needle, const char* haystack);
bool formatRow(const char* in, char* out);
void DisplayText(const char* row1, const char* row2, DisplayUnit unit);
void TM1621PreInit(void);
void TM1621Init(void);
void TM1621SendRows();
void TM1621SendCommon(uint8_t common);
void TM1621SendAddress(uint16_t address);
void TM1621StopSequence(void);
void TM1621SendCmnd(uint16_t command);
void TM1621Show(void);

#endif //TM1621_h