/**
 * ESP32 Q-Learning 조도 제어 시스템
 * storage.cpp - 데이터 저장 관련 함수 구현
 */

 #include "storage.h"
 #include "config.h"
 #include "qlearning.h"
 #include "pattern_analysis.h"
 #include <SPIFFS.h>
 
 /**
  * 저장소 초기화 함수
  */
 bool initializeStorage() {
   Serial.println("SPIFFS 초기화 중...");
   
   if (!SPIFFS.begin(true)) {
     Serial.println("SPIFFS 마운트 실패!");
     return false;
   }
   
   Serial.println("SPIFFS 마운트 성공");
   return true;
 }
 
 /**
  * Q-테이블 저장 함수
  */
 bool saveQTable() {
   Serial.println("Q-테이블 저장 중...");
   
   File file = SPIFFS.open(SPIFFS_QTABLE_FILE, FILE_WRITE);
   if (!file) {
     Serial.println("파일 열기 실패!");
     return false;
   }
   
   // Q-테이블 데이터 저장
   for (int i = 0; i < LUX_LEVELS; i++) {
     for (int j = 0; j < TIME_PERIODS; j++) {
       for (int k = 0; k < SEASON_PERIODS; k++) {
         for (int l = 0; l < LED_SETTINGS; l++) {
           for (int m = 0; m < ACTIONS; m++) {
             file.write((uint8_t*)&qTable[i][j][k][l][m], sizeof(float));
           }
         }
       }
     }
   }
   
   file.close();
   Serial.println("Q-테이블 저장 완료");
   return true;
 }
 
 /**
  * Q-테이블 로드 함수
  */
 bool loadQTable() {
   Serial.println("Q-테이블 로드 중...");
   
   if (!SPIFFS.exists(SPIFFS_QTABLE_FILE)) {
     Serial.println("Q-테이블 파일이 존재하지 않음");
     return false;
   }
   
   File file = SPIFFS.open(SPIFFS_QTABLE_FILE, FILE_READ);
   if (!file) {
     Serial.println("파일 열기 실패!");
     return false;
   }
   
   // 파일 크기 검증
   size_t expectedSize = LUX_LEVELS * TIME_PERIODS * SEASON_PERIODS * LED_SETTINGS * ACTIONS * sizeof(float);
   if (file.size() != expectedSize) {
     Serial.println("파일 크기 불일치! Q-테이블을 다시 초기화합니다.");
     file.close();
     return false;
   }
   
   // Q-테이블 데이터 로드
   for (int i = 0; i < LUX_LEVELS; i++) {
     for (int j = 0; j < TIME_PERIODS; j++) {
       for (int k = 0; k < SEASON_PERIODS; k++) {
         for (int l = 0; l < LED_SETTINGS; l++) {
           for (int m = 0; m < ACTIONS; m++) {
             file.read((uint8_t*)&qTable[i][j][k][l][m], sizeof(float));
           }
         }
       }
     }
   }
   
   file.close();
   Serial.println("Q-테이블 로드 완료");
   return true;
 }
 
 /**
  * 패턴 데이터 저장 함수
  */
 bool savePatternData() {
   Serial.println("패턴 데이터 저장 중...");
   
   File file = SPIFFS.open(SPIFFS_PATTERN_FILE, FILE_WRITE);
   if (!file) {
     Serial.println("파일 열기 실패!");
     return false;
   }
   
   // 일별 패턴 데이터 저장
   for (int i = 0; i < MAX_DAILY_PATTERNS; i++) {
     // 월, 일 저장
     file.write(dailyPatterns[i].month);
     file.write(dailyPatterns[i].day);
     
     // 시간별 조도 및 LED 설정 저장
     for (int j = 0; j < HOURS_PER_DAY; j++) {
       file.write((uint8_t*)&dailyPatterns[i].hourlyLux[j], sizeof(float));
       file.write(dailyPatterns[i].hourlyLED[j]);
     }
   }
   
   file.close();
   Serial.println("패턴 데이터 저장 완료");
   return true;
 }
 
 /**
  * 패턴 데이터 로드 함수
  */
 bool loadPatternData() {
   Serial.println("패턴 데이터 로드 중...");
   
   if (!SPIFFS.exists(SPIFFS_PATTERN_FILE)) {
     Serial.println("패턴 데이터 파일이 존재하지 않음");
     return false;
   }
   
   File file = SPIFFS.open(SPIFFS_PATTERN_FILE, FILE_READ);
   if (!file) {
     Serial.println("파일 열기 실패!");
     return false;
   }
   
   // 파일 크기 검증
   size_t entrySize = 2 + HOURS_PER_DAY * (sizeof(float) + 1);  // 월, 일 + 시간별 데이터
   size_t expectedSize = MAX_DAILY_PATTERNS * entrySize;
   
   if (file.size() != expectedSize) {
     Serial.println("파일 크기 불일치! 패턴 데이터를 다시 초기화합니다.");
     file.close();
     return false;
   }
   
   // 일별 패턴 데이터 로드
   for (int i = 0; i < MAX_DAILY_PATTERNS; i++) {
     // 월, 일 로드
     dailyPatterns[i].month = file.read();
     dailyPatterns[i].day = file.read();
     
     // 시간별 조도 및 LED 설정 로드
     for (int j = 0; j < HOURS_PER_DAY; j++) {
       file.read((uint8_t*)&dailyPatterns[i].hourlyLux[j], sizeof(float));
       dailyPatterns[i].hourlyLED[j] = file.read();
     }
   }
   
   file.close();
   Serial.println("패턴 데이터 로드 완료");
   return true;
 }