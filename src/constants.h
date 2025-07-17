#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "button.h"

#include "Transport.h"

#include <Arduino.h>

/* region Transport */

const TransportMode TRANSPORT_MODE = TransportMode::Udp;

// Only relevant for TransportMode::Udp
#define WIFI_SSID ""
#define WIFI_PASSWD ""

// Only relevant for TransportMode::EspNow
#define ESP_NOW_WIFI_CHANNEL 1

#if __has_include("wifi_creds.h")
#include "wifi_creds.h"
#endif // __has_include("wifi_creds.h")

/* endregion */

/* #region Audio */

#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT

// ADC1_CHANNEL_0 is GPIO 36 on ESP32
#define ADC_MIC_CHANNEL ADC1_CHANNEL_0

#define AUDIO_SAMPLE_RATE 48000

/* #endregion */

/* #region TX Buttons */

#define NUM_TX_BUTTONS 4
const TxButton TX_BUTTONS[NUM_TX_BUTTONS] = {
    {.pin = 16, .channel = 0xFF},
    {.pin = 17, .channel = 0x00},
    {.pin = 18, .channel = 0x01},
    {.pin = 19, .channel = 0x02},
};

/* #endregion */

/* region OLED */

#define OLED_WIDTH 128
#define OLED_HEIGHT 32

#define OLED_RESET -1

#define OLED_I2C_ADDRESS 0x3C

/* endregion */

#endif // CONSTANTS_H