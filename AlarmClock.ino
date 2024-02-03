#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args) write(args);
#else
#define printByte(args) print(args, BYTE);
#endif

uint8_t arrowBold[8] = {0x4, 0xe, 0x1f, 0x1b, 0x11, 0x0, 0x0};
uint8_t arrowUp[8] = {0x0, 0x0, 0x4, 0xa, 0x11, 0x0, 0x0};
uint8_t arrowDown[8] = {0x0, 0x0, 0x11, 0xa, 0x4, 0x0, 0x0};

boolean spaceWarriorStarted = false;
boolean jumpStarted = false;
boolean clockStarted = true;

int sensor = A3;
int temp;

boolean backlight = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS1302 rtc;

boolean selectClick = false;
boolean leftClick = false;
boolean rightClick = false;
boolean clickable = true;
boolean clickedS = false;
boolean clickedL = false;
boolean clickedR = false;

long lastS;
long delayL = 0;
boolean delayed = true;

void setup()
{
  lcd.init();
  Serial.begin(9600);
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
  lcd.home();
  lcd.noBlink();
  lcd.noCursor();
  lcd.setCursor(0, 0);
  lcd.print("00:00:00");
  lcd.setCursor(14, 0);
  lcd.print((String) char(223) + "C");
  rtc.begin();
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

char buf[20];

boolean settings = false;
long last;
int alarmTime[6][2];
boolean alarmDays[6][7];
int nowV = -1;
boolean selected = false;
boolean chosenTime = false;
boolean chosenDate = false;

int nowH = 0;

void clock()
{
  if (!settings)
  {
    DateTime now = rtc.now();
    if (lastS != now.second())
    {
      if (delayed)
        clickable = true;

      lastS = now.second();
      // Serial.println(now.tostr(buf));
      lcd.setCursor(0, 0);
      lcd.print(fortt(now.hour()));

      lcd.setCursor(3, 0);
      lcd.print(fortt(now.minute()));

      lcd.setCursor(6, 0);
      lcd.print(fortt(now.second()));

      temp = (analogRead(sensor) * 5.0) / 1024.0 * 100.0;
      lcd.setCursor(12, 0);
      lcd.print(temp);

      lcd.setCursor(0, 1);
      lcd.print((String)now.day() + "/" + now.month() + "/" + now.year());
    }

    if (selectClick)
    {
      if (!clickedS)
      {
        last = millis();
      }
      clickedS = true;
    }
    else if ((millis() - last) >= 2000 && clickedS)
    {
      lcd.clear();
      settings = true;
      clickedS = false;
      clickable = false;
      delayL = (millis() + 2000);
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
    else
    {
      clickedS = false;
    }
    if (rightClick)
      jumpStarted = true;
    if (leftClick)
      spaceWarriorStarted = true;
  }
  else
  {
    if (!selected)
    {
      lcd.setCursor(15, 0);
      lcd.printByte(1);
      lcd.setCursor(15, 1);
      lcd.printByte(2);
      if (nowV < 0)
      {
        lcd.setCursor(0, 0);
        lcd.print("new+");
        if (delayed)
          clickable = true;
        if (selectClick)
          clickedS = true;
        else if (clickedS)
        {
          lcd.clear();
          selected = true;
          clickedS = false;
        }
      }
      else
      {
        lcd.setCursor(0, 0);
        lcd.print(fortt(alarmTime[nowV][0]) + ":" + fortt(alarmTime[nowV][1]));
      }
    }
    else
    {
      lcd.setCursor(0, 0);
      lcd.print(fortt(alarmTime[nowV][0]) + ":" + fortt(alarmTime[nowV][1]) + " " + tes(0) + tes(1) + tes(2) + tes(3) + tes(4) + tes(5) + tes(6));
      lcd.setCursor(nowH, 1);
      if (chosenTime)
      {
        if ((nowH >= 0 && nowH <= 1) || (nowH >= 3 && nowH <= 4))
        {
          lcd.printByte(0);
          lcd.setCursor(nowH + 1, 1);
          lcd.printByte(0);
        }
        else if (nowH >= 6)
          lcd.printByte(0);
        lcd.setCursor(15, 0);
        lcd.printByte(1);
        lcd.setCursor(15, 1);
        lcd.printByte(2);

        if (leftClick)
          clickedL = true;
        else if (clickedL)
        {

          if (nowH >= 0 && nowH <= 1)
            alarmTime[nowV][0]++;
          if (nowH >= 3 && nowH <= 4)
            alarmTime[nowV][1]++;
          clickedL = false;
        }

        if (rightClick)
          clickedR = true;
        else if (clickedR)
        {
          Serial.println("2");
          if (nowH >= 0 && nowH <= 1)
            alarmTime[nowV][0]--;
          if (nowH >= 3 && nowH <= 4)
            alarmTime[nowV][1]--;
          clickedR = false;
        }
      }
      else if (chosenDate)
      {
        if ((nowH >= 0 && nowH <= 1) || (nowH >= 3 && nowH <= 4))
        {
          lcd.printByte(0);
          lcd.setCursor(nowH + 1, 1);
          lcd.printByte(0);
        }
        else if (nowH >= 6)
          lcd.printByte(0);
      }
      else
      {
        if ((nowH >= 0 && nowH <= 1) || (nowH >= 3 && nowH <= 4))
          lcd.print("^^");
        else if (nowH >= 6)
          lcd.print("^");
        lcd.setCursor(14, 1);
        lcd.print("<>");

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
          clickedS = true;
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
    }
  }
}

String fortt(int i)
{
  String s;
  if (i < 10 && i >= 0)
    s = "0" + String(i);
  else
    s = String(i);
  return s;
}

String tes(int i)
{
  String s;
  switch (i)
  {
  case 0:
    if (alarmDays[nowV][i])
      s = "P";
    else
      s = "p";
    break;
  case 1:
    if (alarmDays[nowV][i])
      s = "W";
    else
      s = "w";
    break;
  case 2:
    if (alarmDays[nowV][i])
      s = "S";
    else
      s = "s";
    break;
  case 3:
    if (alarmDays[nowV][i])
      s = "C";
    else
      s = "c";
    break;
  case 4:
    if (alarmDays[nowV][i])
      s = "P";
    else
      s = "p";
    break;
  case 5:
    if (alarmDays[nowV][i])
      s = "S";
    else
      s = "s";
    break;
  case 6:
    if (alarmDays[nowV][i])
      s = "N";
    else
      s = "n";
    break;
  }

  return s;
}

boolean endGame = false;
long time[4];
long timer[3];
int points = 0;
int position[3];
int lives = 3;
boolean fired = false;
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
      // h = 0;
      // min = 0;
      // sec = 0;
    }
  }
}