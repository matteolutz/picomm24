#include <Arduino.h>

#include <AutoAnalogAudio.h>
#include <RF24.h>

#include "constants.h"

AutoAnalog audio;

RF24 radio(RADIO_CE, RADIO_CSN);

const size_t COMM_PIPES[RADIO_N_COMM_PIPES] = {0, 1, 2, 3};

void DACC_Handler(void)
{
  audio.dacHandler(); // Link the DAC ISR/IRQ library. Called by the MCU when DAC is ready for data
}

void RX()
{
  uint8_t pipe;
  while (radio.available(&pipe))
  {
    radio.read(&audio.dacBuffer, 32);

    /*
    for (int i = 0; i < 16; i++)
    { // Convert signed 16-bit variables into unsigned 12-bit
      audio.dacBuffer16[i] += 0x8000;
      audio.dacBuffer16[i] = audio.dacBuffer16[i] >> 4;
    }
      */

    audio.feedDAC(AUDIO_DAC_CHANNEL, 32);
  }
  else
  {
    // If not RX IRQ, than it must be TX complete
    radio.txStandby();
    radio.startListening();
  }
}

void setupAudio()
{
  audio.begin(1, 1);
  audio.autoAdjust = 0;
  audio.dacBitsPerSample = 12;
  audio.adcBitsPerSample = 8;
  audio.setSampleRate(AUDIO_SAMPLE_RATE);
}

void setupTxButtons()
{
  for (size_t i = 0; i < RADIO_N_COMM_PIPES; ++i)
  {
    pinMode(TX_BUTTONS[i], INPUT_PULLUP);
  }
}

void setupRadio()
{
  radio.begin();
  radio.setChannel(RADIO_CHANNEL);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_8);
  radio.setAddressWidth(5);

  for (uint8_t i = 0; i < RADIO_N_COMM_PIPES; ++i)
  {
    radio.openReadingPipe(i + 1, PIPES[COMM_PIPES[i]]);
  }

  radio.txDelay = 0;
  radio.csDelay = 0;

  radio.maskIRQ(0, 1, 0);
  radio.printDetails();

  radio.startListening();

  attachInterrupt(digitalPinToInterrupt(RADIO_IRQ), RX, FALLING);
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for Serial to be ready

  Serial.println("[picomm] Welcome, starting up...");

  Serial.println("[picomm] Initializing audio...");
  setupAudio();
  Serial.println("[picomm] Audio initialized.");

  Serial.println("[picomm] Initializing TX buttons...");
  setupTxButtons();
  Serial.println("[picomm] TX buttons initialized.");

  Serial.println("[picomm] Initializing radio...");
  setupRadio();
  Serial.println("[picomm] Radio initialized.");
}

void loop()
{
  for (size_t i = 0; i < RADIO_N_COMM_PIPES; ++i)
  {
    if (digitalRead(TX_BUTTONS[i]) == HIGH)
      continue; // Skip if the button is not pressed

    radio.stopListening();
    radio.openWritingPipe(PIPES[COMM_PIPES[i]]);

    while (digitalRead(TX_BUTTONS[i]) == LOW)
    {
      audio.getADC(32);
      radio.startFastWrite(&audio.adcBuffer, 32, 1);
    }

    // radio.startListening();
  }
}