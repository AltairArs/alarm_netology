#include "stm32f10x.h"
#include <stdio.h>
#include "Board_LED.h"
#include "Board_Buttons.h"

struct time{
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;
  uint32_t mseconds;
} typedef time;

time current_time;
time alarm_time;
uint32_t time_signal;
uint32_t max_time_signal = 1000; 
char is_signal = 0; 
uint32_t mseconds_for_alarm_settings = 60;

char is_equal(time *t1, time*t2){
  return (t1->hours == t2->hours) && (t1->minutes == t2->minutes);
}

void time_add_mseconds(uint32_t mseconds, time *t){
  t->mseconds += mseconds; 
  if (t->mseconds >= 1000)
  {
      t->seconds += t->mseconds / 1000; 
      t->mseconds %= 1000; 
      if (t->seconds >= 60) {
        t->minutes += t->seconds / 60; 
        t->seconds %= 60; 
        if (t->minutes >= 60) {
          t->hours += t->minutes / 60; 
          t->minutes %= 60; 
          if (t->hours >= 24) {
            t->seconds %= 24;  
          }
        }
      }
  }
}

void init_time(time *st, uint32_t h, uint32_t m, uint32_t s) {
  st->hours = h;
  st->minutes = m;
  st->seconds = s;
  st->mseconds = 0;
}

char button_is_pressed(){
  return GPIOB->ODR;
}

void Delay(uint32_t delay_ticks){
  uint32_t current_ticks;
  
  current_ticks = current_time.mseconds;
  while((current_time.mseconds - current_ticks) < delay_ticks) __NOP();
}

void change_led(char on){
  if (on) {
    LED_On(0); 
  } else {
    LED_Off(0);
  }
}

void SysTick_Handler() {
  time_add_mseconds(1, &current_time);
  
  if (is_signal){
    time_signal++;
    if (time_signal == max_time_signal) {
      is_signal = 0;
      time_signal = 0;
      change_led(0);
    }
  }
  
  if (is_equal(&current_time, &alarm_time) && current_time.seconds == 0){
    is_signal = 1; // ????????? ??????? ??????????
    change_led(1); // ????????? ??????????
  }
  
  if (button_is_pressed()){
    time_add_mseconds(mseconds_for_alarm_settings, &alarm_time);
  }
}

int main(){
  init_time(&current_time, 8, 29, 59);
  init_time(&alarm_time, 8, 30, 0);
  
  LED_Initialize();
  Buttons_Initialize();
  SysTick_Config(SystemCoreClock / 1000);
  
  while(1){}
}