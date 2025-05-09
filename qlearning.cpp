/**
 * ESP32 Q-Learning 조도 제어 시스템
 * qlearning.cpp - Q-Learning 알고리즘 구현
 */

 #include "qlearning.h"
 #include "config.h"
 
 // Q-테이블 및 관련 변수
 float qTable[LUX_LEVELS][TIME_PERIODS][SEASON_PERIODS][LED_SETTINGS][ACTIONS] = {0};
 int currentLuxLevel = 2;     // 중간 값으로 초기화
 int currentLEDSetting = 2;   // 중간 값으로 초기화
 int currentPWM = 128;        // 중간 값으로 초기화
 float currentEpsilon = EPSILON_START;
 bool learningMode = true;
 unsigned long episodeCount = 0;
 
 /**
  * Q-테이블 초기화 함수
  */
 void initializeQTable() {
   Serial.println("Q-테이블 초기화 중...");
   
   // Q-테이블을 모두 0으로 초기화
   for (int i = 0; i < LUX_LEVELS; i++) {
     for (int j = 0; j < TIME_PERIODS; j++) {
       for (int k = 0; k < SEASON_PERIODS; k++) {
         for (int l = 0; l < LED_SETTINGS; l++) {
           for (int m = 0; m < ACTIONS; m++) {
             qTable[i][j][k][l][m] = 0.0;
           }
         }
       }
     }
   }
   
   Serial.println("Q-테이블 초기화 완료");
 }
 
 /**
  * 행동 선택 함수 (ε-탐욕 정책)
  */
 int selectAction(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting) {
   if (random(100) < currentEpsilon * 100) {
     // 탐색: 무작위 행동
     return random(ACTIONS);
   } else {
     // 활용: 최대 Q값을 가진 행동
     return getBestAction(luxLevel, timePeriod, seasonPeriod, ledSetting);
   }
 }
 
 /**
  * 최적 행동 선택 함수
  */
 int getBestAction(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting) {
   float maxQ = -1000000;
   int bestAction = 0;
   
   for (int a = 0; a < ACTIONS; a++) {
     if (qTable[luxLevel][timePeriod][seasonPeriod][ledSetting][a] > maxQ) {
       maxQ = qTable[luxLevel][timePeriod][seasonPeriod][ledSetting][a];
       bestAction = a;
     }
   }
   
   return bestAction;
 }
 
 /**
  * 보상 계산 함수
  */
 float calculateReward(float lux, int pwm) {
   // 목표 조도와의 차이에 대한 페널티
   float luxDifference = lux - TARGET_LUX;
   float luxPenalty = -(luxDifference * luxDifference) / LUX_SCALING;
   
   // 전력 소비에 대한 페널티 (PWM 값에 비례)
   float powerPenalty = -POWER_PENALTY * (pwm / 255.0);
   
   return luxPenalty + powerPenalty;
 }
 
 /**
  * Q-테이블 업데이트 함수
  */
 void updateQTable(int luxLevel, int timePeriod, int seasonPeriod, int ledSetting,
                  int action, float reward, int newLuxLevel, int newTimePeriod, int newSeasonPeriod) {
   
   // 다음 상태에서 최대 Q값 찾기
   float maxNextQ = -1000000;
   for (int a = 0; a < ACTIONS; a++) {
     if (qTable[newLuxLevel][newTimePeriod][newSeasonPeriod][action][a] > maxNextQ) {
       maxNextQ = qTable[newLuxLevel][newTimePeriod][newSeasonPeriod][action][a];
     }
   }
   
   // Q-러닝 업데이트 공식 적용
   float currentQ = qTable[luxLevel][timePeriod][seasonPeriod][ledSetting][action];
   float tdTarget = reward + DISCOUNT_FACTOR * maxNextQ;
   float tdError = tdTarget - currentQ;
   
   qTable[luxLevel][timePeriod][seasonPeriod][ledSetting][action] = 
       currentQ + LEARNING_RATE * tdError;
       
   Serial.print("Q-테이블 업데이트: [");
   Serial.print(luxLevel);
   Serial.print("][");
   Serial.print(timePeriod);
   Serial.print("][");
   Serial.print(seasonPeriod);
   Serial.print("][");
   Serial.print(ledSetting);
   Serial.print("][");
   Serial.print(action);
   Serial.print("] = ");
   Serial.println(qTable[luxLevel][timePeriod][seasonPeriod][ledSetting][action]);
 }
 
 /**
  * 환경 변화 감지 후 학습 파라미터 조정
  */
 void adjustLearningParameters() {
   // 학습률 증가 (더 빠른 적응)
   learningRate = 0.2;
   
   // 탐색률 증가 (더 많은 탐색)
   currentEpsilon = EPSILON_AFTER_CHANGE;
   
   Serial.println("환경 변화 감지: 학습 파라미터 조정됨");
   Serial.print("학습률 = ");
   Serial.print(learningRate);
   Serial.print(", 탐색률 = ");
   Serial.println(currentEpsilon);
 }