#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "OutputBuffer.h"
#include "EspNowTransport.h"

const int MAX_ESP_NOW_PACKET_SIZE = 250;
const uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static EspNowTransport *instance = NULL;

void receiveCallback(const uint8_t *macAddr, const uint8_t *data, int dataLen)
{
  // annoyingly we can't pass an param into this so we need to do a bit of hack to access the EspNowTransport instance
  int headerSize = instance->m_HeaderSize;

  // first m_header_size bytes of m_buffer are the expected header
  if ((dataLen > headerSize) && (dataLen <= MAX_ESP_NOW_PACKET_SIZE) && (memcmp(data, instance->m_Buffer, headerSize) == 0))
  {
    instance->m_OutputBuffer->add_samples(data + headerSize, dataLen - headerSize);
  }
}

bool EspNowTransport::begin()
{
  // Set Wifi channel
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(m_WifiChannel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  esp_err_t result = esp_now_init();
  if (result == ESP_OK)
  {
    Serial.println("ESPNow Init Success");
    esp_now_register_recv_cb(receiveCallback);
  }
  else
  {
    Serial.printf("ESPNow Init failed: %s\n", esp_err_to_name(result));
    return false;
  }
  // this will broadcast a message to everyone in range
  esp_now_peer_info_t peerInfo = {};
  memcpy(&peerInfo.peer_addr, broadcastAddress, 6);
  if (!esp_now_is_peer_exist(broadcastAddress))
  {
    result = esp_now_add_peer(&peerInfo);
    if (result != ESP_OK)
    {
      Serial.printf("Failed to add broadcast peer: %s\n", esp_err_to_name(result));
      return false;
    }
  }
  return true;
}

EspNowTransport::EspNowTransport(OutputBuffer *outputBuffer, uint8_t wifiChannel) : Transport(outputBuffer, MAX_ESP_NOW_PACKET_SIZE)
{
  instance = this;
  m_WifiChannel = wifiChannel;
}

void EspNowTransport::send()
{
  esp_err_t result = esp_now_send(broadcastAddress, m_Buffer, m_Index + m_HeaderSize);

  if (result != ESP_OK)
  {
    Serial.printf("Failed to send: %s\n", esp_err_to_name(result));
  }
}
