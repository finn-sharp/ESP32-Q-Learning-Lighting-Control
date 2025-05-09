/**
 * ESP32 Q-Learning 조도 제어 시스템
 * utils.h - 유틸리티 함수 헤더
 */

 #ifndef UTILS_H
 #define UTILS_H
 
 #include <Arduino.h>
 
 // 함수 선언
 void printSystemStatus();
 void blinkStatusLED(int times);
 float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
 
 #endif  // UTILS_H