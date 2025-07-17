#ifndef OLED_H
#define OLED_H

#include <Arduino.h>

#include <Adafruit_SSD1306.h>

#include "constants.h"

enum class TextAlign
{
    Min,
    Max
};

int16_t getAlignInfoDotOffset(TextAlign align, uint16_t size, uint16_t dotSize);
int16_t getAlignedStart(TextAlign align, uint16_t size, uint16_t containerSize);

void oledDrawUiItem(
    Adafruit_SSD1306 &display,
    const char *text, bool isActive, TextAlign xAlign, TextAlign yAlign, int16_t maxLen = -1,
    int16_t oledWidth = OLED_WIDTH, int16_t oledHeight = OLED_HEIGHT);

#endif // OLED_H