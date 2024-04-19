bool end = false;
long time[4];
long timer[3];
int points = 0;
int position[3];
int lives = 3;
bool fired = false;
int jump = 0;
int high[2] = {1, 1};
int op = 0;
bool shown = false;

void initSpaceWarrior()
{
    end = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("*");
    lcd.setCursor(0, 1);
    lcd.print("*");
    lcd.setCursor(15, high[0]);
    lcd.print("<");
}

void spaceWarrior()
{
  if (!end)
  {
    clockStarted = false;
    lcd.backlight();
    backlight = true;
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

      if (selectClick && (position[1] <= 4 || position[1] == 15))
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
        end = true;
        shown = true;
        clickable = false;
        delayL = (400 + millis());
        delayed = false;
      }
    }
  }
  else
  {
    if (delayed)
      clickable = true;

    if (shown)
    {
      shown = false;
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
      initSpaceWarrior();
      delay(800);
    }

    if (leftClick || rightClick)
    {
      clickable = false;
      delayL = (400 + millis());
      delayed = false;
      clockStarted = true;
      end = false;
      spaceWarriorStarted = false;
      jumpStarted = false;
      initClock();
    }
  }
}