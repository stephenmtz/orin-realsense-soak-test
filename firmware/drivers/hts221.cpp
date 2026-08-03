#include "hts221.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_i2c.h"
#include "task.h"
#include <algorithm>
#include <cstdint>
#include <type_traits>

namespace drivers {
    constexpr uint32_t kI2cTimeoutMs = 100;
    constexpr HTS221::RegVal kWhoAmIValue = 0xBC;
    constexpr HTS221::RegMask kPowerDownMask = 0x80;
    constexpr HTS221::RegBitPos kPowerDownBitPos = 7;
    constexpr HTS221::RegMask kBduMask = 0x04;
    constexpr HTS221::RegBitPos kBduBitPos = 2;
    constexpr HTS221::RegMask kOdrMask = 0x03;
    constexpr HTS221::RegBitPos kOdrBitPos = 0;
    constexpr HTS221::RegMask kHeaterMask = 0x02;
    constexpr HTS221::RegBitPos kHeaterBitPos = 1;
    constexpr HTS221::RegVal kAvConfDefault = 0x1B;
    constexpr HTS221::RegAddr kAutoIncrementBit = 0x80;

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

    template <typename T>
    std::optional<T> HTS221::readBitField(RegAddr addr, RegMask mask, RegBitPos bit_pos) {
        auto cur = readReg(addr);
        if (!cur) {
            return std::nullopt;
        }
        return static_cast<T>((*cur & mask) >> bit_pos);
    }

    template <typename T>
    std::optional<T> HTS221::readMultReg(RegAddr start_reg_addr, T bitMask) {
        uint8_t buf[sizeof(T)] = {};
        if (HAL_I2C_Mem_Read(i2c_, addr_ << 1, start_reg_addr | kAutoIncrementBit, I2C_MEMADD_SIZE_8BIT, buf,
                              sizeof(buf), kI2cTimeoutMs) != HAL_OK) {
            return std::nullopt;
        }

        typename std::make_unsigned<T>::type raw = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            raw |= static_cast<decltype(raw)>(buf[i]) << (8 * i);
        }
        return static_cast<T>(raw & static_cast<decltype(raw)>(bitMask));
    }

    bool HTS221::whoAmI() {
        auto val = readReg(kWhoAmI);
        return val.has_value() && *val == kWhoAmIValue;
    }

    bool HTS221::setDataRate(dataRate rate) {
        return writeBitField(kCtrlReg1, kOdrMask, kOdrBitPos, static_cast<RegVal>(rate));
    }

    bool HTS221::setResolution() {
        return writeReg(kAvConf, kAvConfDefault);
    }

    bool HTS221::setHeater(bool on) {
        return writeBitField(kCtrlReg2, kHeaterMask, kHeaterBitPos, on ? 1 : 0);
    }

    bool HTS221::setBlockDataUpdate(bool enable) {
        return writeBitField(kCtrlReg1, kBduMask, kBduBitPos, enable ? 1 : 0);
    }

    bool HTS221::readCalibration() {
        auto h0_rh_x2 = readReg(kH0RhX2);
        auto h1_rh_x2 = readReg(kH1RhX2);
        auto t0_degc_x8 = readReg(kT0DegCX8);
        auto t1_degc_x8 = readReg(kT1DegCX8);
        auto t0_msb = readBitField<uint16_t>(kT1T0Msb, 0x03, 0);
        auto t1_msb = readBitField<uint16_t>(kT1T0Msb, 0x0C, 2);
        auto h0_t0_out = readMultReg<int16_t>(kH0T0OutL, static_cast<int16_t>(0xFFFF));
        auto h1_t0_out = readMultReg<int16_t>(kH1T0OutL, static_cast<int16_t>(0xFFFF));
        auto t0_out = readMultReg<int16_t>(kT0OutL, static_cast<int16_t>(0xFFFF));
        auto t1_out = readMultReg<int16_t>(kT1OutL, static_cast<int16_t>(0xFFFF));

        if (!h0_rh_x2 || !h1_rh_x2 || !t0_degc_x8 || !t1_degc_x8 || !t0_msb || !t1_msb || !h0_t0_out || !h1_t0_out ||
            !t0_out || !t1_out) {
            return false;
        }

        if (*t1_out == *t0_out || *h1_t0_out == *h0_t0_out) {
            return false;
        }

        const float h0_rh = *h0_rh_x2 / 2.0f;
        const float h1_rh = *h1_rh_x2 / 2.0f;
        const float t0_degc = static_cast<float>((*t0_msb << 8) | *t0_degc_x8) / 8.0f;
        const float t1_degc = static_cast<float>((*t1_msb << 8) | *t1_degc_x8) / 8.0f;

        tempSlope_ = (t1_degc - t0_degc) / static_cast<float>(*t1_out - *t0_out);
        tempIntercept_ = t0_degc - tempSlope_ * static_cast<float>(*t0_out);

        humiditySlope_ = (h1_rh - h0_rh) / static_cast<float>(*h1_t0_out - *h0_t0_out);
        humidityIntercept_ = h0_rh - humiditySlope_ * static_cast<float>(*h0_t0_out);

        return true;
    }

    bool HTS221::init() {
        if (!whoAmI()) {
            return false;
        }
        if (!setBlockDataUpdate(true) || !setResolution() || !setDataRate(kOneHtz)) {
            return false;
        }
        if (!writeBitField(kCtrlReg1, kPowerDownMask, kPowerDownBitPos, 1)) {
            return false;
        }
        return readCalibration();
    }

    bool HTS221::readTemp(float &outC) {
        auto raw = readMultReg<int16_t>(kTempOutL, static_cast<int16_t>(0xFFFF));
        if (!raw) {
            return false;
        }
        outC = tempSlope_ * static_cast<float>(*raw) + tempIntercept_;
        return true;
    }

    bool HTS221::readHumidity(float &outRH) {
        auto raw = readMultReg<int16_t>(kHumidityOutL, static_cast<int16_t>(0xFFFF));
        if (!raw) {
            return false;
        }
        outRH = std::clamp(humiditySlope_ * static_cast<float>(*raw) + humidityIntercept_, 0.0f, 100.0f);
        return true;
    }

} // namespace drivers
