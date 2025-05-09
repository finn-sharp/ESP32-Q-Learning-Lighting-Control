/**
 * ESP32 Q-Learning 조도 제어 시스템
 * sensors.cpp - 센서 관련 함수 구현
 */

 #include "sensors.h"
 #include "config.h"
 
 // 센서 객체 정의
 BH1750 lightMeter;
 RTC_DS3231 rtc;
 
 // 현재 시간 정보
 int currentHour = 12;    // 기본값
 int currentMinute = 0;
 int currentDay = 1;
 int currentMonth = 1;
 unsigned long lastTimeUpdate = 0;
 
 /**
  * 센서 초기화 함수
  */
 bool initializeSensors() {
   bool success = true;
   
   // BH1750 조도 센서 초기화
   if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, BH1750_ADDR)) {
     Serial.println("BH1750 조도 센서 초기화 성공");
   } else {
     Serial.println("BH1750 조도 센서 초기화 실패");
     success = false;
   }
   
   // DS3231 RTC 초기화
   if (rtc.begin()) {
     Serial.println("DS3231 RTC 초기화 성공");
     
     // RTC가 멈춰있거나 처음 사용 시 시간 설정 (컴파일 시간 기준)
     if (rtc.lostPower()) {
       Serial.println("RTC 시간 재설정 (컴파일 시간 기준)");
       rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     }
     
     // 현재 시간 업데이트
     updateCurrentTime();
     
   } else {
     Serial.println("DS3231 RTC 초기화 실패");
     success = false;
   }
   
   return success;
 }
 
 /**
  * 조도 측정 함수
  */
 float measureLux() {
   float lux = lightMeter.readLightLevel();
   
   // 비정상 값 필터링 (센서 오류 등)
   if (isnan(lux) || lux < 0) {
     Serial.println("조도 센서 오류! 기본값 사용");
     return 500.0;  // 기본값
   }
   
   return lux;
 }
 
 /**
  * 현재 시간 업데이트 함수
  */
 void updateCurrentTime() {
   DateTime now = rtc.now();
   
   currentHour = now.hour();
   currentMinute = now.minute();
   currentDay = now.day();
   currentMonth = now.month();
   
   Serial.print("현재 시간: ");
   Serial.print(currentHour);
   Serial.print(":");
   Serial.print(currentMinute);
   Serial.print(" 날짜: ");
   Serial.print(currentDay);
   Serial.print("/");
   Serial.println(currentMonth);
 }
 
 /**
  * 조도 레벨 결정 함수
  */
 int getLuxLevel(float lux) {
   for (int i = 0; i < LUX_LEVELS; i++) {
     if (lux >= luxBoundaries[i] && lux < luxBoundaries[i+1]) {
       return i;
     }
   }
   return LUX_LEVELS - 1;  // 기본적으로 최대 레벨 반환
 }
 
 /**
  * 시간대 결정 함수
  */
 int getTimePeriod() {
   for (int i = 0; i < TIME_PERIODS; i++) {
     if (currentHour >= timeBoundaries[i] && currentHour < timeBoundaries[i+1]) {
       return i;
     }
   }
   return 0;  // 기본값
 }
 
 /**
  * 계절 결정 함수
  */
 int getSeasonPeriod() {
   if (currentMonth >= seasonBoundaries[0] && currentMonth < seasonBoundaries[1]) {
     return 0;  // 봄 (3-5월)
   } else if (currentMonth >= seasonBoundaries[1] && currentMonth < seasonBoundaries[2]) {
     return 1;  // 여름 (6-8월)
   } else if (currentMonth >= seasonBoundaries[2] && currentMonth < seasonBoundaries[3]) {
     return 2;  // 가을 (9-11월)
   } else {
     return 3;  // 겨울 (12-2월)
   }
 }