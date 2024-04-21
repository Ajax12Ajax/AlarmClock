bool endSW = false;
long timeSW[4];
long timerSW[3];
int pointsSW = 0;
int positionSW[3];
int lives = 3;
bool fired = false;
int highSW[2] = {1, 1};
bool shownSW = false;

void initSpaceWarrior()
{
  endSW = false;
  for (int i = 0; i <= 2; i++)
  {
    positionSW[i] = 0;
    timerSW[i] = 0;
  }
  pointsSW = 0;
  lives = 3;
  for (int i = 0; i <= 1; i++)
    highSW[i] = 1;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("*");
  lcd.setCursor(0, 1);
  lcd.print("*");
  lcd.setCursor(15, highSW[0]);
  lcd.print("<");
  mode = Game_SpaceWarrior;
}

void spaceWarrior()
{
  if (!endSW)
  {
    if (millis() - timeSW[0] >= 400)
    {
      timeSW[0] = millis();
      if (fired)
        positionSW[1]--;
    }

    if (millis() - timeSW[1] >= timerSW[1])
    {
      timerSW[1] = random(100, 450);
      timeSW[1] = millis();
      positionSW[0]++;
    }

    if (millis() - timeSW[2] >= timerSW[2])
    {
      timerSW[2] = random(100, 450);
      timeSW[2] = millis();
      positionSW[2]++;
    }

    if (millis() - timeSW[3] >= 80)
    {
      timeSW[3] = millis();
      lcd.clear();
      lcd.setCursor(15, highSW[0]);
      lcd.print("<");
      lcd.setCursor(positionSW[0], 0);
      lcd.print("*");
      lcd.setCursor(positionSW[2], 1);
      lcd.print("*");
      if (fired)
      {
        lcd.setCursor(positionSW[1], highSW[1]);
        lcd.print("-");
      }
    }

    if (positionSW[1] == 0)
    {
      highSW[1] = highSW[0];
      positionSW[1] = 15;
      fired = false;
    }

    if (positionSW[2] == 16)
    {
      lives--;
      positionSW[2] = 0;
    }
    if (positionSW[0] == 16)
    {
      lives--;
      positionSW[0] = 0;
    }

    if (fired)
    {
      if ((positionSW[2] == positionSW[1]) && (highSW[1] == 1))
      {
        positionSW[2] = 0;
        highSW[1] = highSW[0];
        positionSW[1] = 15;
        fired = false;
        pointsSW++;
      }
      if ((positionSW[0] == positionSW[1]) && (highSW[1] == 0))
      {
        positionSW[0] = 0;
        highSW[1] = highSW[0];
        positionSW[1] = 15;
        fired = false;
        pointsSW++;
      }
    }

    if (selectClick && (positionSW[1] <= 4 || positionSW[1] == 15))
    {
      highSW[1] = highSW[0];
      positionSW[1] = 14;
      fired = true;
    }
    if (leftClick)
      highSW[0] = 0;
    if (rightClick)
      highSW[0] = 1;

    if (lives == 0)
    {
      endSW = true;
      shownSW = true;
      clickable = false;
      timeDelay = (400 + millis());
      delayed = false;
    }
  }
  else
  {
    if (delayed)
      clickable = true;
    if (shownSW)
    {
      shownSW = false;
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("GAME");
      lcd.setCursor(3, 1);
      lcd.print("OVER");
      lcd.setCursor(11, 0);
      lcd.print("Wynik");
      lcd.setCursor(13, 1);
      lcd.print(pointsSW);
    }

    if (selectClick)
    {
      initSpaceWarrior();
      delay(800);
    }

    if (leftClick || rightClick)
    {
      clickable = false;
      timeDelay = (400 + millis());
      delayed = false;
      initClock();
    }
  }
}