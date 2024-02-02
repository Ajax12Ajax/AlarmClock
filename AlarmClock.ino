#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

boolean spaceWarriorStarted = false;
boolean jumpStarted = false;
boolean clockStarted = true;

int sensor = A3;
int temp;

boolean backlight = true;
boolean clickBacklight = false;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS1302 rtc;

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
  lcd.noBlink();
  lcd.noCursor();
  rtc.begin();
  //rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop()
{
  if (clockStarted)
    clock();
  if (spaceWarriorStarted || jumpStarted)
    game();
}

char buf[20];

boolean settings = false;
long last;
int alarmData[6][2];
boolean days[6][7];
int now = -1;

void clock()
{
  int sensorVal1 = digitalRead(9);
  int sensorVal2 = digitalRead(10);
  int sensorVal3 = digitalRead(8);

  if (!settings)
  {
    lcd.init();
    lcd.setCursor(0, 0);
    lcd.print("00:00:00");
    lcd.setCursor(14, 0);
    lcd.print((String) char(223) + "C");
    DateTime now = rtc.now();
    //Serial.println(now.tostr(buf));
    lcd.setCursor(0, 0);
    lcd.print(format(now.hour()));
    Serial.println(format(now.hour()));

    lcd.setCursor(3, 0);
    lcd.print(format(now.minute()));

    lcd.setCursor(6, 0);
    lcd.print(format(now.second()));

    temp = (analogRead(sensor) * 5.0) / 1024.0 * 100.0;
    lcd.setCursor(12, 0);
    lcd.print(temp);

    lcd.setCursor(0, 1);
    lcd.print((String)now.day() + "/" + now.month() + "/" + now.year());

    if (sensorVal1 == LOW)
    {
      if (!clickBacklight)
      {
        last = millis();
      }
      clickBacklight = true;
    }
    else if ((millis() - last) >= 2000 && clickBacklight)
    {
      lcd.clear();
      settings = true;
    }
    else if (clickBacklight)
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
      clickBacklight = false;
    }
    else
    {
      clickBacklight = false;
    }
    if (sensorVal3 == LOW)
      jumpStarted = true;
    if (sensorVal2 == LOW)
      spaceWarriorStarted = true;
  }
  else
  {
    if (now < 0)
    {
      lcd.setCursor(0, 0);
      lcd.print("00:00:00");
    }
    else
    {
    }
  }
}

String format(int i)
{
  String s;
  if (i < 10 && i >= 0)
    s = "0" + i;
  else
    s = i;
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
  int sensorVal1 = digitalRead(9);
  int sensorVal2 = digitalRead(10);
  int sensorVal3 = digitalRead(8);

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

      if (sensorVal1 == LOW)
      {
        high[1] = high[0];
        position[1] = 14;
        fired = true;
      }
      if (sensorVal2 == LOW)
        high[0] = 0;
      if (sensorVal3 == LOW)
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

      if (sensorVal1 == LOW && high[0] == 1)
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

    if (sensorVal1 == LOW)
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

    if (sensorVal2 == LOW || sensorVal3 == LOW)
    {
      clockStarted = true;
      endGame = false;
      spaceWarriorStarted = false;
      jumpStarted = false;
      lcd.backlight();
      lcd.clear();
      lcd.noBlink();
      lcd.noCursor();
      // h = 0;
      // min = 0;
      // sec = 0;
    }
  }
}