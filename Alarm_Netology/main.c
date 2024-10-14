#include "stm32f10x.h" //Библиотека STM32
#include <stdio.h>
#include "Board_LED.h" //Библиотека для работы со светодиодами
#include "Board_Buttons.h" //Библиотека для работы с кнопками

//Структура хранящая время
struct time{
  uint32_t hours; //Часы
  uint32_t minutes; //Минуты
  uint32_t seconds; //Секунды
  uint32_t mseconds; //Милисекунды
} typedef time; //Чтобы не писать постоянно struct

time current_time; //Время
time alarm_time; //Время, в которое просигналит будильник
uint32_t time_signal; //Счетчик времени сигнала от будильника
uint32_t max_time_signal = 1000;  //Максимальное время в милисекундах для сигнала
char is_signal = 0; //Флаг, 1 - если будильник сигналит
uint32_t mseconds_for_alarm_settings = 60; //Коэффициент для настроки будильника

// Проверка на то, что время одинаковое
char is_equal(time *t1, time*t2){
  return (t1->hours == t2->hours) && (t1->minutes == t2->minutes);
}

// Добавление милисекунд
void time_add_mseconds(uint32_t mseconds, time *t){
  t->mseconds += mseconds; 
  if (t->mseconds >= 1000)
  {
      //Обновление секунд
      t->seconds += t->mseconds / 1000; 
      t->mseconds %= 1000; 
      if (t->seconds >= 60) {
        //Обновление минут
        t->minutes += t->seconds / 60; 
        t->seconds %= 60; 
        if (t->minutes >= 60) {
          //Обновление часов
          t->hours += t->minutes / 60; 
          t->minutes %= 60; 
          if (t->hours >= 24) {
            t->hours %= 24;  
          }
        }
      }
  }
}

//Инициализация времени
void init_time(time *st, uint32_t h, uint32_t m, uint32_t s) {
  st->hours = h;
  st->minutes = m;
  st->seconds = s;
  st->mseconds = 0;
}

//Проверка, что кнопка нажата
char button_is_pressed(){
  return GPIOB->ODR;
}

//Включение/выключение светодиода
void change_led(char on){
  if (on) {
    LED_On(0); 
  } else {
    LED_Off(0);
  }
}

//Обработчик системного таймера
void SysTick_Handler() {
  //Добавление 1 милисекунды
  time_add_mseconds(1, &current_time);

  //Если будильник сигнализирует
  if (is_signal){
    time_signal++;
    if (time_signal == max_time_signal) {
      //Выключаем будильник
      is_signal = 0;
      time_signal = 0;
      change_led(0);
    }
  }

  //Проверка для включения будильника
  if (is_equal(&current_time, &alarm_time) && current_time.seconds == 0){
    is_signal = 1;
    change_led(1);
  }
  
  //Если кнопка нажата
  if (button_is_pressed()){
    //Редактировать время будильника
    time_add_mseconds(mseconds_for_alarm_settings, &alarm_time);
  }
}

//Основная функция
int main(){
  //Инициализация времени
  init_time(&current_time, 8, 29, 59);
  init_time(&alarm_time, 8, 30, 0);

  //Инициализация светодиодов и кнопок
  LED_Initialize();
  Buttons_Initialize();

  //Настройка системного таймера
  SysTick_Config(SystemCoreClock / 1000);

  //Бесконечный цикл
  while(1){}
}
