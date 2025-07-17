#include "button.h"

#include "PicommHeader.h"

#include <Preferences.h>

const char *getButtonName(const TxButton &button)
{
    if (button.channel == PICOMM_CHANNEL_BROADCAST)
    {
        return "ALL";
    }

    return button.name ? button.name : "Unknown";
}

void serializeButton(Preferences &prefs, const TxButton &button, size_t idx)
{
    String key = "btn_" + String(idx);
    prefs.putString((key + "_name").c_str(), button.name);
    prefs.putUInt((key + "_pin").c_str(), button.pin);
    prefs.putUInt((key + "_ch").c_str(), button.channel);
}

TxButton deserializeButton(Preferences &prefs, size_t idx)
{
    TxButton button;
    String key = "btn_" + String(idx);
    button.name = prefs.getString((key + "_name").c_str(), "").c_str();
    button.pin = prefs.getUInt((key + "_pin").c_str(), -1);
    button.channel = prefs.getUInt((key + "_ch").c_str(), PICOMM_CHANNEL_BROADCAST);
    return button;
}