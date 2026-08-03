#pragma once 

namespace drivers { 
    class Mosfet { 
        public: 
            bool init(); 
            bool setDutyCycle(float pct);
            bool off(); 
    };
}