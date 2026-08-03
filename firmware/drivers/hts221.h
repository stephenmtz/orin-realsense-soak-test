#pragma once 
#include <cstdint> 
#include <optional> 

extern "C" { 
    #include "stm32f4xx_hal.h"
}

namespace drivers { 
    class HTS221 { 
        public:
            using RegVal = uint8_t; 
            using RegAddr = uint8_t;
            using RegBitPos = uint8_t;
            using RegMask = uint8_t;
            
            enum dataRate : RegVal { 
                kOneShot = 0x00, 
                kOneHtz = 0x01, 
                kSevenHtz = 0x02, 
                kTwelveHtz = 0x03, 
            };

            HTS221(I2C_HandleTypeDef *i2c, RegVal addr);
            
            bool init(); 
            bool whoAmI(); 
            bool readTemp(float &outC); 
            bool readHumidity(float &outRH);
            bool setDataRate(dataRate rate); 
            bool setResolution(); 
            bool setHeater(bool on);
            bool setBlockDataUpdate(bool enable); 
            

        private: 
            I2C_HandleTypeDef *i2c_;
            RegAddr addr_;
            static constexpr RegAddr kWhoAmI = 0x0F;
            static constexpr RegAddr kAvConf = 0x10; 
            static constexpr RegAddr kCtrlReg1 = 0x20; 
            static constexpr RegAddr kCtrlReg2 = 0x21;
            static constexpr RegAddr kCtrlReg3 = 0x22; 
            static constexpr RegAddr kStatusReg = 0x27;
            static constexpr RegAddr kHumidityOutL = 0x28;
            static constexpr RegAddr kHumidityOutH = 0x29;
            static constexpr RegAddr kTempOutL = 0x2A;
            static constexpr RegAddr kTempOutH = 0x2B;
            static constexpr RegAddr kH0RhX2 = 0x30;
            static constexpr RegAddr kH1RhX2 = 0x31;
            static constexpr RegAddr kT0DegCX8 = 0x32;
            static constexpr RegAddr kT1DegCX8 = 0x33;
            static constexpr RegAddr kT1T0Msb = 0x35; 
            static constexpr RegAddr kH0T0OutL = 0x36;
            static constexpr RegAddr kH0T0OutH = 0x37;
            static constexpr RegAddr kH1T0OutL = 0x3A;
            static constexpr RegAddr kH1T0OutH = 0x3B;
            static constexpr RegAddr kT0OutL = 0x3C;
            static constexpr RegAddr kT0OutH = 0x3D;
            static constexpr RegAddr kT1OutL = 0x3E;
            static constexpr RegAddr kT1OutH = 0x3F;

            float tempSlope_;
            float tempIntercept_;
            float humiditySlope_;
            float humidityIntercept_;

            bool readCalibration();
            std::optional<RegVal> readReg(RegAddr reg_addr);
            bool writeReg(RegAddr reg_addr, RegVal reg_val);
            template <typename T> 
            std::optional<T> readBitField(RegAddr addr, RegMask mask, RegBitPos bit_pos); 
            bool writeBitField(RegAddr addr, RegMask mask, RegBitPos bit_pos, RegVal val);
            template <typename T>
            std::optional<T> readMultReg(RegAddr start_reg_addr, T bitMask);
    }; // class HTS221
} // namespace drivers 