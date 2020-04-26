#pragma once

#include "screen_common.h"
#include <array>
#include <bitset>


struct Main_screen : Screen {
   String_buffer& lcd;
   Buttons_events eventers;
   Callback<> out_callback;

   Main_screen(
        String_buffer& lcd
      , Buttons_events eventers
      , Out_callback out_callback
      
   ) : lcd          {lcd}
     , eventers     {eventers}
     , out_callback {out_callback.value}
   {}

   void init() override {
      eventers.enter ([this]{ });
      eventers.up    ([this]{ });
      eventers.down  ([this]{ });
      eventers.out   ([this]{ out_callback(); });
      lcd.clear();
      lcd.line(0) << "Работа ";
      lcd.line(1) << "Режим работы ";
   }

   void deinit() override {
      eventers.enter (nullptr);
      eventers.up    (nullptr);
      eventers.down  (nullptr);
      eventers.out   (nullptr);
   }

   void draw() override {
      lcd.line(0).cursor(10) << ':';
      lcd.line(1).cursor(10) << '%';
      
   }

};

struct Work_time_screen : Screen {
    String_buffer& lcd;
    Buttons_events eventers;
    Callback<> out_callback;
    const uint16_t& hours;

    Work_time_screen (
          String_buffer& lcd
        , Buttons_events eventers
        , Out_callback out_callback
        , uint16_t& hours
    ) : lcd          {lcd}
      , eventers     {eventers}
      , out_callback {out_callback.value}
      , hours        {hours}
    {}

    void init() override {
        eventers.out   ([this]{ out_callback(); });
        redraw();
    }
    void deinit() override {
        eventers.up    (nullptr);
        eventers.down  (nullptr);
        eventers.out   (nullptr);
    }

    void draw() override {}


private:
   
    void redraw() {
        lcd.line(0) << hours;
        // lcd << clear_after;
    }
};


