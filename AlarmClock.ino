#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

#define BUFSIZE DS1302_RAMSIZE
char bufTime[20];

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args, BYTE);
#endif

uint8_t arrowBold[8] = {0x4, 0xe, 0x1f, 0x1b, 0x11, 0x0, 0x0};
uint8_t arrowUp[8] = {0x0, 0x0, 0x4, 0xa, 0x11, 0x0, 0x0};
uint8_t arrowDown[8] = {0x0, 0x0, 0x11, 0xa, 0x4, 0x0, 0x0};
uint8_t arrowLeft[8] = {0x0, 0x2, 0x4, 0x8, 0x4, 0x2, 0x0};
uint8_t arrowRight[8] = {0x0, 0x8, 0x4, 0x2, 0x4, 0x8, 0x0};

enum modes
{
  Clock,

  Clock_Settings,

  Game_Jump,
  Game_SpaceWarrior
};

modes mode = Clock;

int sensor = A3;
int temp;

int buzzer = 3;

bool backlight = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS1302 rtc;

String adapt(int i)
{
  String s;
  if (i < 10 && i >= 0)
    s = "0" + String(i);
  else
    s = String(i);
  return s;
}

bool selectClick = false;
bool leftClick = false;
bool rightClick = false;
bool clickable = true;
bool clickedS = false;
bool clickedL = false;
bool clickedR = false;

long touchDownTime;
bool pressed = false;
long timeDelay = 0;
bool delayed = true;

