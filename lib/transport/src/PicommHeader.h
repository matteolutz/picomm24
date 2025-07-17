#ifndef PICOMM_HEADER_H
#define PICOMM_HEADER_H

#include <Arduino.h>

#define PICOMM_CHANNEL_BROADCAST 0xFF

typedef struct
{
    uint8_t channel;
} PicommHeader;

const PicommHeader PICOMM_DEFAULT_HEADER = {
    .channel = PICOMM_CHANNEL_BROADCAST};

#endif // PICOMM_HEADER_H