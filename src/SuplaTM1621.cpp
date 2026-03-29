
/*
 edition shimano73 dariuszjszymanski@gmail.com


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
#include <Arduino.h>
#include "SuplaTM1621.h"

_Tm1621 Tm1621;


struct CharMap {
  char c;
  uint8_t seg_row1;
  uint8_t seg_row2;
};

const CharMap charMapFull[] = {
  // cyfry
  {'0', 0x5F, 0xF5},
  {'1', 0x50, 0x05},
  {'2', 0x3D, 0xB6},
  {'3', 0x79, 0x97},
  {'4', 0x72, 0x47},
  {'5', 0x6B, 0xD3},
  {'6', 0x6F, 0xF3},
  {'7', 0x51, 0x85},
  {'8', 0x7F, 0xF7},
  {'9', 0x7B, 0xD7},

  // znaki
  {'C', 0x0F, 0x05},
  {'I', 0x50, 0x05},
  {'U', 0x5E, 0x75},
  {'A', 0x77, 0x98},
  {'H', 0x76, 0x4B},
  {'S', 0x6B, 0xD3},
  {'L', 0x0E, 0x70},
  {'P', 0x37, 0xD9},
  {'E', 0x2F, 0xF2},
  {'G', 0x6F, 0xF3},

  // specjalne
  {'-', 0x20, 0x02},
  {' ', 0x00, 0x00}
};


bool dot1 ;
bool dot2 ;

uint8_t getCharCode(char c, uint8_t row) {

  for (uint32_t i = 0; i < sizeof(charMapFull)/sizeof(CharMap); i++) {
    if (charMapFull[i].c == c) {
      return (row == 0) ? charMapFull[i].seg_row1
                        : charMapFull[i].seg_row2;
    }
  }

  return 0x00; // brak znaku
}

bool isNumber(const char* str) {
    while (*str) {
        if (!isdigit(*str) && *str != '.' && *str != '-') return false;
        str++;
    }
    return true;
}



bool formatRow(const char* in, char* out) {

    int len = strlen(in);
    bool hasDot = false;

    bool isNum = true;

    for (int i = 0; i < len; i++) {
        if (!isdigit(in[i]) && in[i] != '.') {
            isNum = false;
        }
        if (in[i] == '.') {
            hasDot = true;
        }
    }

    char temp[8];
    int ti = 0;

    // usuwamy kropkę
    for (int i = 0; i < len; i++) {
        if (in[i] != '.') {
            temp[ti++] = in[i];
        }
    }
    temp[ti] = '\0';

    int cleanLen = strlen(temp);

    // 👉 LICZBA
    if (isNum) {

        int offset = 4 - cleanLen;
        if (offset < 0) offset = 0;

        for (int i = 0; i < 4; i++) {
            if (i < offset) out[i] = ' ';
            else out[i] = temp[i - offset];
        }

    } 
    // 👉 TEKST (np. L4, CFG)
    else {

        // 🔴 wyrównanie do PRAWEJ dla tekstu z cyfrą
        int offset = 4 - cleanLen;
        if (offset < 0) offset = 0;

        for (int i = 0; i < 4; i++) {
            if (i < offset) out[i] = ' ';
            else out[i] = temp[i - offset];
        }
    }

    out[4] = '\0';

    return hasDot;
}

void DisplayText(const char* row1, const char* row2, DisplayUnit unit)
{
    char r1[5];
    char r2[5];

    dot1 = formatRow(row1, r1);
    dot2 = formatRow(row2, r2);

    strcpy(Tm1621.row[0], r1);
    strcpy(Tm1621.row[1], r2);

    Tm1621.voltage = false;
    Tm1621.kwh = false;

    switch(unit)
    {
        case UNIT_VOLT:
            Tm1621.voltage = true;
            break;

        case UNIT_KWH:
            Tm1621.kwh = true;
            break;

        default:
            break;
    }

    TM1621SendRows();
}


int Pin(uint32_t gpio) {
  return gpio;
}

int GetCommandCode(char* destination, size_t destination_size, const char* needle, const char* haystack)
{
  // Returns -1 of not found
  // Returns index and command if found
  int result = -1;
  const char* read = haystack;
  char* write = destination;

  while (true) {
    result++;
    size_t size = destination_size -1;
    write = destination;
    char ch = '.';
    while ((ch != '\0') && (ch != '|')) {
      ch = pgm_read_byte(read++);
      if (size && (ch != '|'))  {
        *write++ = ch;
        size--;
      }
    }
    *write = '\0';
    if (!strcasecmp(needle, destination)) {
      break;
    }
    if (0 == ch) {
      result = -1;
      break;
    }
  }
  return result;
}


void TM1621SendRows() {

  uint8_t buffer[8] = { 0 };

  for (uint32_t j = 0; j < 2; j++) {

    int len = strlen(Tm1621.row[j]);
    int src = len - 1;

    // 🔴 budujemy 4 znaki od prawej
    for (int pos = 3; pos >= 0; pos--) {

      char c = ' ';

      if (src >= 0) {
        c = Tm1621.row[j][src--];
      }

      uint32_t bidx = (j == 0) ? pos : (7 - pos);

      buffer[bidx] = getCharCode(c, j);
    }

    // 🔴 obsługa kropki (z formatRow)
    if (dot1)  buffer[2] |= 0x80;   // Row 1
    if (dot2)  buffer[5] |= 0x08;   // Row 2
  }

  // 🔴 symbole
  if (Tm1621.fahrenheit) buffer[1] |= 0x80;
  if (Tm1621.celsius)    buffer[3] |= 0x80;
  if (Tm1621.kwh)        buffer[4] |= 0x08;
  if (Tm1621.humidity)   buffer[6] |= 0x08;
  if (Tm1621.voltage)    buffer[7] |= 0x08;

  // 🔴 wysyłka do TM1621 (Twoja oryginalna – NIE ruszamy)
  TM1621SendAddress(0x10);

  for (uint32_t i = 0; i < 8; i++) {
    TM1621SendCommon(buffer[i]);
  }

  TM1621StopSequence();
}

void TM1621SendCommon(uint8_t common) {
  for (uint32_t i = 0; i < 8; i++) {
    digitalWrite(Tm1621.pin_wr, 0);    // Start write sequence
    if (common & 1) {
      digitalWrite(Tm1621.pin_da, 1);  // Set data
    } else {
      digitalWrite(Tm1621.pin_da, 0);  // Set data
    }
    delayMicroseconds(TM1621_PULSE_WIDTH);
    digitalWrite(Tm1621.pin_wr, 1);    // Read data
    delayMicroseconds(TM1621_PULSE_WIDTH);
    common >>= 1;
  }
}

void TM1621SendAddress(uint16_t address) {
  uint16_t full_address = (address | 0x0140) << 7;  // 0b101aaaaaa0000000
  digitalWrite(Tm1621.pin_cs, 0);      // Start command sequence
  delayMicroseconds(TM1621_PULSE_WIDTH / 2);
  for (uint32_t i = 0; i < 9; i++) {
    digitalWrite(Tm1621.pin_wr, 0);    // Start write sequence
    if (full_address & 0x8000) {
      digitalWrite(Tm1621.pin_da, 1);  // Set data
    } else {
      digitalWrite(Tm1621.pin_da, 0);  // Set data
    }
    delayMicroseconds(TM1621_PULSE_WIDTH);
    digitalWrite(Tm1621.pin_wr, 1);    // Read data
    delayMicroseconds(TM1621_PULSE_WIDTH);
    full_address <<= 1;
  }
}


void TM1621StopSequence(void) {
  digitalWrite(Tm1621.pin_cs, 1);      // Stop command sequence
  delayMicroseconds(TM1621_PULSE_WIDTH / 2);
  digitalWrite(Tm1621.pin_da, 1);      // Reset data
}

void TM1621SendCmnd(uint16_t command) {
  uint16_t full_command = (0x0400 | command) << 5;  // 0b100cccccccc00000
  digitalWrite(Tm1621.pin_cs, 0);      // Start command sequence
  delayMicroseconds(TM1621_PULSE_WIDTH / 2);
  for (uint32_t i = 0; i < 12; i++) {
    digitalWrite(Tm1621.pin_wr, 0);    // Start write sequence
    if (full_command & 0x8000) {
      digitalWrite(Tm1621.pin_da, 1);  // Set data
    } else {
      digitalWrite(Tm1621.pin_da, 0);  // Set data
    }
    delayMicroseconds(TM1621_PULSE_WIDTH);
    digitalWrite(Tm1621.pin_wr, 1);    // Read data
    delayMicroseconds(TM1621_PULSE_WIDTH);
    full_command <<= 1;
  }
  TM1621StopSequence();
}

void TM1621PreInit(void) { 
 // if (!PinUsed(GPIO_TM1621_CS) || !PinUsed(GPIO_TM1621_WR) || !PinUsed(GPIO_TM1621_RD) || !PinUsed(GPIO_TM1621_DAT)) { return; }

  Tm1621.device =  TM1621_POWR316D;
  Tm1621.present = true;
  Tm1621.pin_da = Pin(GPIO_TM1621_DAT);
  Tm1621.pin_cs = Pin(GPIO_TM1621_CS);
  Tm1621.pin_rd = Pin(GPIO_TM1621_RD);
  Tm1621.pin_wr = Pin(GPIO_TM1621_WR);
  pinMode(Tm1621.pin_da, OUTPUT);
  digitalWrite(Tm1621.pin_da, 1);
  pinMode(Tm1621.pin_cs, OUTPUT);
  digitalWrite(Tm1621.pin_cs, 1);
  pinMode(Tm1621.pin_rd, OUTPUT);
  digitalWrite(Tm1621.pin_rd, 1);
  pinMode(Tm1621.pin_wr, OUTPUT);
  digitalWrite(Tm1621.pin_wr, 1);

  //Xdrv87SettingsLoad(0);

  Tm1621.state = 200;

  //AddLog(LOG_LEVEL_INFO, PSTR("DSP: TM1621"));
}

void TM1621Init(void) {
  digitalWrite(Tm1621.pin_cs, 0);
  delayMicroseconds(80);
  digitalWrite(Tm1621.pin_rd, 0);
  delayMicroseconds(15);
  digitalWrite(Tm1621.pin_wr, 0);
  delayMicroseconds(25);
  digitalWrite(Tm1621.pin_da, 0);
  delayMicroseconds(TM1621_PULSE_WIDTH);
  digitalWrite(Tm1621.pin_da, 1);

  for (uint32_t command = 0; command < sizeof(tm1621_commands); command++) {
    TM1621SendCmnd(tm1621_commands[command]);
  }

  TM1621SendAddress(0x00);
  for (uint32_t segment = 0; segment < 16; segment++) {
    TM1621SendCommon(0);
  }
  TM1621StopSequence();

  snprintf_P(Tm1621.row[0], sizeof(Tm1621.row[0]), PSTR("----"));
  snprintf_P(Tm1621.row[1], sizeof(Tm1621.row[1]), PSTR("----"));
  TM1621SendRows();
}


void TM1621Show(void) {
  Tm1621.celsius = false;
  Tm1621.fahrenheit = false;
  Tm1621.humidity = false;
  Tm1621.voltage = false;
  Tm1621.kwh = false;
  snprintf_P(Tm1621.row[0], sizeof(Tm1621.row[0]), PSTR("    "));
  snprintf_P(Tm1621.row[1], sizeof(Tm1621.row[1]), PSTR("    "));

/*
  if ((Xdrv87Settings.line[0][0] > 0) || (Xdrv87Settings.line[1][0] > 0)) {
    float value = TM1621GetValues(Xdrv87Settings.line[0][Tm1621.display_rotate], 1);
    if (!isnan(value)) {
      ext_snprintf_P(Tm1621.row[0], sizeof(Tm1621.row[0]), PSTR("%1_f"), &value);
      if (TM1621_TEMPERATURE == Xdrv87Settings.unit[0][Tm1621.display_rotate]) {
        if (Settings->flag.temperature_conversion) {  // SetOption8 - Switch between Celsius or Fahrenheit
          Tm1621.fahrenheit = true;
        } else {
          Tm1621.celsius = true;
        }
      }
      Tm1621.voltage = (TM1621_VOLTAGE_CURRENT == Xdrv87Settings.unit[0][Tm1621.display_rotate]);
      Tm1621.kwh = (4 == Xdrv87Settings.unit[0][Tm1621.display_rotate]);
    }
    value = TM1621GetValues(Xdrv87Settings.line[1][Tm1621.display_rotate], 0);
    if (!isnan(value)) {
      ext_snprintf_P(Tm1621.row[1], sizeof(Tm1621.row[1]), PSTR("%1_f"), &value);
      Tm1621.humidity = (TM1621_HUMIDITY == Xdrv87Settings.unit[1][Tm1621.display_rotate]);
      Tm1621.voltage = (TM1621_VOLTAGE_CURRENT == Xdrv87Settings.unit[1][Tm1621.display_rotate]);
      Tm1621.kwh = (TM1621_ENERGY_POWER == Xdrv87Settings.unit[1][Tm1621.display_rotate]);
    }
    uint32_t max = 0;
    while ((max < TM1621_MAX_VALUES) && ((Xdrv87Settings.line[0][max] > 0) || (Xdrv87Settings.line[1][max] > 0))) { max++; }
    Tm1621.display_rotate++;
    if (Tm1621.display_rotate >= max) {
      Tm1621.display_rotate = 0;
    }
    TM1621SendRows();
    return;
  }

*/
#ifdef SUPLA_CSE7766
  if (TM1621_POWR316D == Tm1621.device) {
    if (0 == Tm1621.display_rotate) {
      ext_snprintf_P(Tm1621.row[0], sizeof(Tm1621.row[0]), PSTR("%1_f"), &Energy->voltage[0]);
      ext_snprintf_P(Tm1621.row[1], sizeof(Tm1621.row[1]), PSTR("%1_f"), &Energy->current[0]);
      Tm1621.voltage = true;
      Tm1621.display_rotate = 1;
    } else {
      ext_snprintf_P(Tm1621.row[0], sizeof(Tm1621.row[0]), PSTR("%1_f"), &Energy->total[0]);
      ext_snprintf_P(Tm1621.row[1], sizeof(Tm1621.row[1]), PSTR("%1_f"), &Energy->active_power[0]);
      Tm1621.kwh = true;
      Tm1621.display_rotate = 0;
    }
    TM1621SendRows();
    return;
  }
#endif  // USE_ENERGY_SENSOR

 
  TM1621SendRows();
}




