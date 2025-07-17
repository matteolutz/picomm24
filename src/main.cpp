#include <Arduino.h>

#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#include "logo.h"
#include "oled.h"

#include "log.h"
#include "constants.h"

#include "id.h"

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

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

#define RECEIVE_BUFFER_SIZE 128
int16_t samples[RECEIVE_BUFFER_SIZE];

int64_t transmittingButtonIdx = -1;

void drawUi();

void setup()
{
  initDeviceId();
  LOG_INIT();

  LOG_INFO("piComm (by Matteo Lutz), version %s, built on %s %s", PICOMM_VERSION, __DATE__, __TIME__);
  LOG_INFO("Device Efuse MAC: %s", String(ESP.getEfuseMac(), HEX).c_str());
  LOG_INFO("Device ID Hash: %s", deviceId.c_str());

  LOG_INFO("Starting...");

  LOG_INFO("Deserializing preferences...");
  Preferences prefs;
  prefs.clear();

  if (!prefs.begin("picomm", false))
  {
    LOG_ERROR("Failed to open preferences, rebooting...");
    delay(5000);
    ESP.restart();
    return;
  }

  for (size_t i = 0; i < NUM_TX_BUTTONS; i++)
  {
    serializeButton(prefs, TX_BUTTONS[i], i);
  }

  LOG_INFO("Preferences deserialized successfully");

  LOG_INFO("Initializing OLED Display...");
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS))
  {
    LOG_ERROR("Failed to initialize OLED display, rebooting...");
    delay(5000);
    ESP.restart();
    return;
  }

  display.clearDisplay();
  display.drawBitmap(0, 0, picommLogo, OLED_WIDTH, OLED_HEIGHT, WHITE);
  display.display();

  LOG_INFO("OLED Display initialized successfully");

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
    if (TX_BUTTONS[i].pin < 0)
    {
      LOG_DEBUG("Skipping TX button %zu as pin is invalid", i);
      continue;
    }

    pinMode(TX_BUTTONS[i].pin, INPUT_PULLUP);
  }
  LOG_INFO("TX Buttons initialized successfully");

  LOG_INFO("Done! Let's rock this show!");

  delay(2000);
  drawUi();
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
    if (TX_BUTTONS[i].pin < 0)
    {
      continue; // Skip unconfigured buttons
    }

    if (digitalRead(TX_BUTTONS[i].pin) == HIGH)
      continue;

    LOG_INFO("TX button %d pressed, transmitting", i);

    transmittingButtonIdx = i;
    drawUi();

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

    transmittingButtonIdx = -1;
    lastTx = millis();

    drawUi();
  }
}

void receive()
{
  outputBuffer.remove_samples(samples, RECEIVE_BUFFER_SIZE);
  output.write(samples, RECEIVE_BUFFER_SIZE);
}

void drawUi()
{
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setTextWrap(false);

  for (size_t i = 0; i < NUM_TX_BUTTONS; i++)
  {
    oledDrawUiItem(display, getButtonName(TX_BUTTONS[i]), i == transmittingButtonIdx,
                   i % 2 == 0 ? TextAlign::Min : TextAlign::Max,
                   i < 2 ? TextAlign::Min : TextAlign::Max);
  }

  display.display();
}