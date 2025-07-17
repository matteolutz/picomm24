#include <Arduino.h>

#include <WiFi.h>

#include "log.h"
#include "constants.h"

#include "EspNowTransport.h"
#include "UdpTransport.h"
#include "PicommHeader.h"

#include "ADCSampler.h"

#include "Output.h"
#include "DACOutput.h"
#include "OutputBuffer.h"

OutputBuffer outputBuffer(300 * 16);

Transport *transport = nullptr;

i2s_config_t i2sAdcConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = AUDIO_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = I2S_COMM_FORMAT_STAND_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

ADCSampler input(ADC_UNIT_1, ADC_MIC_CHANNEL, i2sAdcConfig);
DACOutput output(I2S_NUM_0);

#define RECEIVE_BUFFER_SIZE 128
int16_t samples[RECEIVE_BUFFER_SIZE];

void setup()
{
  LOG_INIT();

  LOG_INFO("Starting...");

  LOG_INFO("Initializing WiFi and creating Transport...");
  WiFi.mode(WIFI_STA);
  switch (TRANSPORT_MODE)
  {
  case TransportMode::EspNow:
    LOG_DEBUG("Transport mode: ESP-NOW");
    WiFi.disconnect();
    transport = new EspNowTransport(&outputBuffer, ESP_NOW_WIFI_CHANNEL);
    break;
  case TransportMode::Udp:
    LOG_DEBUG("Transport mode: UDP");
    WiFi.begin(WIFI_SSID, WIFI_PASSWD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      LOG_ERROR("Failed to connect to WiFi network '%s', rebooting...", WIFI_SSID);
      delay(5000);
      ESP.restart();
      return;
    }

    WiFi.setSleep(WIFI_PS_NONE);
    LOG_INFO("Connected to WiFi network: %s", WIFI_SSID);
    LOG_INFO("IP Address: %s", WiFi.localIP().toString().c_str());
    LOG_INFO("MAC Address: %s", WiFi.macAddress().c_str());

    transport = new UdpTransport(&outputBuffer);
    break;
  }

  transport->begin();
  LOG_INFO("WiFi and Transport initialized successfully");

  LOG_INFO("Initializing Output...");
  output.start(AUDIO_SAMPLE_RATE);
  LOG_INFO("Output initialized successfully");

  LOG_INFO("Flushing OutputBuffer...");
  outputBuffer.flush();
  LOG_INFO("OutputBuffer flushed successfully");

  LOG_INFO("Initializing TX Buttons...");
  for (size_t i = 0; i < NUM_TX_BUTTONS; i++)
  {
    pinMode(TX_BUTTONS[i].pin, INPUT_PULLUP);
  }
  LOG_INFO("TX Buttons initialized successfully");

  LOG_INFO("Done! Let's rock this show!");
}

void send();
void receive();

void loop()
{
  send();
  receive();
}

unsigned long lastTx = 0;
void send()
{
  if (millis() - lastTx < 1000)
  {
    return; // Avoid sending too frequently
  }

  // Check if any TX button is pressed
  for (size_t i = 0; i < NUM_TX_BUTTONS; i++)
  {
    if (digitalRead(TX_BUTTONS[i].pin) == HIGH)
      continue;

    LOG_INFO("TX button %d pressed, transmitting", i);

    PicommHeader header = {
        .channel = TX_BUTTONS[i].channel,
    };
    transport->setHeader(&header);

    output.stop();
    input.start();

    LOG_DEBUG("Starting transmission on channel %d", TX_BUTTONS[i].channel);
    unsigned long txStart = millis();
    while (millis() - txStart < 1000 || digitalRead(TX_BUTTONS[i].pin) == LOW)
    {
      int samplesRead = input.read(samples, RECEIVE_BUFFER_SIZE);
      for (int i = 0; i < samplesRead; i++)
      {
        transport->addSample(samples[i]);
      }
    }

    transport->flush();

    input.stop();
    output.start(AUDIO_SAMPLE_RATE);

    LOG_INFO("Transmission complete on channel %d, receiving again...", TX_BUTTONS[i].channel);
    lastTx = millis();
  }
}

void receive()
{
  outputBuffer.remove_samples(samples, RECEIVE_BUFFER_SIZE);
  output.write(samples, RECEIVE_BUFFER_SIZE);
}