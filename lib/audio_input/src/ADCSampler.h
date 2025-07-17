#pragma once

#include <driver/adc.h>
#include "I2SSampler.h"

class ADCSampler : public I2SSampler
{
private:
    adc_unit_t m_AdcUnit;
    adc1_channel_t m_AdcChannel;

protected:
    void configureI2S();
    void unConfigureI2S();

public:
    ADCSampler(adc_unit_t adcUnit, adc1_channel_t adcChannel, const i2s_config_t &i2sConfig);
    virtual int read(int16_t *samples, int count);
};