void setup()
{
  Serial.begin(115200);

  pinMode(buzzer, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(A0, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(0, arrowBold);
  lcd.createChar(1, arrowUp);
  lcd.createChar(2, arrowDown);
  lcd.createChar(3, arrowLeft);
  lcd.createChar(4, arrowRight);
  lcd.home();
  lcd.noBlink();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("00:00:00");
  lcd.setCursor(14, 0);
  lcd.print((String) char(223) + "C");
  rtc.begin();

  load();

  // DateTime dateTimeCurrent = DateTime(__DATE__, __TIME__);
  // Serial.print(__TIME__);
  // Serial.println(__DATE__);
  // dateTimeCurrent.setsecond(dateTimeCurrent.second() + 12);
  // rtc.adjust(dateTimeCurrent);
}

void loop()
{
  int sensorVal1 = digitalRead(10);
  int sensorVal2 = digitalRead(9);
  int sensorVal3 = digitalRead(8);
  if (clickable)
  {
    selectClick = sensorVal1 == LOW;
    leftClick = sensorVal2 == LOW;
    rightClick = sensorVal3 == LOW;
  }
  else
  {
    selectClick = false;
    leftClick = false;
    rightClick = false;
  }

  if (timeDelay <= millis() && delayed == false)
    delayed = true;

  switch (mode)
  {
  case Clock:
    clock();
    break;

  case Clock_Settings:
    clockSetting();
    break;

  case Game_Jump:
    jump();
    break;
  case Game_SpaceWarrior:
    spaceWarrior();
    break;
  }
}

float alarmTime[6][4];
bool alarmDays[6][8];
byte alarmMode[6];
bool alarm = false;
byte alarmsAmount = 0;
long lastSec;
long lastMil;
int lastDay;
int timerSec = 0;
bool noticeVisible = false;

void initClock()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("00:00:00");
  lcd.setCursor(14, 0);
  lcd.print((String) char(223) + "C");
  mode = Clock;
}

void clock()
{
  DateTime now = rtc.now();
  if (lastSec != now.second())
  {
    if (delayed)
      clickable = true;
    lastSec = now.second();
    // Serial.println(now.tostr(bufTime));

    lcd.setCursor(0, 0);
    lcd.print(adapt(now.hour()));

    lcd.setCursor(3, 0);
    lcd.print(adapt(now.minute()));

    lcd.setCursor(6, 0);
    lcd.print(adapt(now.second()));

    temp = ((analogRead(sensor) * 5.0) / 1024.0 * 100.0) - 3;
    lcd.setCursor(12, 0);
    lcd.print(temp);

    lcd.setCursor(0, 1);
    lcd.print((String)now.day() + "/" + now.month() + "/" + now.year());

    if (!alarm)
    {
      timerSec = 0;
      for (int i = 0; alarmsAmount > i; i++)
      {
        bool today = false;
        bool without = false;
        int dayOfWeek = now.dayOfWeek() - 1;
        if (dayOfWeek == -1)
          dayOfWeek = 6;
        if (alarmDays[i][dayOfWeek])
          today = true;
        else
        {
          without = true;
          today = true;
          for (int t = 0; 7 > t; t++)
            if (alarmDays[i][t])
            {
              without = false;
              today = false;
            }
        }
        if (today && alarmTime[i][0] == now.hour() && alarmTime[i][1] == now.minute())
        {
          if (alarmMode[i] == 0)
            alarm = true;
          alarmMode[i] = 1;

          Serial.print("Alarm: ");
          Serial.print(i);
          Serial.print(" without: ");
          Serial.print(without);
          Serial.print(" time: ");
          Serial.print(alarmTime[i][0]);
          Serial.print(":");
          Serial.print(alarmTime[i][1]);
          Serial.print(" day: ");
          Serial.println(dayOfWeek);

          if (without)
            removeAlarm(i);
          lcd.backlight();
          backlight = true;
        }
      }
    }
    else
    {
      timerSec++;
      if (timerSec >= 420)
      {
        alarm = false;
        lcd.setCursor(12, 1);
        lcd.print("    ");
        noTone(buzzer);
      }
    }
  }
  if (millis() >= lastMil && alarm)
  {
    lastMil = millis() + 500;
    tone(buzzer, 400, 400);
    if (!noticeVisible)
    {
      lcd.setCursor(12, 1);
      lcd.print("!!!!");
      noticeVisible = true;
    }
    else
    {
      lcd.setCursor(12, 1);
      lcd.print("    ");
      noticeVisible = false;
    }
  }
  if (lastDay != now.day())
  {
    Serial.println("refresh");
    lastDay = now.day();
    for (int i = 0; i < alarmsAmount; i++)
      alarmMode[i] = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("00:00:00");
    lcd.setCursor(14, 0);
    lcd.print((String) char(223) + "C");
  }

  if (!alarm)
  {
    if (selectClick)
    {
      if (!clickedS)
        touchDownTime = millis();
      clickedS = true;
      if ((millis() - touchDownTime) >= 500 && !pressed)
      {
        if (backlight)
        {
          lcd.noBacklight();
          backlight = false;
        }
        else
        {
          lcd.backlight();
          backlight = true;
        }
        pressed = true;
      }
    }
    else if (clickedS)
    {
      if (pressed)
      {
        clickedS = false;
        clickable = false;
        timeDelay = (millis() + 100);
        delayed = false;
        pressed = false;
      }
      else
      {
        if (backlight)
        {
          lcd.clear();
          mode = Clock_Settings;
        }
        else
        {
          lcd.backlight();
          backlight = true;
        }
        clickedS = false;
      }
    }

    if (leftClick && rightClick)
    {
      if (!clickedL || !clickedR)
        touchDownTime = millis();
      clickedL = true;
      clickedR = true;
      if ((millis() - touchDownTime) >= 1000 && !pressed)
      {
        now.setminute(now.minute() + 1);
        rtc.adjust(now);
        pressed = true;
      }
    }
    else if (clickedL && clickedR)
    {
      if (pressed)
      {
        clickable = false;
        timeDelay = (millis() + 200);
        delayed = false;
        clickedL = false;
        clickedR = false;
        pressed = false;
      }
      else
      {
        clickedL = false;
        clickedR = false;
      }
    }
    else if (leftClick)
      clickedL = true;
    else if (rightClick)
      clickedR = true;
    else if (clickedL)
    {
      if (backlight)
        initSpaceWarrior();
      else
      {
        lcd.backlight();
        backlight = true;
      }
      clickedL = false;
    }
    else if (clickedR)
    {
      if (backlight)
        initJump();
      else
      {
        lcd.backlight();
        backlight = true;
      }
      clickedR = false;
    }
  }
  else
  {
    if (selectClick)
      clickedS = true;
    else if (clickedS)
    {
      alarm = false;
      noTone(buzzer);
      lcd.setCursor(12, 1);
      lcd.print("    ");
      clickedS = false;
      clickable = false;
      timeDelay = (millis() + 700);
      delayed = false;
    }
  }

  delay(10);
}