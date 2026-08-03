#include "mosfet.h"
#include "FreeRTOS.h"

namespace drivers { 

    Mosfet::Mosfet(TIM_HandleTypeDef *htim, uint32_t channel) 
        : htim_(htim), channel_(channel), dutyPercent_(0.0f){
            configAsser(htim_ != nullptr);
        }
    
    bool Mosfet::init(){
        if(!)
    }

}