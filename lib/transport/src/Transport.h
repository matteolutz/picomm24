#pragma once
#include <stdlib.h>
#include <stdint.h>

#include "PicommHeader.h"

class OutputBuffer;

enum class TransportMode
{
  EspNow,
  Udp
};

class Transport
{
protected:
  const int m_HeaderSize = sizeof(PicommHeader);

protected:
  // audio buffer for samples we need to send
  uint8_t *m_Buffer = NULL;
  int m_BufferSize = 0;
  int m_Index = 0;

  OutputBuffer *m_OutputBuffer = NULL;

protected:
  virtual void send() = 0;

public:
  Transport(OutputBuffer *outputBuffer, size_t bufferSize);

  virtual bool begin() = 0;

  bool setHeader(const PicommHeader *header);
  void addSample(int16_t sample);

  void flush();
};
