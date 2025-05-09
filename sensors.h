/**
 * ESP32 Q-Learning 조도 제어 시스템
 * sensors.h - 센서 관련 함수 헤더
 */

 #ifndef SENSORS_H
 #define SENSORS_H
 
 #include <Arduino.h>
 #include <Wire.h>
 #include <BH1750.h>
 #include <RTClib.h>
 
 // 센서 객체 초기화
 extern BH1750 lightMeter;
 extern RTC_DS3231 rtc;
 
 // 현재 시간 정보
 extern int currentHour;
 extern int currentMinute;
 extern int currentDay;
 extern int currentMonth;
 extern unsigned long lastTimeUpdate;
 
 // 함수 선언
 bool initializeSensors();
 float measureLux();
 void updateCurrentTime();
 int getLuxLevel(float lux);
 int getTimePeriod();
 int getSeasonPeriod();
 
 #endif  // SENSORS_H