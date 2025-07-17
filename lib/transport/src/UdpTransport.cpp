#include <Arduino.h>
#include <AsyncUDP.h>
#include "UdpTransport.h"
#include "OutputBuffer.h"

#include "PicommHeader.h"

const int MAX_UDP_SIZE = 1436;

UdpTransport::UdpTransport(OutputBuffer *outputBuffer) : Transport(outputBuffer, MAX_UDP_SIZE)
{
}

unsigned long lastPacket;
bool UdpTransport::begin()
{
  udp = new AsyncUDP();
  lastPacket = millis();
  if (udp->listen(8192))
  {
    Serial.println("Listening on port 8192");
    udp->onPacket([this](AsyncUDPPacket packet)
                  {
                    // our packets contain unsigned 8 bit PCM samples
                    // so we can push them straight into the output buffer
                    if (packet.length() <= m_HeaderSize || packet.length() > MAX_UDP_SIZE)
                    {
                      return;
                    }

                    m_OutputBuffer->add_samples(packet.data() + m_HeaderSize, packet.length() - m_HeaderSize); });
    return true;
  }
  Serial.println("Failed to listen");
  return false;
}

void UdpTransport::send()
{
  udp->broadcast(m_Buffer, m_Index);
}
