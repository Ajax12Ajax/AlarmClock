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

bool spaceWarriorStarted = false;
bool jumpStarted = false;
bool clockStarted = true;

int sensor = A3;
int temp;

int buzzer = 2;

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

long delayL = 0;
bool delayed = true;

float alarmTime[6][4];
bool alarmDays[6][8];

void setup()
{
  lcd.init();
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
  // rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop()
{
  int sensorVal1 = digitalRead(9);
  int sensorVal2 = digitalRead(10);
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

  if (delayL <= millis() && delayed == false)
    delayed = true;

  if (clockStarted)
    clock();
  if (spaceWarriorStarted || jumpStarted)
    game();
}

bool alarm = false;
bool settings = false;
bool selected = false;
bool chosenTime = false;
bool chosenDate = false;
long lastSec;
long last;
long lastMil;
int timerSec = 0;
int amt = 0;
int nowV = 0;
int nowH = 0;
bool visable = false;
bool alarmed = false;

int zellersCongruence(int day, int month, int year)
{
  if (month < 3)
  {
    month += 12;
    year -= 1;
  }
  int K = year % 100;
  int J = year / 100;
  int h = (day + (13 * (month + 1)) / 5 + K + K / 4 + J / 4 - 2 * J) % 7;
  return (h + 5) % 7;
}

void clock()
{
  if (!settings)
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

      temp = (analogRead(sensor) * 5.0) / 1024.0 * 100.0;
      lcd.setCursor(12, 0);
      lcd.print(temp);

      lcd.setCursor(0, 1);
      lcd.print((String)now.day() + "/" + now.month() + "/" + now.year());

      if (!alarm)
      {
        for (int i = 0; amt > i; i++)
        {
          bool today = false;
          bool without = false;
          Serial.println("--------------");
          Serial.println(i);
          Serial.print("day: ");
          Serial.println(zellersCongruence(now.day(), now.month(), now.year()));
          if (alarmDays[i][zellersCongruence(now.day(), now.month(), now.year())])
          {
            today = true;
            Serial.println("pland");
          }
          else
          {
            without = true;
            today = true;
            for (int t = 0; 7 > t; t++)
            {
              if (alarmDays[i][t])
              {
                without = false;
                today = false;
              }
            }
          }
          Serial.print("today: ");
          Serial.println(today);
          Serial.print("Time: ");
          Serial.print(alarmTime[i][0]);
          Serial.print(":");
          Serial.println(alarmTime[i][1]);
          if (today && alarmTime[i][0] == now.hour() && alarmTime[i][1] == now.minute())
          {
            if (!alarmed)
              alarm = true;
            alarmed = true;
            Serial.println("Ring!!!!!");
            if (without)
            {
              for (int t = i; t < 6 - 1; t++)
              {
                for (int j = 0; j < 4; j++)
                  alarmTime[t][j] = alarmTime[t + 1][j];
                for (int j = 0; j < 8; j++)
                  alarmDays[t][j] = alarmDays[t + 1][j];
              }
              for (int j = 0; j < 4; j++)
                alarmTime[amt - 1][j] = 0;
              for (int j = 0; j < 8; j++)
                alarmDays[amt - 1][j] = 0;
              amt--;
              save();
            }
          }
          else
          {
            alarmed = false;
          }
        }
      }
      else
      {
        timerSec++;
        if (timerSec >= 600)
        {
          alarm = false;
          lcd.setCursor(12, 1);
          lcd.print("    ");
          noTone(buzzer);
        }
      }
    }

    if (!alarm)
    {
      if (selectClick)
      {
        if (!clickedS)
          last = millis();
        clickedS = true;
      }
      else if ((millis() - last) >= 2000 && clickedS)
      {
        lcd.clear();
        settings = true;
        clickedS = false;
        clickable = false;
        delayL = (millis() + 1000);
        delayed = false;
      }
      else if (clickedS)
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
        clickedS = false;
      }

      if (rightClick)
        jumpStarted = true;

      if (leftClick)
        spaceWarriorStarted = true;
    }
    else
    {
      if (millis() >= lastMil)
      {
        lastMil = millis() + 500;
        tone(buzzer, 400, 400);
        if (!visable)
        {
          lcd.setCursor(12, 1);
          lcd.print("!!!!");
          visable = true;
        }
        else
        {
          lcd.setCursor(12, 1);
          lcd.print("    ");
          visable = false;
        }
      }

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
        delayL = (millis() + 1000);
        delayed = false;
      }
    }
  }
  else if (settings)
  {
    alarm = false;
    if (!selected)
    {
      if (delayed)
        clickable = true;

      lcd.setCursor(15, 0);
      lcd.printByte(1);
      lcd.setCursor(15, 1);
      lcd.printByte(2);

      for (int i = 0; 1 >= i; i++)
        if (amt > nowV + i)
        {
          if (i == 0)
          {
            lcd.setCursor(14, 0);
            lcd.print("<");
          }
          lcd.setCursor(0, i);
          lcd.print(adapt(alarmTime[nowV + i][0]) + ":" + adapt(alarmTime[nowV + i][1]) + " " +
                    adaptDays(nowV + i, 0) + adaptDays(nowV + i, 1) + adaptDays(nowV + i, 2) + adaptDays(nowV + i, 3) + adaptDays(nowV + i, 4) + adaptDays(nowV + i, 5) + adaptDays(nowV + i, 6));
        }
        else if (amt == nowV + i && amt < 6)
        {
          if (i == 0)
          {
            lcd.setCursor(5, 0);
            lcd.print("<");
          }
          lcd.setCursor(0, i);
          lcd.print("new+");
        }

      if (selectClick)
      {
        if (!clickedS)
          last = millis();
        clickedS = true;
      }
      else if ((millis() - last) >= 2000 && clickedS)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("00:00:00");
        lcd.setCursor(14, 0);
        lcd.print((String) char(223) + "C");
        nowV = 0;
        settings = false;
        clickedS = false;
        clickable = false;
        delayL = (millis() + 1000);
        delayed = false;
      }
      else if (clickedS)
      {
        if (amt <= nowV)
        {
          amt++;
          save();
        }
        lcd.clear();
        selected = true;
        clickedS = false;
        clickable = false;
        delayL = (millis() + 1000);
        delayed = false;
      }

      if (leftClick && rightClick)
      {
        if (!clickedL && !clickedR)
          last = millis();
        clickedL = true;
        clickedR = true;
      }
      else if ((millis() - last) >= 2000 && clickedL && clickedR)
      {
        if (amt > nowV)
        {
          for (int i = nowV; i < 6 - 1; i++)
          {
            for (int j = 0; j < 4; j++)
              alarmTime[i][j] = alarmTime[i + 1][j];
            for (int j = 0; j < 8; j++)
              alarmDays[i][j] = alarmDays[i + 1][j];
          }
          for (int j = 0; j < 4; j++)
            alarmTime[amt - 1][j] = 0;
          for (int j = 0; j < 8; j++)
            alarmDays[amt - 1][j] = 0;
          amt--;
          lcd.clear();
          save();
          clickable = false;
          delayL = (millis() + 1000);
          delayed = false;
        }
        clickedL = false;
        clickedR = false;
      }
      else if (leftClick)
        clickedL = true;
      else if (rightClick)
        clickedR = true;
      else if (clickedL)
      {
        if (nowV < amt && nowV < 5)
          nowV++;
        lcd.clear();
        clickedL = false;
      }
      else if (clickedR)
      {
        if (nowV > 0)
          nowV--;
        lcd.clear();
        clickedR = false;
      }
    }
    else
    {
      if (delayed)
        clickable = true;

      lcd.setCursor(0, 0);
      lcd.print(adapt(alarmTime[nowV][0]) + ":" + adapt(alarmTime[nowV][1]) + " " +
                adaptDays(nowV, 0) + adaptDays(nowV, 1) + adaptDays(nowV, 2) + adaptDays(nowV, 3) + adaptDays(nowV, 4) + adaptDays(nowV, 5) + adaptDays(nowV, 6));
      lcd.setCursor(nowH, 1);

      if (!chosenDate && !chosenTime)
      {
        if ((nowH >= 0 && nowH <= 1) || (nowH >= 3 && nowH <= 4))
          lcd.print("^^");
        else if (nowH >= 6)
          lcd.print("^");
        lcd.setCursor(14, 1);
        lcd.printByte(3);
        lcd.setCursor(15, 1);
        lcd.printByte(4);

        if (leftClick)
          clickedL = true;
        else if (clickedL)
        {
          if (nowH == 6)
            nowH = 3;
          else if (nowH == 3)
            nowH = 0;
          else if (nowH > 6)
            nowH--;
          lcd.clear();
          clickedL = false;
        }

        if (rightClick)
          clickedR = true;
        else if (clickedR)
        {
          if (nowH == 0)
            nowH = 3;
          else if (nowH == 3)
            nowH = 6;
          else if (nowH < 12)
            nowH++;
          lcd.clear();
          clickedR = false;
        }

        if (selectClick)
        {
          if (!clickedS)
            last = millis();
          clickedS = true;
        }
        else if ((millis() - last) >= 2000 && clickedS)
        {
          lcd.clear();
          nowH = 0;
          selected = false;
          clickedS = false;
          clickable = false;
          delayL = (millis() + 1000);
          delayed = false;
        }
        else if (clickedS)
        {
          if (nowH >= 0 && nowH <= 4)
            chosenTime = true;
          else if (nowH >= 6)
            chosenDate = true;
          lcd.clear();
          clickedS = false;
        }
      }
      if (chosenTime)
      {
        lcd.printByte(0);
        lcd.setCursor(nowH + 1, 1);
        lcd.printByte(0);
        lcd.setCursor(15, 0);
        lcd.printByte(1);
        lcd.setCursor(15, 1);
        lcd.printByte(2);

        if (leftClick)
          clickedL = true;
        else if (clickedL)
        {
          if (nowH >= 0 && nowH <= 1 && alarmTime[nowV][0] < 24.0f)
            alarmTime[nowV][0]++;
          else if (nowH >= 3 && nowH <= 4 && alarmTime[nowV][1] < 60.0f)
            alarmTime[nowV][1] += 5.0f;
          save();
          clickedL = false;
        }

        if (rightClick)
          clickedR = true;
        else if (clickedR)
        {
          if (nowH >= 0 && nowH <= 1 && alarmTime[nowV][0] > 0.0f)
            alarmTime[nowV][0]--;
          else if (nowH >= 3 && nowH <= 4 && alarmTime[nowV][1] > 0.0f)
            alarmTime[nowV][1] -= 5.0f;
          save();
          clickedR = false;
        }

        if (selectClick)
          clickedS = true;
        else if (clickedS)
        {
          chosenTime = false;
          save();
          lcd.clear();
          clickedS = false;
        }
      }
      if (chosenDate)
      {
        lcd.printByte(0);

        if (leftClick)
          clickedL = true;
        else if (clickedL)
        {
          alarmDays[nowV][nowH - 6] = true;
          save();
          clickedL = false;
        }

        if (rightClick)
          clickedR = true;
        else if (clickedR)
        {
          alarmDays[nowV][nowH - 6] = false;
          save();
          clickedR = false;
        }

        if (selectClick)
          clickedS = true;
        else if (clickedS)
        {
          chosenDate = false;
          save();
          lcd.clear();
          clickedS = false;
        }
      }
    }
  }
  else if (alarm)
  {
  }
  delay(10);
}

