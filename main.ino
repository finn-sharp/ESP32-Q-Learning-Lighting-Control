/**
 * ESP32 Q-Learning 조도 제어 시스템
 * main.ino - 주 프로그램 파일
 */

 #include <Wire.h>
 #include "config.h"
 #include "sensors.h"
 #include "qlearning.h"
 #include "pattern_analysis.h"
 #include "storage.h"
 #include "utils.h"
 
 // 전역 변수
 unsigned long lastUpdateTime = 0;
 unsigned long lastSaveTime = 0;
 unsigned long lastPatternUpdateTime = 0;
 
 void setup() {
   // 시리얼 초기화
   Serial.begin(115200);
   Serial.println("ESP32 Q-Learning 조도 제어 시스템 시작");
   
   // 핀 설정
   pinMode(LED_PIN, OUTPUT);
   ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
   ledcAttachPin(LED_PIN, PWM_CHANNEL);
   ledcWrite(PWM_CHANNEL, currentPWM);
   
   // I2C 초기화
   Wire.begin(SDA_PIN, SCL_PIN);
   
   // 센서 초기화
   if (!initializeSensors()) {
     Serial.println("센서 초기화 실패!");
     while (1) { delay(1000); }  // 무한 루프
   }
   
   // 저장소 초기화
   if (!initializeStorage()) {
     Serial.println("저장소 초기화 실패! 새 Q-테이블 생성...");
     initializeQTable();
   } else {
     loadQTable();
     loadPatternData();
   }
   
   // 초기 시간 설정
   updateCurrentTime();
   
   // 패턴 분석 초기화
   initializePatternAnalysis();
   
   Serial.println("초기화 완료");
   delay(1000);  // 시스템 안정화
 }
 
 void loop() {
   unsigned long currentMillis = millis();
   
   // 현재 시간 업데이트 (1분마다)
   if (currentMillis - lastTimeUpdate >= TIME_UPDATE_INTERVAL) {
     updateCurrentTime();
     lastTimeUpdate = currentMillis;
     
     // 자정에 패턴 데이터 업데이트
     if (currentHour == 0 && currentMinute == 0) {
       updateDailyPattern();
       checkSeasonChange();
     }
   }
   
   // 조도 제어 업데이트 (30초마다)
   if (currentMillis - lastUpdateTime >= UPDATE_INTERVAL) {
     lastUpdateTime = currentMillis;
     
     // 현재 조도 측정
     float currentLux = measureLux();
     Serial.print("현재 조도: ");
     Serial.print(currentLux);
     Serial.println(" lux");
     
     // 현재 상태 결정
     int luxLevel = getLuxLevel(currentLux);
     int timePeriod = getTimePeriod();
     int seasonPeriod = getSeasonPeriod();
     
     Serial.print("상태: 조도 레벨=");
     Serial.print(luxLevel);
     Serial.print(", 시간대=");
     Serial.print(timePeriod);
     Serial.print(", 계절=");
     Serial.print(seasonPeriod);
     Serial.print(", LED 설정=");
     Serial.println(currentLEDSetting);
     
     // 행동 선택 (Q-learning)
     int action = selectAction(luxLevel, timePeriod, seasonPeriod, currentLEDSetting);
     
     // 행동 실행
     int newPWM = pwmValues[action];
     ledcWrite(PWM_CHANNEL, newPWM);
     
     Serial.print("선택 행동: ");
     Serial.print(action);
     Serial.print(" (PWM: ");
     Serial.print(newPWM);
     Serial.println(")");
     
     // 안정화 대기 (5초)
     delay(5000);
     
     // 새 조도 측정 및 새 상태 결정
     float newLux = measureLux();
     int newLuxLevel = getLuxLevel(newLux);
     
     Serial.print("새 조도: ");
     Serial.print(newLux);
     Serial.println(" lux");
     
     // 보상 계산
     float reward = calculateReward(newLux, newPWM);
     Serial.print("보상: ");
     Serial.println(reward);
     
     // 학습 모드일 때만 Q-테이블 업데이트
     if (learningMode) {
       updateQTable(luxLevel, timePeriod, seasonPeriod, currentLEDSetting, 
                   action, reward, newLuxLevel, timePeriod, seasonPeriod);
       
       // 탐색률 감소
       if (environmentChangeDetected) {
         // 환경 변화 감지 시 탐색률 리셋
         currentEpsilon = EPSILON_AFTER_CHANGE;
         environmentChangeDetected = false;
         Serial.println("환경 변화 후 탐색률 리셋");
       } else {
         // 일반적인 감소
         currentEpsilon = max(EPSILON_MIN, currentEpsilon * EPSILON_DECAY);
       }
       
       Serial.print("현재 탐색률: ");
       Serial.println(currentEpsilon);
       
       episodeCount++;
     }
     
     // 패턴 분석용 데이터 기록
     recordLuxData(currentHour, newLux, newPWM);
     
     // 환경 변화 감지
     if (detectEnvironmentChange()) {
       environmentChangeDetected = true;
       Serial.println("환경 변화 감지! 학습 파라미터 조정");
       adjustLearningParameters();
     }
     
     // 상태 업데이트
     currentLuxLevel = newLuxLevel;
     currentLEDSetting = action;
     currentPWM = newPWM;
   }
   
   // 데이터 저장 (10분마다)
   if (currentMillis - lastSaveTime >= SAVE_INTERVAL) {
     lastSaveTime = currentMillis;
     if (learningMode) {
       saveQTable();
       savePatternData();
       Serial.println("데이터 저장 완료");
     }
   }
   
   // 기타 시스템 태스크
   delay(100);  // CPU 부하 줄이기
 }