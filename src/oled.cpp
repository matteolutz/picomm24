#include "oled.h"

#include <Arduino.h>

int16_t getAlignInfoDotOffset(TextAlign align, uint16_t size, uint16_t dotSize)
{
    uint16_t spacing = 4;

    switch (align)
    {
    case TextAlign::Min:
        return spacing + size;
    case TextAlign::Max:
        return -spacing - dotSize;
    }
    return 0; // Default case, should not happen
}

int16_t getAlignedStart(TextAlign align, uint16_t size, uint16_t containerSize)
{
    switch (align)
    {
    case TextAlign::Min:
        return 0;
    case TextAlign::Max:
        return containerSize - size;
    }
    return 0; // Default case, should not happen
}

void oledDrawUiItem(
    Adafruit_SSD1306 &display,
    const char *text, bool isActive, TextAlign xAlign, TextAlign yAlign, int16_t maxLen, int16_t oledWidth, int16_t oledHeight)
{
    size_t len = strlen(text);
    if (maxLen >= 0 && len > static_cast<size_t>(maxLen))
    {
        len = static_cast<size_t>(maxLen);
    }

    String textString = String(text).substring(0, len);
    switch (xAlign)
    {
    case TextAlign::Min:
        textString = "< " + textString;
        break;
    case TextAlign::Max:
        textString = textString + " >";
        break;
    }

    int16_t _x1, _y1;
    uint16_t textWidth, textHeight;

    display.getTextBounds(textString, 0, 0, &_x1, &_y1, &textWidth, &textHeight);

    uint16_t x = getAlignedStart(xAlign, textWidth, oledWidth);
    uint16_t y = getAlignedStart(yAlign, textHeight, oledHeight);

    display.setCursor(x, y);
    display.print(textString);

    if (isActive)
    {
        uint16_t dotSize = textHeight - 2;

        uint16_t dotX = x + getAlignInfoDotOffset(xAlign, textWidth, dotSize);
        uint16_t dotY = y;

        display.fillRect(dotX, dotY, dotSize, dotSize, WHITE);
    }
}
