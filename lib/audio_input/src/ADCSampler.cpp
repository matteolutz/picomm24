#include "ADCSampler.h"

#if CONFIG_IDF_TARGET_ESP32

ADCSampler::ADCSampler(adc_unit_t adcUnit, adc1_channel_t adcChannel, const i2s_config_t &i2sConfig) : I2SSampler(I2S_NUM_0, i2sConfig)
{
    m_AdcUnit = adcUnit;
    m_AdcChannel = adcChannel;
}

void ADCSampler::configureI2S()
{
    // init ADC pad
    i2s_set_adc_mode(m_AdcUnit, m_AdcChannel);
    // enable the adc
    i2s_adc_enable(m_i2sPort);
}

void ADCSampler::unConfigureI2S()
{
    // make sure ot do this or the ADC is locked
    i2s_adc_disable(m_i2sPort);
}

int ADCSampler::read(int16_t *samples, int count)
{
    // read from i2s
    size_t bytesRead = 0;
    i2s_read(m_i2sPort, samples, sizeof(int16_t) * count, &bytesRead, portMAX_DELAY);

    int samplesRead = bytesRead / sizeof(int16_t);

    for (int i = 0; i < samplesRead; i++)
    {
        samples[i] = (2048 - (uint16_t(samples[i]) & 0xfff)) * 15;
    }

    return samplesRead;
}

#endif
