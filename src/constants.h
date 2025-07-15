#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

#pragma region Radio

#define RADIO_CE 0
#define RADIO_CSN 0
#define RADIO_IRQ 0

#define RADIO_CHANNEL 1

// Number of reading pipes (i.e. channels to receive from and send to, this is 5 at max)
#define RADIO_N_COMM_PIPES 4

const uint64_t PIPES[14] =
    {
        0xABCDABCD71LL,
        0x544d52687CLL,
        0x544d526832LL,
        0x544d52683CLL,
        0x544d526846LL,
        0x544d526850LL,
        0x544d52685ALL,
        0x544d526820LL,
        0x544d52686ELL,
        0x544d52684BLL,
        0x544d526841LL,
        0x544d526855LL,
        0x544d52685FLL,
        0x544d526869LL};

#pragma endregion

#pragma region Audio

#define AUDIO_DAC_CHANNEL 0
#define AUDIO_SAMPLE_RATE 16050

#pragma endregion

#pragma region TX Buttons

const size_t TX_BUTTONS[RADIO_N_COMM_PIPES] = {0, 0, 0, 0};

#pragma endregion

#endif // CONSTANTS_H