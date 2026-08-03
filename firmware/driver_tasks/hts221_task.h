#pragma once

#include <cstdint>
#include <optional>

extern "C" {
#include "stm32f4xx_hal.h"
}

namespace driver_tasks {

constexpr uint8_t kHts221DefaultI2CAddress = 0x5F;

struct HTS221Reading {
    float temperatureC;
    float humidityRH;
};

bool hts221TaskStart(I2C_HandleTypeDef *i2c, uint8_t i2c_addr = kHts221DefaultI2CAddress);

std::optional<HTS221Reading> hts221TaskLatestReading();

} // namespace driver_tasks
