/**
 * ESP32 Q-Learning 조도 제어 시스템
 * utils.cpp - 유틸리티 함수 구현
 */

 #include "utils.h"
 #include "config.h"
 #include "sensors.h"
 
 /**
  * 시스템 상태 출력 함수
  */
 void printSystemStatus() {
   Serial.println("\n----- 시스템 상태 -----");
   
   // 시간 정보
   Serial.print("시간: ");
   Serial.print(currentHour);
   Serial.print(":");
   if (currentMinute < 10) Serial.print("0");
   Serial.print(currentMinute);
   Serial.print(" (");
   Serial.print(currentDay);
   Serial.print("/");
   Serial.print(currentMonth);
   Serial.println(")");
   
   // 센서 정보
   float lux = measureLux();
   Serial.print("현재 조도: ");
   Serial.print(lux);
   Serial.println(" lux");
   
   // 제어 정보
   Serial.print("현재 LED PWM: ");
   Serial.print(currentPWM);
   Serial.print(" (");
   Serial.print(map(currentPWM, 0, 255, 0, 100));
   Serial.println("%)");
   
   // 학습 정보
   Serial.print("학습 모드: ");
   Serial.println(learningMode ? "활성" : "비활성");
   Serial.print("현재 탐색률: ");
   Serial.println(currentEpsilon);
   Serial.print("에피소드 수: ");
   Serial.println(episodeCount);
   
   Serial.println("----------------------");
 }
 
 /**
  * 상태 LED 점멸 함수
  */
 void blinkStatusLED(int times) {
   for (int i = 0; i < times; i++) {
     digitalWrite(STATUS_LED_PIN, HIGH);
     delay(100);
     digitalWrite(STATUS_LED_PIN, LOW);
     delay(100);
   }
 }
 
 /**
  * 부동 소수점 맵핑 함수 (Arduino map의 float 버전)
  */
 float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }