/**
 * ESP32 Q-Learning 조도 제어 시스템
 * config.h - 시스템 설정 및 상수
 */

 #ifndef CONFIG_H
 #define CONFIG_H
 
 // 하드웨어 핀 설정
 #define LED_PIN 16           // PWM으로 LED 제어할 핀
 #define SDA_PIN 21           // I2C SDA 핀
 #define SCL_PIN 22           // I2C SCL 핀
 #define STATUS_LED_PIN 17    // 상태 표시 LED 핀 (옵션)
 
 // PWM 설정
 #define PWM_FREQ 5000        // PWM 주파수 (Hz)
 #define PWM_CHANNEL 0        // PWM 채널
 #define PWM_RESOLUTION 8     // PWM 해상도 (8비트: 0-255)
 
 // BH1750 조도 센서 설정
 #define BH1750_ADDR 0x23     // BH1750 I2C 주소
 
 // DS3231 RTC 설정
 #define DS3231_ADDR 0x68     // DS3231 I2C 주소
 
 // 파일 시스템 설정
 #define SPIFFS_QTABLE_FILE "/qtable.dat"
 #define SPIFFS_PATTERN_FILE "/pattern.dat"
 
 // Q-Learning 파라미터
 #define LEARNING_RATE 0.1    // 학습률 (알파)
 #define DISCOUNT_FACTOR 0.95 // 할인 인자 (감마)
 #define EPSILON_START 0.9    // 초기 탐색률 (입실론)
 #define EPSILON_MIN 0.05     // 최소 탐색률
 #define EPSILON_DECAY 0.995  // 탐색률 감소 인자
 #define EPSILON_AFTER_CHANGE 0.3  // 환경 변화 감지 후 탐색률
 
 // 목표 조도 및 보상 파라미터
 #define TARGET_LUX 500.0     // 목표 조도 (lux)
 #define LUX_SCALING 10000.0  // 조도 차이 정규화 인자
 #define POWER_PENALTY 0.01   // 전력 소비 페널티 계수
 
 // 상태 공간 정의
 #define LUX_LEVELS 5         // 조도 레벨 수
 #define TIME_PERIODS 4       // 시간대 수
 #define SEASON_PERIODS 4     // 계절 수
 #define LED_SETTINGS 5       // LED 설정 수
 #define ACTIONS 5            // 가능한 행동 수
 
 // 조도 레벨 경계 (lux)
 const float luxBoundaries[LUX_LEVELS+1] = {0, 300, 450, 550, 700, 10000};
 
 // 시간대 경계 (24시간제)
 const int timeBoundaries[TIME_PERIODS+1] = {0, 6, 12, 18, 24};
 
 // 계절 경계 (월 기준)
 const int seasonBoundaries[SEASON_PERIODS] = {3, 6, 9, 12};  // 봄, 여름, 가을, 겨울 시작 월
 
 // LED PWM 설정 값 (0-255)
 const int pwmValues[LED_SETTINGS] = {0, 64, 128, 192, 255};
 
 // 시간 간격 설정
 #define UPDATE_INTERVAL 30000       // 조도 제어 업데이트 간격 (30초)
 #define SAVE_INTERVAL 600000        // 데이터 저장 간격 (10분)
 #define TIME_UPDATE_INTERVAL 60000  // 시간 업데이트 간격 (1분)
 
 // 패턴 분석 설정
 #define MAX_DAILY_PATTERNS 30       // 저장할 일일 패턴 최대 일수
 #define HOURS_PER_DAY 24            // 시간당 데이터 포인트
 #define ENV_CHANGE_THRESHOLD 100.0  // 환경 변화 감지 임계값 (lux)
 
 // 전역 변수 선언 (extern)
 extern int currentLuxLevel;
 extern int currentLEDSetting;
 extern int currentPWM;
 extern float currentEpsilon;
 extern bool learningMode;
 extern bool environmentChangeDetected;
 extern unsigned long episodeCount;
 extern int currentHour;
 extern int currentMinute;
 extern int currentDay;
 extern int currentMonth;
 extern unsigned long lastTimeUpdate;
 
 #endif  // CONFIG_H