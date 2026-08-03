#include "hts221.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_i2c.h"
#include "task.h"
#include <cstdint>

namespace drivers {
    constexpr uint32_t kI2cTimeoutMs = 100; 
    
    HTS221::HTS221(I2C_HandleTypeDef *i2c, RegVal addr) : i2c_(i2c), addr_(addr) {
        configASSERT(i2c_ != nullptr);
    }

    std::optional<HTS221::RegVal> HTS221::readReg(RegAddr reg_addr) {
        RegVal val;
        if (HAL_I2C_Mem_Read(i2c_, addr_ << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, &val, 1, kI2cTimeoutMs) != HAL_OK) {
            return std::nullopt;
        }
        return val;
    }

    bool HTS221::writeReg(RegAddr reg_addr, RegVal reg_val) {
        return HAL_I2C_Mem_Write(i2c_, addr_ << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, kI2cTimeoutMs) == HAL_OK;
    }

    bool HTS221::writeBitField(RegAddr addr, RegMask mask, RegBitPos bit_pos, RegVal val) {
        auto cur = readReg(addr);
        if (!cur) {
            return false;
        }
        RegVal updated = static_cast<RegVal>((*cur & ~mask) | ((val << bit_pos) & mask));
        return writeReg(addr, updated);
    }
} // namespace drivers
