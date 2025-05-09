/**
 * ESP32 Q-Learning 조도 제어 시스템
 * storage.h - 데이터 저장 관련 함수 헤더
 */

 #ifndef STORAGE_H
 #define STORAGE_H
 
 #include <Arduino.h>
 
 // 함수 선언
 bool initializeStorage();
 bool saveQTable();
 bool loadQTable();
 bool savePatternData();
 bool loadPatternData();
 
 #endif  // STORAGE_H