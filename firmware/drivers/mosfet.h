#pragma once 
#include <cstdint>

extern "C" { 
    #include "stm32f4xx_hal.h"
}
namespace drivers { 
    class Mosfet { 
        public: 
            Mosfet(TIM_HandleTypeDef *htim, uint32_t channel); 

            bool init(); 
            bool setDutyCycle(float pct);
            bool off(); 
            bool on(); 
            bool isOn() const; 
        private: 
            TIM_HandleTypeDef *htim_; 
            uint32_t channel_; 
            float dutyPercent_; 
    };  
}