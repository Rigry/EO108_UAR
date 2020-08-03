#pragma once

#include "screen_common.h"
#include "pwm_.h"
#include "const_chain.h"
#include <array>
#include <bitset>


struct Main_screen : Screen {
   String_buffer& lcd;
   Buttons_events eventers;
   Callback<> out_callback;
   PWM& pwm;
   uint16_t& hours;
   uint16_t& minutes;
   char element{0};

   uint16_t& flash_power;
   ConstChain<uint16_t, 200, 400, 600, 800, 990> power;

   bool blink_{false};
   Timer blink{500_ms};

   Main_screen(
        String_buffer& lcd
      , Buttons_events eventers
      , Out_callback out_callback
      , PWM& pwm
      , uint16_t& hours
      , uint16_t& minutes
      , uint16_t& flash_power
      
   ) : lcd          {lcd}
     , eventers     {eventers}
     , out_callback {out_callback.value}
     , pwm          {pwm}
     , hours        {hours}
     , minutes      {minutes}
     , flash_power  {flash_power}
   {power = flash_power;}

   void init() override {
      eventers.enter ([this]{ });
      eventers.up    ([this]{flash_power = pwm.duty_cycle = ++power;});
      eventers.down  ([this]{flash_power = pwm.duty_cycle = --power;});
      eventers.out   ([this]{ out_callback(); });
      lcd.clear();
      lcd.line(0) << "Работа ";
      lcd.line(1) << "Мощность ";
   }

   void deinit() override {
      eventers.enter (nullptr);
      eventers.up    (nullptr);
      eventers.down  (nullptr);
      eventers.out   (nullptr);
   }

   void draw() override {
      hours   > 9 ? lcd.line(0).cursor(10).width(2) << hours : lcd.line(0).cursor(10) << 0 << hours;
      blink_ ^= blink.event();
      blink_ ? lcd.line(0).cursor(12) << ':' : lcd.line(0).cursor(12) << ' ';
      minutes > 9 ? lcd.line(0).cursor(13).width(2) << minutes : lcd.line(0).cursor(13) << 0 << minutes;

      pwm.duty_cycle == 200 ? lcd.line(1).cursor(10) << element                       : pwm.duty_cycle == 400 ?
                              lcd.line(1).cursor(10) << element << element            : pwm.duty_cycle == 600 ?
                              lcd.line(1).cursor(10) << element << element << element : pwm.duty_cycle == 800 ?
                              lcd.line(1).cursor(10) << element << element << element << element : 
                              lcd.line(1).cursor(10) << element << element << element << element << element;
      lcd << clear_after;
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
        lcd.line(0) << "Общая наработка";
        lcd.line(1).width(5) << hours << 'ч';
        lcd << clear_after;
    }
};

struct Qty_show_screen : Screen {
   String_buffer& lcd;
   Buttons_events eventers;
   Callback<> out_callback;
   const std::string_view name;
   const uint16_t& qty_times;

   Qty_show_screen (
         String_buffer& lcd
       , Buttons_events eventers
       , Out_callback out_callback
       , std::string_view name
       , uint16_t& qty_times
   ) : lcd          {lcd}
     , eventers     {eventers}
     , out_callback {out_callback.value}
     , name         {name}
     , qty_times    {qty_times}
   {}

   void init() override {
      eventers.up    ([this]{ });
      eventers.down  ([this]{ });
      eventers.out   ([this]{ out_callback(); });
      lcd.line(0) << name << next_line;
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
      lcd.line(1) << qty_times;
      lcd << clear_after;
   }
};


