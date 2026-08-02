#pragma once 
#include <cstdint> 

extern "C" { 
    #include "stm32f4xx_hal.h"
}

namespace drivers { 
    class HTS221 { 
        public:
            auto RegVal = uint8_t; 
             
            HTS221(I2C_HandlerTypeDef* hi2c, RegVal addr = 0xBE); 
            bool init(); 
            bool readTemp(float &outC); 
            bool readHumidity(float &outRH); 

        private: 
            bool writeReg()
    }
}