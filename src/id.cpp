#include "id.h"

String deviceId;

String getDeviceIdHash()
{
    uint64_t deviceId = ESP.getEfuseMac();
    deviceId >>= 3;
    uint32_t hashed = (deviceId ^ (deviceId >> 10) ^ (deviceId >> 20)) & 0x3FFFF;

    return String(hashed, HEX);
}

void initDeviceId()
{
    deviceId = getDeviceIdHash();
}