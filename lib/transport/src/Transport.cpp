#include "Arduino.h"
#include "Transport.h"

Transport::Transport(OutputBuffer *outputBuffer, size_t bufferSize)
    : m_OutputBuffer(outputBuffer), m_BufferSize(bufferSize)
{
  m_Buffer = (uint8_t *)malloc(m_BufferSize);
  m_Index = 0;
  setHeader(&PICOMM_DEFAULT_HEADER);
}

void Transport::addSample(int16_t sample)
{
  m_Buffer[m_Index + m_HeaderSize] = (sample + 32768) >> 8;
  m_Index++;
  // have we reached a full packet?
  if ((m_Index + m_HeaderSize) == m_BufferSize)
  {
    send();
    m_Index = 0;
  }
}

void Transport::flush()
{
  if (m_Index > 0)
  {
    send();
    m_Index = 0;
  }
}

bool Transport::setHeader(const PicommHeader *header)
{
  if (m_HeaderSize > m_BufferSize || header == nullptr)
  {
    return false;
  }

  memcpy(m_Buffer, header, m_HeaderSize);

  return true;
}
