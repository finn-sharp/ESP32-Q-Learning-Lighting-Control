/**
 * ESP32 Q-Learning 조도 제어 시스템
 * pattern_analysis.cpp - 패턴 분석 관련 함수 구현
 */

 #include "pattern_analysis.h"
 #include "config.h"
 #include "sensors.h"
 
 // 패턴 관련 변수
 DailyPattern dailyPatterns[MAX_DAILY_PATTERNS];
 float todayLux[HOURS_PER_DAY] = {0};
 int todayLED[HOURS_PER_DAY] = {0};
 int luxDataCount[HOURS_PER_DAY] = {0};
 bool environmentChangeDetected = false;
 
 /**
  * 패턴 분석 초기화
  */
 void initializePatternAnalysis() {
   Serial.println("패턴 분석 초기화 중...");
   
   // 오늘의 데이터 초기화
   for (int i = 0; i < HOURS_PER_DAY; i++) {
     todayLux[i] = 0;
     todayLED[i] = 0;
     luxDataCount[i] = 0;
   }
   
   // 패턴 배열 초기화 (이미 로드되지 않은 경우)
   for (int i = 0; i < MAX_DAILY_PATTERNS; i++) {
     dailyPatterns[i].month = 0;
     dailyPatterns[i].day = 0;
     
     for (int j = 0; j < HOURS_PER_DAY; j++) {
       dailyPatterns[i].hourlyLux[j] = 0;
       dailyPatterns[i].hourlyLED[j] = 0;
     }
   }
   
   Serial.println("패턴 분석 초기화 완료");
 }
 
 /**
  * 조도 데이터 기록 함수
  */
 void recordLuxData(int hour, float lux, int pwm) {
   if (hour < 0 || hour >= HOURS_PER_DAY) {
     return;  // 유효하지 않은 시간
   }
   
   // 새 데이터로 업데이트
   if (luxDataCount[hour] == 0) {
     todayLux[hour] = lux;
     todayLED[hour] = pwm;
   } else {
     // 누적 평균 계산
     todayLux[hour] = (todayLux[hour] * luxDataCount[hour] + lux) / (luxDataCount[hour] + 1);
     todayLED[hour] = (todayLED[hour] * luxDataCount[hour] + pwm) / (luxDataCount[hour] + 1);
   }
   
   luxDataCount[hour]++;
   
   Serial.print("시간 ");
   Serial.print(hour);
   Serial.print(" 데이터 기록: 조도 = ");
   Serial.print(todayLux[hour]);
   Serial.print(" lux, LED = ");
   Serial.println(todayLED[hour]);
 }
 
 /**
  * 일일 패턴 업데이트 (자정에 호출)
  */
 void updateDailyPattern() {
   Serial.println("일일 패턴 업데이트 중...");
   
   // 배열 한 칸씩 뒤로 이동 (가장 오래된 데이터 삭제)
   for (int i = MAX_DAILY_PATTERNS - 1; i > 0; i--) {
     dailyPatterns[i] = dailyPatterns[i-1];
   }
   
// 오늘 데이터 저장
dailyPatterns[0].month = currentMonth;
dailyPatterns[0].day = currentDay;

for (int i = 0; i < HOURS_PER_DAY; i++) {
  dailyPatterns[0].hourlyLux[i] = todayLux[i];
  dailyPatterns[0].hourlyLED[i] = todayLED[i];
}

// 오늘 데이터 초기화
for (int i = 0; i < HOURS_PER_DAY; i++) {
  todayLux[i] = 0;
  todayLED[i] = 0;
  luxDataCount[i] = 0;
}

Serial.println("일일 패턴 업데이트 완료");
}

/**
* 환경 변화 감지 함수
*/
bool detectEnvironmentChange() {
// 현재 시간에 대한 예상 자연광 확인
int hour = currentHour;

// 내역이 없는 경우 (초기 상태)
if (dailyPatterns[0].month == 0 || luxDataCount[hour] < 3) {
  return false;
}

// 이전 7일 간의 같은 시간대 평균 조도 계산
float expectedLux = 0;
int validDays = 0;

for (int i = 0; i < 7 && i < MAX_DAILY_PATTERNS; i++) {
  if (dailyPatterns[i].hourlyLux[hour] > 0) {
    expectedLux += dailyPatterns[i].hourlyLux[hour];
    validDays++;
  }
}

if (validDays == 0) {
  return false;
}

expectedLux /= validDays;

// 현재 조도와 예상 조도의 차이 계산
float currentLux = measureLux();
float luxDifference = abs(currentLux - expectedLux);

// 차이가 임계값보다 크면 환경 변화로 간주
if (luxDifference > ENV_CHANGE_THRESHOLD) {
  Serial.print("환경 변화 감지: 예상 조도 = ");
  Serial.print(expectedLux);
  Serial.print(" lux, 현재 조도 = ");
  Serial.print(currentLux);
  Serial.print(" lux, 차이 = ");
  Serial.print(luxDifference);
  Serial.println(" lux");
  return true;
}

return false;
}

/**
* 계절 변화 감지 함수
*/
void checkSeasonChange() {
// 최근 1일과 30일 전(또는 가능한 가장 오래된 날)의 월 비교
int currentMonth = dailyPatterns[0].month;
int oldestMonth = 0;

// 유효한 가장 오래된 패턴 찾기
for (int i = 1; i < MAX_DAILY_PATTERNS; i++) {
  if (dailyPatterns[i].month > 0) {
    oldestMonth = dailyPatterns[i].month;
  } else {
    break;  // 더 이상 유효한 패턴 없음
  }
}

// 데이터가 부족하면 리턴
if (oldestMonth == 0 || oldestMonth == currentMonth) {
  return;
}

// 계절 변화 감지 (월이 다른 계절 경계를 넘은 경우)
int currentSeason = getSeasonPeriod();
int oldestSeason = -1;

// 가장 오래된 월의 계절 계산
if (oldestMonth >= seasonBoundaries[0] && oldestMonth < seasonBoundaries[1]) {
  oldestSeason = 0;  // 봄
} else if (oldestMonth >= seasonBoundaries[1] && oldestMonth < seasonBoundaries[2]) {
  oldestSeason = 1;  // 여름
} else if (oldestMonth >= seasonBoundaries[2] && oldestMonth < seasonBoundaries[3]) {
  oldestSeason = 2;  // 가을
} else {
  oldestSeason = 3;  // 겨울
}

if (currentSeason != oldestSeason) {
  Serial.print("계절 변화 감지: ");
  Serial.print(oldestSeason);
  Serial.print(" -> ");
  Serial.println(currentSeason);
  
  // 환경 변화 플래그 설정 및 학습 파라미터 조정
  environmentChangeDetected = true;
  adjustLearningParameters();
}
}