/**
 * ESP32 Q-Learning 조도 제어 시스템
 * pattern_analysis.h - 패턴 분석 관련 함수 헤더
 */

 #ifndef PATTERN_ANALYSIS_H
 #define PATTERN_ANALYSIS_H
 
 #include <Arduino.h>
 
 // 일일 패턴 구조체
 struct DailyPattern {
   uint8_t month;
   uint8_t day;
   float hourlyLux[HOURS_PER_DAY];
   uint8_t hourlyLED[HOURS_PER_DAY];
 };
 
 // 전역 변수
 extern DailyPattern dailyPatterns[MAX_DAILY_PATTERNS];
 extern float todayLux[HOURS_PER_DAY];
 extern int todayLED[HOURS_PER_DAY];
 extern int luxDataCount[HOURS_PER_DAY];
 extern bool environmentChangeDetected;
 
 // 함수 선언
 void initializePatternAnalysis();
 void recordLuxData(int hour, float lux, int pwm);
 void updateDailyPattern();
 bool detectEnvironmentChange();
 void checkSeasonChange();
 
 #endif  // PATTERN_ANALYSIS_H