char data[BUFSIZE];
void printArray(const byte *ptr, int len)
{
  Serial.print("[");
  for (int i = 0; i < len - 1; i++)
  {
    Serial.print(*(ptr + i), DEC);
    Serial.print(", ");
  }
  Serial.print(*(ptr + len - 1), DEC);
  Serial.println("]");
}

void save()
{
  data[0] = amt;
  int now = 1;
  for (int i = 0; i < 6; i++)
  {
    data[now] = alarmTime[i][0];
    now++;
    data[now] = alarmTime[i][1];
    now++;
  }
  for (int i = 0; i < 6; i++)
  {
    data[now] = alarmDays[i][0];
    now++;
    data[now] = putDays(alarmDays[i][1], alarmDays[i][2], alarmDays[i][3]);
    now++;
    data[now] = putDays(alarmDays[i][4], alarmDays[i][5], alarmDays[i][6]);
    now++;
  }
  Serial.println("Setting new data to RAM:");
  printArray(data, BUFSIZE);
  rtc.putram(data, BUFSIZE);
}
byte putDays(bool b1, bool b2, bool b3)
{
  byte by = 0;
  if (b1 && !b2 && !b3)
    by = 1;
  if (!b1 && b2 && !b3)
    by = 2;
  if (!b1 && !b2 && b3)
    by = 3;
  if (b1 && b2 && !b3)
    by = 4;
  if (!b1 && b2 && b3)
    by = 5;
  if (b1 && !b2 && b3)
    by = 6;
  if (b1 && b2 && b3)
    by = 7;
  if (!b1 && !b2 && !b3)
    by = 8;
  return by;
}

