#ifndef ID_H
#define ID_H

#include <Arduino.h>

#define PICOMM_VERSION "0.1.0"

extern String deviceId;

String getDeviceIdHash();
void initDeviceId();

#endif // ID_H