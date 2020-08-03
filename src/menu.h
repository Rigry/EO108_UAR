#pragma once

#include "string_buffer.h"
#include "hd44780.h"
#include "select_screen.h"
#include "set_screen.h"
#include "screens.h"
#include "button.h"
#include "pwm_.h"
#include "work_count.h"

template<class Pins, class Flash_data, class Work_count, size_t qty_lines = 4, size_t size_line = 20>
struct Menu : TickSubscriber {
   String_buffer lcd {};
   HD44780& hd44780 {HD44780::make(Pins{}, lcd.get_buffer())};
   Button_event& up;
   Button_event& down;
   Button_event& enter;
   Flash_data&   flash;
   PWM&          pwm;
   Work_count&   work_count;


   Screen* current_screen {&main_screen};
   size_t tick_count{0};

   Buttons_events buttons_events {
        Up_event    {[this](auto c){   up.set_click_callback(c);}}
      , Down_event  {[this](auto c){ down.set_click_callback(c);}}
      , Enter_event {[this](auto c){enter.set_click_callback(c);}}
      , Out_event   {[this](auto c){enter.set_long_push_callback(c);}}
      , Increment_up_event   {[this](auto c){  up.set_increment_callback(c);}}
      , Increment_down_event {[this](auto c){down.set_increment_callback(c);}}
   };

   Menu (
        Pins pins
      , Button_event& up
      , Button_event& down
      , Button_event& enter
      , Flash_data&   flash
      , PWM&          pwm
      , Work_count&   work_count
   ) : up{up}, down{down}, enter{enter}
      , flash{flash}, pwm{pwm}, work_count{work_count}
   {
      tick_subscribe();
      current_screen->init();
      while(not hd44780.init_done()){}
   }

   Main_screen main_screen {
          lcd, buttons_events
      //   , Enter_event  { [this](auto c){enter.set_click_callback(c);}}
        , Out_callback { [this]{ change_screen(main_select); }}
        , pwm
        , work_count.hours
        , work_count.mins
        , flash.power
   };

   Select_screen<2> main_select {
          lcd, buttons_events
        , Out_callback          { [this]{change_screen(main_screen);}}
        , Line {"Наработка"      ,[this]{change_screen(work_time)  ;}}
        , Line {"Информация"     ,[this]{change_screen(info_screen);}}

   };

   Select_screen<4> work_time {
          lcd, buttons_events
        , Out_callback            {[this]{change_screen(main_screen)     ;}}
        , Line {"Общая наработка" ,[this]{change_screen(work_time_screen);}}
        , Line {"Сброс наработки" ,[this]{flash.reset++;
                                          work_count.reset();
                                          change_screen(work_time_screen);}}
        , Line {"Кол-во сбросов"  ,[this]{change_screen(qty_reset_show)  ;}}
        , Line {"Кол-во вкл/откл" ,[this]{change_screen(qty_on_off_show) ;}}

   };

   Work_time_screen work_time_screen {
          lcd, buttons_events
        , Out_callback       { [this]{ change_screen(work_time);}}
        , flash.hours
   };

   Qty_show_screen qty_reset_show {
        lcd, buttons_events
      , Out_callback  { [this]{ change_screen(work_time); }}
      , "Кол-во сбросов"
      , flash.reset
   };

   Qty_show_screen qty_on_off_show {
        lcd, buttons_events
      , Out_callback  { [this]{ change_screen(work_time); }}
      , "Кол-во включений"
      , flash.on_off
   };
   
   Select_screen<2> info_screen {
          lcd, buttons_events
        , Out_callback          { [this]{change_screen(main_screen);}}
        , Line {"НовоТех-ЭКО"    ,[this]{}}
        , Line {"Наша реклама"   ,[this]{}}

   };
   
   
   void notify() override {
      every_qty_cnt_call(tick_count, 50, [this]{
         current_screen->draw();
      });
   }

   void change_screen(Screen& new_screen) {
      current_screen->deinit();
      current_screen = &new_screen;
      current_screen->init();
   }

};