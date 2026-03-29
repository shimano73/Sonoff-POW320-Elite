#ifndef GUI_Generic_Config_h
#define GUI_Generic_Config_h



#define RELAY_BI_SET_PIN 2  
#define RELAY_BI_RESET_PIN 4
#define CSE7766_RX_PIN 16
#define CFG_BUTTON_PIN 0
#define CFG_LED_PIN 5


// ##### User configuration #####
#define USE_CUSTOM
#define SUPLA_SONOFF_POW_ELITE


#ifdef USE_CUSTOM
#define BUILD_VERSION "User GUI 1.0.1"

#define TEMPLATE_BOARD_OLD

#define DEFAULT_TEMPLATE_BOARD BOARD_SHELLY2


// #define SUPLA_OTA

#define SUPLA_ENABLE_GUI
#define SUPLA_ENABLE_SSL

#define SUPLA_CONFIG
#define SUPLA_LED
#define SUPLA_CSE7766


#endif  // USE_CUSTOM
#endif  // GUI-Generic_Config_h
