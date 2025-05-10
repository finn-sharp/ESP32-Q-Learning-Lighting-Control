# ESP32 강화학습 기반 지능형 조도 제어 시스템

## 목차
1. [프로젝트 개요](#프로젝트-개요)
2. [하드웨어 요구사항](#하드웨어-요구사항)
3. [소프트웨어 요구사항](#소프트웨어-요구사항)
4. [하드웨어 설정](#하드웨어-설정)
5. [소프트웨어 설치](#소프트웨어-설치)
6. [시스템 초기화 및 설정](#시스템-초기화-및-설정)
7. [작동 원리](#작동-원리)
8. [프로젝트 구조](#프로젝트-구조)
9. [트러블슈팅](#트러블슈팅)
10. [확장 가능성](#확장-가능성)

## 프로젝트 개요

이 프로젝트는 ESP32 마이크로컨트롤러와 Q-Learning 강화학습 알고리즘을 활용하여 실내 조도를 자동으로 제어하는 지능형 시스템입니다. 시스템은 목표 조도를 유지하면서 에너지 효율성을 극대화하고 계절 변화, 시간대별 자연광 패턴 등 환경 변화에 자동으로 적응합니다.

주요 특징:
- **강화학습 기반 제어**: Q-Learning을 활용한 자가 학습 시스템
- **계절 및 시간 패턴 학습**: 계절 변화와 일일 패턴 인식 및 적응
- **에너지 효율 최적화**: 필요한 최소 조명만 사용하여 에너지 절약
- **환경 변화 감지 및 적응**: 갑작스러운 환경 변화 감지 및 빠른 적응

## 하드웨어 요구사항

### 필수 부품
1. **ESP32 개발 보드** - ESP32-WROOM 또는 ESP32-WROVER 시리즈 (최소 4MB 플래시 메모리)
2. **BH1750 조도 센서** - I2C 인터페이스
3. **DS3231 RTC 모듈** - 정확한 시간 및 날짜 추적용
4. **LED** - PWM 제어 가능한 LED 또는 LED 스트립
5. **MOSFET 또는 트랜지스터** - LED 전력 제어 (예: IRF540N, 2N2222)
6. **저항** - 10kΩ (풀업용), 220Ω (LED 제한용)
7. **브레드보드 및 점퍼 와이어**
8. **5V 전원 공급 장치**

### 선택적 부품
1. **상태 표시 LED** - 시스템 상태 표시용
2. **SD 카드 모듈** - 더 많은 데이터 저장용 (선택 사항)
3. **OLED 디스플레이** - 시스템 상태 표시용 (선택 사항)

### 부품 구매 링크
- [ESP32 개발 보드](https://www.aliexpress.com/item/1005004867391145.html)
- [BH1750 조도 센서](https://www.aliexpress.com/item/32341898423.html)
- [DS3231 RTC 모듈](https://www.aliexpress.com/item/1005003380622486.html)
- [MOSFET 모듈](https://www.aliexpress.com/item/32993228739.html)

## 소프트웨어 요구사항

### 필수 소프트웨어
1. **Arduino IDE** (버전 1.8.19 이상)
   - [다운로드 링크](https://www.arduino.cc/en/software)

2. **ESP32 보드 패키지**
   - [설치 방법](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)

### 필수 라이브러리
아래 라이브러리들을 Arduino IDE의 라이브러리 관리자에서 설치하세요:

1. **BH1750** (by Christopher Laws)
   - 버전: 1.3.0 이상
   - BH1750 조도 센서 제어

2. **RTClib** (by Adafruit)
   - 버전: 2.1.1 이상
   - DS3231 RTC 모듈 제어

3. **SPIFFS** (ESP32 코어에 포함됨)
   - ESP32 내부 파일 시스템 사용

## 하드웨어 설정

### 배선도
아래 핀 연결을 따라 하드웨어를 구성하세요:

| 부품          | ESP32 핀    | 설명                   |
|--------------|------------|------------------------|
| BH1750 VCC   | 3.3V       | 전원                   |
| BH1750 GND   | GND        | 접지                   |
| BH1750 SDA   | GPIO21     | I2C 데이터 라인         |
| BH1750 SCL   | GPIO22     | I2C 클럭 라인           |
| DS3231 VCC   | 3.3V       | 전원                   |
| DS3231 GND   | GND        | 접지                   |
| DS3231 SDA   | GPIO21     | I2C 데이터 라인 (공유)   |
| DS3231 SCL   | GPIO22     | I2C 클럭 라인 (공유)     |
| MOSFET Gate  | GPIO16     | PWM 제어 신호           |
| MOSFET Drain | LED-       | LED 제어                |
| MOSFET Source| GND        | 접지                   |
| LED+         | 5V         | LED 전원                |
| 상태 LED+     | GPIO17     | 상태 표시 LED (선택사항) |
| 상태 LED-     | GND (저항 통해) | 접지                |

### 배선 다이어그램

ESP32 ──── 3.3V ────┬──── BH1750 VCC  
│                └──── DS3231 VCC  
│  
├──── GND ─────┬──── BH1750 GND  
│              ├──── DS3231 GND  
│              └──── MOSFET Source  
│  
├──── GPIO21 ──┬──── BH1750 SDA  
│              └──── DS3231 SDA  
│  
├──── GPIO22 ──┬──── BH1750 SCL  
│              └──── DS3231 SCL  
│  
├──── GPIO16 ───── MOSFET Gate  
│  
└──── GPIO17 ───── 상태 LED+ ─── 저항 ─── GND  
5V ─────────────── LED+  
MOSFET Drain ───── LED-  

## 소프트웨어 설치

### Arduino IDE 설정
1. **ESP32 보드 패키지 설치**
   - Arduino IDE 메뉴에서 `File` > `Preferences` 선택
   - "Additional Board Manager URLs" 필드에 다음 URL 추가:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - OK 클릭
   - `Tools` > `Board` > `Boards Manager...` 선택
   - "esp32"를 검색하고 "ESP32 by Espressif Systems" 설치

2. **필요한 라이브러리 설치**
   - `Tools` > `Manage Libraries...` 선택
   - BH1750 검색 후 "BH1750" by Christopher Laws 설치
   - RTClib 검색 후 "RTClib" by Adafruit 설치

### 프로젝트 파일 구성
1. Arduino IDE에서 새 스케치 생성
2. 아래 파일들을 생성하고 코드를 복사하세요:
   - `main.ino`
   - `config.h`
   - `sensors.h` & `sensors.cpp`
   - `qlearning.h` & `qlearning.cpp`
   - `pattern_analysis.h` & `pattern_analysis.cpp`
   - `storage.h` & `storage.cpp`
   - `utils.h` & `utils.cpp`

모든 파일이 같은 디렉토리(스케치 폴더)에 위치해야 합니다.

### 코드 컴파일 및 업로드
1. Arduino IDE에서 ESP32 보드 선택:
   - `Tools` > `Board` > `ESP32 Arduino` > `ESP32 Dev Module`

2. 적절한 플래시 모드 및 속도 설정:
   - Flash Mode: "QIO"
   - Flash Frequency: "80MHz"
   - Flash Size: "4MB (32Mb)"
   - Partition Scheme: "Default 4MB with spiffs"
   - PSRAM: "Disabled"

3. 포트 선택:
   - `Tools` > `Port` > 연결된 ESP32의 COM 포트 선택

4. 컴파일 버튼(체크 표시) 클릭하여 검증

5. 업로드 버튼(화살표) 클릭하여 ESP32에 코드 업로드

## 시스템 초기화 및 설정

### 초기 설정
1. **시간 설정**
   - 코드가 업로드되면 RTC 모듈은 컴파일 시간으로 자동 설정됩니다
   - 더 정확한 시간 설정이 필요하면 코드의 다음 부분을 수정하세요:
     ```cpp
     if (rtc.lostPower()) {
       Serial.println("RTC 시간 재설정 (컴파일 시간 기준)");
       rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
     }
     ```
   - 특정 시간으로 설정하려면:
     ```cpp
     rtc.adjust(DateTime(2023, 10, 15, 12, 30, 0)); // 년, 월, 일, 시, 분, 초
     ```

2. **목표 조도 설정**
   - `config.h` 파일에서 TARGET_LUX 값을 원하는 조도 수준으로 조정하세요:
     ```cpp
     #define TARGET_LUX 500.0  // 목표 조도 (lux)
     ```

3. **학습 파라미터 조정 (선택사항)**
   - 학습 특성을 조정하려면 `config.h` 파일의 다음 값을 수정하세요:
     ```cpp
     #define LEARNING_RATE 0.1    // 학습률 (높을수록 빠른 학습)
     #define DISCOUNT_FACTOR 0.95 // 할인 인자 (미래 보상 중요도)
     #define EPSILON_START 0.9    // 초기 탐색률
     #define EPSILON_MIN 0.05     // 최소 탐색률
     #define EPSILON_DECAY 0.995  // 탐색률 감소 속도
     ```

### 작동 확인
1. **시리얼 모니터 열기**
   - `Tools` > `Serial Monitor` 선택
   - 속도를 115200 baud로 설정

2. **초기화 메시지 확인**
   - ESP32가 재시작되며 다음과 같은 초기화 메시지가 표시됩니다:
     ```
     ESP32 Q-Learning 조도 제어 시스템 시작
     BH1750 조도 센서 초기화 성공
     DS3231 RTC 초기화 성공
     현재 시간: 12:30 날짜: 15/10
     SPIFFS 마운트 성공
     Q-테이블 초기화 중...
     Q-테이블 초기화 완료
     패턴 분석 초기화 중...
     패턴 분석 초기화 완료
     초기화 완료
     ```

3. **시스템 작동 확인**
   - 시스템은 30초마다 조도 측정 및 제어 업데이트를 수행합니다:
     ```
     현재 조도: 320.50 lux
     상태: 조도 레벨=1, 시간대=2, 계절=2, LED 설정=2
     선택 행동: 3 (PWM: 192)
     새 조도: 480.25 lux
     보상: -0.0456
     ```

## 작동 원리

### Q-Learning 개요
이 시스템은 강화학습의 일종인 Q-Learning 알고리즘을 사용합니다:

1. **상태 공간**: 
   - 조도 레벨 (5단계)
   - 시간대 (4단계: 아침, 낮, 저녁, 밤)
   - 계절 (4단계: 봄, 여름, 가을, 겨울)
   - 현재 LED 설정 (5단계)

2. **행동 공간**:
   - LED PWM 설정 변경 (5단계: 0%, 25%, 50%, 75%, 100%)

3. **보상 함수**:
   - 목표 조도와의 근접성에 대한 보상
   - 전력 소비에 대한 패널티

4. **학습 과정**:
   - 시스템은 초기에 다양한 행동을 탐색하며 경험을 수집
   - 시간이 지남에 따라 탐색 확률이 감소하고 학습된 정책을 더 많이 활용
   - Q-테이블은 이전 경험에 기반하여 지속적으로 업데이트

### 계절 및 시간 패턴 학습
시스템은 일별 조도 패턴을 기록하고 분석하여 자연광 변화에 적응합니다:

1. **일일 패턴 기록**:
   - 매 시간 평균 조도와 LED 설정 저장
   - 자정에 일일 패턴 업데이트

2. **계절 변화 감지**:
   - 월 정보를 기반으로 계절 변화 감지
   - 계절 변화 시 학습 파라미터 조정

3. **환경 변화 감지**:
   - 예상 조도와 실제 조도의 차이 모니터링
   - 차이가 임계값을 초과하면 환경 변화로 간주하고 적응 모드 활성화

## 프로젝트 구조

### 파일 구조
- **main.ino**: 주 프로그램 파일, 초기화 및 메인 루프 포함
- **config.h**: 시스템 설정 및 상수 정의
- **sensors.h/cpp**: 센서 관련 함수 (조도 측정, 시간 획득 등)
- **qlearning.h/cpp**: Q-Learning 알고리즘 구현
- **pattern_analysis.h/cpp**: 시간/계절 패턴 분석 기능
- **storage.h/cpp**: 데이터 저장 관련 함수 (Q-테이블, 패턴 데이터 등)
- **utils.h/cpp**: 유틸리티 함수 (상태 출력, LED 점멸 등)

### 메모리 사용량
- Q-테이블: 약 2KB (5×4×4×5×5×4 바이트)
- 패턴 데이터: 약 3KB (30일 × 24시간 × 5 바이트)
- 코드: 약 250KB
- 총 메모리 요구량: 약 300KB (4MB 플래시 충분)

## 트러블슈팅

### 일반적인 문제

#### 센서 초기화 실패
**증상**: "BH1750 조도 센서 초기화 실패" 또는 "DS3231 RTC 초기화 실패" 메시지
**해결 방법**:
1. I2C 연결 확인 (SDA, SCL 핀)
2. 전원 연결 확인 (3.3V, GND)
3. I2C 주소 확인 (`config.h`의 BH1750_ADDR 및 DS3231_ADDR)
4. I2C 풀업 저항 확인 (10kΩ 저항이 필요할 수 있음)

#### SPIFFS 초기화 실패
**증상**: "SPIFFS 마운트 실패!" 메시지
**해결 방법**:
1. ESP32 플래시 메모리 포맷:
   ```cpp
   SPIFFS.format();
파티션 스킴 확인 ("Default 4MB with spiffs" 사용)
ESP32를 재부팅한 후 다시 시도

LED 제어 문제
증상: LED가 응답하지 않거나 예상대로 작동하지 않음
해결 방법:

MOSFET/트랜지스터 연결 확인
PWM 핀 설정 확인 (config.h의 LED_PIN)
다른 GPIO 핀으로 시도 (PWM 지원하는 핀인지 확인)
LED 작동 테스트를 위한 간단한 코드 실행:
cppfor (int i = 0; i < 255; i++) {
  ledcWrite(PWM_CHANNEL, i);
  delay(10);
}


비정상적인 조도 측정
증상: 조도 센서가 0 또는 이상한 값을 반환
해결 방법:

센서 위치 확인 (직사광선이나 그림자를 피해야 함)
센서 청소 (먼지나 얼룩이 측정에 영향을 줄 수 있음)
조도 변환 모드 변경:
cpplightMeter.begin(BH1750::ONE_TIME_HIGH_RES_MODE);


고급 문제
메모리 부족
증상: 업로드 중 "Not enough memory" 오류 또는 불규칙한 재부팅
해결 방법:

상태 공간 축소 (LUX_LEVELS, TIME_PERIODS 등 줄이기)
PSRAM 활성화 (지원하는 ESP32 모델의 경우)
불필요한 기능 비활성화

학습이 느리거나 효과가 없음
증상: 시스템이 목표 조도에 수렴하지 않거나 학습 속도가 느림
해결 방법:

학습 파라미터 조정:
cpp#define LEARNING_RATE 0.2    // 더 빠른 학습을 위해 증가
#define EPSILON_DECAY 0.99   // 더 오래 탐색하도록 증가

보상 함수 조정:
cpp#define LUX_SCALING 5000.0   // 더 민감한 조도 보상
#define POWER_PENALTY 0.005  // 전력 패널티 감소

Q-테이블 초기화 및 재학습

확장 가능성
기능 확장

웹 인터페이스 추가

ESP32의 Wi-Fi 기능을 활용하여 웹 대시보드 구현
시스템 모니터링 및 원격 설정 가능


추가 센서 통합

PIR 센서: 재실 감지
온습도 센서: 환경 조건 모니터링
외부 조도 센서: 자연광 직접 측정


다중 구역 제어

여러 조도 센서와 LED 그룹 제어
구역별 목표 조도 설정



알고리즘 개선

심층 강화학습

DQN(Deep Q-Network)으로 확장
더 복잡한 패턴 인식 가능


예측 기반 제어

시간 패턴 예측을 통한 선제적 조명 조정
날씨 API 연동을 통한 자연광 예측


사용자 피드백 통합

사용자 선호도 학습
사용자 패턴에 따른 맞춤형 조명 제어




이 프로젝트는 ESP32와 강화학습을 활용한 지능형 제어 시스템의 기초를 제공합니다. 필요에 따라 확장하고 조정하여 다양한 환경 제어 문제에 적용할 수 있습니다.
더 자세한 정보나 지원이 필요하면 GitHub 이슈를 통해 문의하세요.
