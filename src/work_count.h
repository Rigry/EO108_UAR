#pragma once

#include "timers.h"

namespace {
    constexpr auto update_minutes        {60'000};
    constexpr auto remember_every        {3'600'000};
    constexpr auto update_every_remember {60};
}

struct Minutes {
    uint32_t data {0};
};

// считает время работы лампы в минутах
template<class Flash_data>
struct Work_count : TickSubscriber {
    
    Flash_data& flash;
    int tick_cnt {0};
    Minutes minutes;
    uint16_t hours{0}; 
    uint16_t mins{0};

    Work_count (Flash_data& flash) : flash{flash} {start();}

    Minutes* get_data() { return &minutes; }
    void start() { tick_subscribe();  }
    void stop()  { tick_unsubscribe();}

    void notify() override {
        if (not (++tick_cnt % update_minutes)) {
            minutes.data++;
            mins = minutes.data % update_every_remember;
        }
        
        if (tick_cnt == remember_every) {
            tick_cnt = 0;
            hours = get_hours();
            flash.hours += 1;
        }
    }

    uint16_t get_hours () {
        return minutes.data / update_every_remember;
    }
    void reset () {
        minutes.data = 0;
        flash.hours  = 0;
        hours        = 0;
        mins         = 0;
    }
    
};