void load()
{
  Serial.println("Previous data in RAM:");
  rtc.getram(data, BUFSIZE);
  printArray(data, BUFSIZE);
  amt = data[0];
  int now = 1;
  for (int i = 0; i < 6; i++)
  {
    alarmTime[i][0] = data[now];
    now++;
    alarmTime[i][1] = data[now];
    now++;
  }
  for (int i = 0; i < 6; i++)
  {
    alarmDays[i][0] = data[now];
    now++;
    getDays(data[now], i, 1, 2, 3);
    now++;
    getDays(data[now], i, 4, 5, 6);
    now++;
  }
}
void getDays(byte by, int i, int i1, int i2, int i3)
{
  switch (by)
  {
  case 1:
    alarmDays[i][i1] = true;
    alarmDays[i][i2] = false;
    alarmDays[i][i3] = false;
    break;
  case 2:
    alarmDays[i][i1] = false;
    alarmDays[i][i2] = true;
    alarmDays[i][i3] = false;
    break;
  case 3:
    alarmDays[i][i1] = false;
    alarmDays[i][i2] = false;
    alarmDays[i][i3] = true;
    break;
  case 4:
    alarmDays[i][i1] = true;
    alarmDays[i][i2] = true;
    alarmDays[i][i3] = false;
    break;
  case 5:
    alarmDays[i][i1] = false;
    alarmDays[i][i2] = true;
    alarmDays[i][i3] = true;
    break;
  case 6:
    alarmDays[i][i1] = true;
    alarmDays[i][i2] = false;
    alarmDays[i][i3] = true;
    break;
  case 7:
    alarmDays[i][i1] = true;
    alarmDays[i][i2] = true;
    alarmDays[i][i3] = true;
    break;
  case 8:
    alarmDays[i][i1] = false;
    alarmDays[i][i2] = false;
    alarmDays[i][i3] = false;
    break;
  }
}

