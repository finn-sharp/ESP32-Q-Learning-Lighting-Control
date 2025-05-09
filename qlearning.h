/**
 * ESP32 Q-Learning 조도 제어 시스템
 * qlearning.h - Q-Learning 알고리즘 헤더
 */

 #ifndef QLEARNING_H
 #define QLEARNING_H
 
 #include <Arduino.h>
 
 // 상태 및 Q-테이블 관련 변수
 extern float qTable[LUX_LEVELS][TIME_PERIODS][SEASON_PERIODS][LED_SETTINGS][ACTIONS];
 extern int currentLuxLevel;
 extern int currentLEDSetting;
 extern int currentPWM;
 extern float currentEpsilon;
 extern bool learningMode;
 extern unsigned long episodeCount;
 
 // Q-Learning 함수
 void initializeQTable();
 int selectAction(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting);
 int getBestAction(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting);
 float calculateReward(float lux, int pwm);
 void updateQTable(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting,
                  int action, float reward, int newLuxLevel, int newTimePeriod, int newSeasonPeriod);
 void adjustLearningParameters();
 
 #endif  // QLEARNING_H