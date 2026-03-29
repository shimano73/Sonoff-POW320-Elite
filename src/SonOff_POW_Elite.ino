/*
  Copyright (C) krycha88

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
#include "SuplaDeviceGUI.h"
#include "SuplaTM1621.h"
#include "supla/control/Bistable2Relay.h"
#include <supla/actions.h>
#include <supla/control/button.h>
#ifdef ARDUINO_ARCH_ESP8266
extern "C" {
#include "user_interface.h"
}
#endif




//Measurement measurement;


BistableRelay *relay;

int StatusDevice = 0;
unsigned long pm_lastmillis =0;

int delay_time = 500; 
bool error = true;

uint8_t refreshCnt = 0;
bool showEnergy = false;
char voltageStr[8];   // bufor dla float jako string
char currentStr[8];   // bufor dla float jako string
char powerStr[8];   // bufor dla float jako string
char energyStr[8];   // bufor dla float jako string  



void format4(char *buf, float value, bool decimal) {

  if (decimal && value < 10) {
    int v = (int)value;
    int d = (int)((value - v) * 10 + 0.5);

    snprintf(buf, 5, "%1d.%1d", v, d);
  } 
  else {
    snprintf(buf, 5, "%4d", (int)(value + 0.5));
  }
}



void Anime_logo() {

  const char* text = "SUPLA";
  char frame[5];  // 4 znaki + null

  int tdelay = 250;

  // długość animacji: tekst + 4 puste miejsca
  int total = strlen(text) + 4;

  for (int step = 0; step < total; step++) {

    for (int i = 0; i < 4; i++) {

      int idx = step - (3 - i);

      if (idx >= 0 && idx < strlen(text)) {
        frame[i] = text[idx];
      } else {
        frame[i] = ' ';
      }
    }

    frame[4] = '\0';

    // ustawiamy wiersze
    strcpy(Tm1621.row[0], frame);
    strcpy(Tm1621.row[1], "    ");

    dot1 = false;
    dot2 = false;

    TM1621SendRows();

    delay(tdelay);
  }
}


void DisplayStatus(int status) {

  char buf[8];

  // format "-17"
  snprintf(buf, sizeof(buf), "%d", status);

  // formatowanie do wyświetlacza
  dot1 = formatRow(buf, Tm1621.row[0]);
  dot2 = false;

  strcpy(Tm1621.row[1], "    ");

  TM1621SendRows();
}

void formatSmart(float val, char* out) {

    // 🔴 zakresy dopasowane do 4 cyfr
    if (val < 1.0f) {
        // 0.976 → 1.0 lub 0.9
        val = roundf(val * 10.0f) / 10.0f;
        snprintf(out, 8, "%.1f", val);
    }
    else if (val < 10.0f) {
        // 9.987 → 10.0 lub 9.9
        val = roundf(val * 10.0f) / 10.0f;
        snprintf(out, 8, "%.1f", val);
    }
    else if (val < 100.0f) {
        // 12.34 → 12.3
        val = roundf(val * 10.0f) / 10.0f;
        snprintf(out, 8, "%.1f", val);
    }
    else if (val < 1000.0f) {
        // 123.4 → 123
        val = roundf(val);
        snprintf(out, 8, "%.0f", val);
    }
    else {
        // overflow → 9999
        strcpy(out, "9999");
    }
}

void setup() {
  Serial.begin(115200);
  eeprom.setStateSavePeriod(5000);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

 
  // Initialize TH1621 display
 
  TM1621PreInit();
  TM1621Init();
  Tm1621.device = TM1621_POWR316D;


  Anime_logo();
  // Clear_row();



  ConfigManager = new SuplaConfigManager();
  ConfigESP = new SuplaConfigESP();

  ImprovSerialComponent *improvSerialComponent = new ImprovSerialComponent();
  improvSerialComponent->enable();

  // Configure main relay
  auto relay = new BistableRelay(RELAY_BI_SET_PIN, RELAY_BI_RESET_PIN,100);
  // Configure cfg button and led 
  Supla::GUI::addConfigESP(ConfigESP->getGpio(FUNCTION_CFG_BUTTON), ConfigESP->getGpio(FUNCTION_CFG_LED));
  // Configure CSE7766 energy meter
  Supla::GUI::addCSE7766(ConfigESP->getGpio(FUNCTION_CSE7766_RX));
  improvSerialComponent->disable();

  auto button = new Supla::Control::Button(0, true);
  button->addAction(Supla::TOGGLE, relay,Supla::ON_PRESS); 

  






//  new Supla::Sensor::EspFreeHeap();

  Supla::GUI::begin();
}

void loop() {
  if (millis() - pm_lastmillis >= delay_time) {
    pm_lastmillis = millis();  
    StatusDevice = SuplaDevice.getCurrentStatus();
    Serial.print("Status device : ");
    Serial.println(StatusDevice); 
   
    if (StatusDevice != 17) error = true;
    if (error) 
    switch (StatusDevice) {
      case  5: 
              DisplayStatus(StatusDevice);
      break;
      case  8: 
               DisplayStatus(StatusDevice);
      break;
      case 10:
               DisplayStatus(StatusDevice);
      break;
      case 17:
              DisplayStatus(StatusDevice);
      break;
      case 25:
               DisplayStatus(StatusDevice);
      break; 
      case 31: {
              // Configure
              DisplayStatus(StatusDevice);               
      }
      break;
      case 40:{ 
              // Configure
               DisplayStatus(StatusDevice);                         
              }
      break;
      case 144 : // Clear_row();
      break;
    
    

    } // if switch
 
     
    if (measurement.valid) {
       refreshCnt++;

      if (refreshCnt >= 10) {   // 10 * 500ms = 5s
        refreshCnt = 0;
        showEnergy = !showEnergy;
      }

    formatSmart(measurement.voltage, voltageStr);
    formatSmart(measurement.current, currentStr);
    formatSmart(measurement.energy, energyStr);       
    formatSmart(measurement.power, powerStr );

    if (showEnergy)           
          DisplayText(voltageStr, currentStr, UNIT_VOLT);
    else  DisplayText(energyStr, powerStr, UNIT_KWH);
/*
    Serial.println(" *********************************************************************** ");    
    Serial.printf(
      "U=%.1f V  I=%.3f A  P=%.1f W  E=%.3f kWh\n",
      measurement.voltage,
      measurement.current,
      measurement.power,
      measurement.energy);
*/
    }
  else
    Serial.println("Brak danych z licznika");
            
    
  } //millis
  SuplaDevice.iterate();
  
}