String adaptDays(int day, int i)
{
  String s;
  switch (i)
  {
  case 0:
    if (alarmDays[day][i])
      s = "P";
    else
      s = "p";
    break;
  case 1:
    if (alarmDays[day][i])
      s = "W";
    else
      s = "w";
    break;
  case 2:
    if (alarmDays[day][i])
      s = "S";
    else
      s = "s";
    break;
  case 3:
    if (alarmDays[day][i])
      s = "C";
    else
      s = "c";
    break;
  case 4:
    if (alarmDays[day][i])
      s = "P";
    else
      s = "p";
    break;
  case 5:
    if (alarmDays[day][i])
      s = "S";
    else
      s = "s";
    break;
  case 6:
    if (alarmDays[day][i])
      s = "N";
    else
      s = "n";
    break;
  }

  return s;
}

bool endGame = false;
long time[4];
long timer[3];
int points = 0;
int position[3];
int lives = 3;
bool fired = false;
int jump = 0;
int high[2] = {1, 1};
int op = 0;

void game()
{
  if (!endGame)
  {
    clockStarted = false;
    if (spaceWarriorStarted)
    {
      if (millis() - time[0] >= 400)
      {
        time[0] = millis();
        if (fired)
          position[1]--;
      }

      if (millis() - time[1] >= timer[1])
      {
        timer[1] = random(100, 450);
        time[1] = millis();
        position[0]++;
      }

      if (millis() - time[2] >= timer[2])
      {
        timer[2] = random(100, 450);
        time[2] = millis();
        position[2]++;
      }

      if (millis() - time[3] >= 80)
      {
        time[3] = millis();
        lcd.clear();
        lcd.setCursor(15, high[0]);
        lcd.print("<");
        lcd.setCursor(position[0], 0);
        lcd.print("*");
        lcd.setCursor(position[2], 1);
        lcd.print("*");
        if (fired)
        {
          lcd.setCursor(position[1], high[1]);
          lcd.print("-");
        }
      }

      if (position[1] == 0)
      {
        high[1] = high[0];
        position[1] = 15;
        fired = false;
      }

      if (position[2] == 16)
      {
        lives--;
        position[2] = 0;
      }
      if (position[0] == 16)
      {
        lives--;
        position[0] = 0;
      }

      if (fired)
      {
        if ((position[2] == position[1]) && (high[1] == 1))
        {
          position[2] = 0;
          high[1] = high[0];
          position[1] = 15;
          fired = false;
          points++;
        }
        if ((position[0] == position[1]) && (high[1] == 0))
        {
          position[0] = 0;
          high[1] = high[0];
          position[1] = 15;
          fired = false;
          points++;
        }
      }

      if (selectClick)
      {
        high[1] = high[0];
        position[1] = 14;
        fired = true;
      }
      if (leftClick)
        high[0] = 0;
      if (rightClick)
        high[0] = 1;

      if (lives == 0)
      {
        endGame = true;
        clockStarted = true;
        delay(800);
      }
    }

    if (jumpStarted)
    {
      if (millis() - time[0] >= timer[0])
      {
        timer[0] = random(350, 580);
        time[0] = millis();
        points++;
        if (high[0] == 0)
          jump++;
        if (position[0] == 0)
        {
          position[0] = 15;
          op++;
        }
        position[0]--;
      }

      if (millis() - time[1] >= timer[1])
      {
        timer[1] = random(250, 500);
        time[1] = millis();
        if (position[1] == 0)
          position[1] = 15;
        if (op >= 2)
          position[1]--;
      }

      if (millis() - time[2] >= timer[2])
      {
        timer[2] = random(380, 600);
        time[2] = millis();
        if (position[2] == 0)
          position[2] = 15;
        if (op >= 3)
          position[2]--;
      }

      if (millis() - time[3] >= 80)
      {
        time[3] = millis();
        if (jump >= 5)
        {
          jump = 0;
          high[0] = 1;
        }
        lcd.clear();
        lcd.setCursor(4, high[0]);
        lcd.print("/");
        lcd.setCursor(position[0], 1);
        lcd.print("|");
        if (op >= 2)
        {
          lcd.setCursor(position[1], 1);
          lcd.print("|");
        }
        if (op >= 3)
        {
          lcd.setCursor(position[2], 1);
          lcd.print("|");
          op = 3;
        }
      }

      if (selectClick && high[0] == 1)
        high[0] = 0;

      for (int i = 0; i <= 2; i++)
      {
        if (position[i] == 4 && high[0] == 1)
        {
          endGame = true;
          clockStarted = true;
          i = 3;
          delay(800);
        }
      }
    }
  }
  else
  {
    if (clockStarted)
    {
      clockStarted = false;
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("GAME");
      lcd.setCursor(3, 1);
      lcd.print("OVER");
      lcd.setCursor(11, 0);
      lcd.print("Wynik");
      lcd.setCursor(13, 1);
      lcd.print(points);
      for (int i = 0; i <= 2; i++)
      {
        position[i] = 0;
        timer[i] = 0;
      }
      points = 0;
      lives = 3;
      op = 0;
      for (int i = 0; i <= 1; i++)
        high[i] = 1;
      jump = 0;
    }

    if (selectClick)
    {
      clockStarted = true;
      endGame = false;
      lcd.clear();
      if (spaceWarriorStarted)
      {
        lcd.setCursor(0, 0);
        lcd.print("*");
        lcd.setCursor(0, 1);
        lcd.print("*");
        lcd.setCursor(15, high[0]);
        lcd.print("<");
      }
      if (jumpStarted)
      {
        lcd.setCursor(4, high[0]);
        lcd.print("/");
        for (int i = 0; i <= 2; i++)
        {
          lcd.setCursor(position[i], 1);
          lcd.print("|");
        }
      }
      delay(800);
    }

    if (leftClick || rightClick)
    {
      clickable = false;
      delayL = (2000 + millis());
      delayed = false;
      clockStarted = true;
      endGame = false;
      spaceWarriorStarted = false;
      jumpStarted = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("00:00:00");
      lcd.setCursor(14, 0);
      lcd.print((String) char(223) + "C");
    }
  }
}