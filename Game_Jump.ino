bool endJ = false;
long timeJ[4];
long timerJ[3];
int pointsJ = 0;
int positionJ[3];
int leap = 0;
int highJ[2] = {1, 1};
int op = 0;
bool shownJ = false;
bool pressedJ = false;

void initJump()
{
  endJ = false;
  for (int i = 0; i <= 2; i++)
  {
    positionJ[i] = 15;
    timerJ[i] = 0;
  }
  pointsJ = 0;
  op = 0;
  for (int i = 0; i <= 1; i++)
    highJ[i] = 1;
  leap = 0;
  lcd.clear();
  lcd.setCursor(4, highJ[0]);
  lcd.print("/");
  for (int i = 0; i <= 2; i++)
  {
    lcd.setCursor(positionJ[i], 1);
    lcd.print("|");
  }
  mode = Game_Jump;
}

void jump()
{
  if (delayed)
    clickable = true;
  if (!endJ)
  {
    if (millis() - timeJ[0] >= timerJ[0])
    {
      timerJ[0] = random(350, 580);
      timeJ[0] = millis();
      pointsJ++;
      if (highJ[0] == 0)
        leap++;
      if (positionJ[0] == 0)
      {
        positionJ[0] = 15;
        op++;
      }
      positionJ[0]--;
    }

    if (millis() - timeJ[1] >= timerJ[1])
    {
      timerJ[1] = random(250, 500);
      timeJ[1] = millis();
      if (positionJ[1] == 0)
        positionJ[1] = 15;
      if (op >= 2)
        positionJ[1]--;
    }

    if (millis() - timeJ[2] >= timerJ[2])
    {
      timerJ[2] = random(380, 600);
      timeJ[2] = millis();
      if (positionJ[2] == 0)
        positionJ[2] = 15;
      if (op >= 3)
        positionJ[2]--;
    }

    if (millis() - timeJ[3] >= 80)
    {
      timeJ[3] = millis();
      if (leap >= 5)
      {
        leap = 0;
        highJ[0] = 1;
      }
      lcd.clear();
      lcd.setCursor(4, highJ[0]);
      lcd.print("/");
      lcd.setCursor(positionJ[0], 1);
      lcd.print("|");
      if (op >= 2)
      {
        lcd.setCursor(positionJ[1], 1);
        lcd.print("|");
      }
      if (op >= 3)
      {
        lcd.setCursor(positionJ[2], 1);
        lcd.print("|");
        op = 3;
      }
    }

    if (selectClick)
    {
      if (highJ[0] == 1 && !pressedJ)
      {
        highJ[0] = 0;
        clickable = false;
        timeDelay = (200 + millis());
        delayed = false;
      }
      pressedJ = true;
    }
    else
    {
      pressedJ = false;
    }

    for (int i = 0; i <= 2; i++)
    {
      if (positionJ[i] == 4 && highJ[0] == 1)
      {
        endJ = true;
        shownJ = true;
        i = 3;
        clickable = false;
        timeDelay = (400 + millis());
        delayed = false;
      }
    }
  }
  else
  {
    if (shownJ)
    {
      shownJ = false;
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("GAME");
      lcd.setCursor(3, 1);
      lcd.print("OVER");
      lcd.setCursor(11, 0);
      lcd.print("Wynik");
      lcd.setCursor(13, 1);
      lcd.print(pointsJ);
    }

    if (selectClick)
    {
      initJump();
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