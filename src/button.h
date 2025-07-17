#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include <Preferences.h>

typedef struct
{
    int8_t pin;
    uint8_t channel;
    const char *name;
} TxButton;

const char *getButtonName(const TxButton &button);

void serializeButton(Preferences &prefs, const TxButton &button, size_t idx);
TxButton deserializeButton(Preferences &prefs, size_t idx);

#endif // BUTTON_H