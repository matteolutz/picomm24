#include <Arduino.h>

#include <AutoAnalogAudio.h>
#include <RF24.h>

#include "constants.h"

#ifndef IRAM_ATTR
#define IRAM_ATTR
#endif // IRAM_ATTR

AutoAnalog audio;

// RF24 radio(RADIO_CE, RADIO_CSN);

const size_t COMM_PIPES[RADIO_N_COMM_PIPES] = {0, 1, 2, 3};

void IRAM_ATTR DACC_Handler(void)
{
  audio.dacHandler(); // Link the DAC ISR/IRQ library. Called by the MCU when DAC is ready for data
}

void IRAM_ATTR RX_Handler()
{
  /*
  uint8_t pipe;
  while (radio.available(&pipe))
  {
    radio.read(&audio.dacBuffer, 32);


  audio.feedDAC(AUDIO_DAC_CHANNEL, 32);
}
else
{
  // If not RX IRQ, than it must be TX complete
  radio.txStandby();
  radio.startListening();
}
*/
}

void setupAudio()
{
  audio.begin(1, 1);
  audio.autoAdjust = 0;
  audio.dacBitsPerSample = 8;
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
  /*
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

    attachInterrupt(digitalPinToInterrupt(RADIO_IRQ), RX_Handler, FALLING);
    */
}

uint8_t shiftVal = 0;

void arraysetup(void)
{
  audio.dacBuffer[0] = 127 >> shiftVal;
  audio.dacBuffer[1] = 152 >> shiftVal;
  audio.dacBuffer[2] = 176 >> shiftVal;
  audio.dacBuffer[3] = 198 >> shiftVal;
  audio.dacBuffer[4] = 217 >> shiftVal;
  audio.dacBuffer[5] = 233 >> shiftVal;
  audio.dacBuffer[6] = 245 >> shiftVal;
  audio.dacBuffer[7] = 252 >> shiftVal;
  audio.dacBuffer[8] = 254 >> shiftVal;
  audio.dacBuffer[9] = 252 >> shiftVal;
  audio.dacBuffer[10] = 245 >> shiftVal;
  audio.dacBuffer[11] = 233 >> shiftVal;
  audio.dacBuffer[12] = 217 >> shiftVal;
  audio.dacBuffer[13] = 198 >> shiftVal;
  audio.dacBuffer[14] = 176 >> shiftVal;
  audio.dacBuffer[15] = 152 >> shiftVal;
  audio.dacBuffer[16] = 128 >> shiftVal;
  audio.dacBuffer[17] = 103 >> shiftVal;
  audio.dacBuffer[18] = 79 >> shiftVal;
  audio.dacBuffer[19] = 57 >> shiftVal;
  audio.dacBuffer[20] = 38 >> shiftVal;
  audio.dacBuffer[21] = 22 >> shiftVal;
  audio.dacBuffer[22] = 10 >> shiftVal;
  audio.dacBuffer[23] = 3 >> shiftVal;
  audio.dacBuffer[24] = 0 >> shiftVal;
  audio.dacBuffer[25] = 3 >> shiftVal;
  audio.dacBuffer[26] = 10 >> shiftVal;
  audio.dacBuffer[27] = 22 >> shiftVal;
  audio.dacBuffer[28] = 38 >> shiftVal;
  audio.dacBuffer[29] = 57 >> shiftVal;
  audio.dacBuffer[30] = 79 >> shiftVal;
  audio.dacBuffer[31] = 103 >> shiftVal;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for Serial to be ready

  Serial.println("[picomm] Welcome, starting up...");

  Serial.println("[picomm] Initializing audio...");
  setupAudio();
  arraysetup();
  Serial.println("[picomm] Audio initialized.");

  Serial.print("[picomm] DAC0_PIN = ");
  Serial.println(DAC0_PIN);

  Serial.println("[picomm] Initializing TX buttons...");
  // setupTxButtons();
  Serial.println("[picomm] TX buttons initialized.");

  Serial.println("[picomm] Initializing radio...");
  setupRadio();
  Serial.println("[picomm] Radio initialized.");
}

void loop()
{
  /*
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
    */

  /*
audio.getADC(32);

Serial.print("[");
for (size_t i = 0; i < 32; i++)
{
  Serial.print(audio.adcBuffer[i]);
  if (i < 31)
    Serial.print(", ");
}
Serial.println("]");
*/

  audio.feedDAC(AUDIO_DAC_CHANNEL, 32);
}