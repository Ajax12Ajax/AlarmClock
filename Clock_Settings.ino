bool selected = false;
bool chosenTime = false;
bool chosenDate = false;
int nowV = 0;
int nowH = 0;

void clockSetting()
{
    if (delayed)
        clickable = true;
    if (!selected)
    {
        lcd.setCursor(15, 0);
        lcd.printByte(1);
        lcd.setCursor(15, 1);
        lcd.printByte(2);

        for (int i = 0; 1 >= i; i++)
            if (alarmsAmount > nowV + i)
            {
                if (i == 0)
                {
                    lcd.setCursor(14, 0);
                    lcd.print("<");
                }
                lcd.setCursor(0, i);
                lcd.print(adapt(alarmTime[nowV + i][0]) + ":" + adapt(alarmTime[nowV + i][1]) + " " +
                          adaptDays(nowV + i, 0) + adaptDays(nowV + i, 1) + adaptDays(nowV + i, 2) + adaptDays(nowV + i, 3) + adaptDays(nowV + i, 4) + adaptDays(nowV + i, 5) + adaptDays(nowV + i, 6));
                if (alarmMode[nowV + i] == 2)
                {
                    lcd.setCursor(13, i);
                    lcd.print("*");
                }
            }
            else if (alarmsAmount == nowV + i && alarmsAmount < 6)
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
                touchDownTime = millis();
            clickedS = true;
            if ((millis() - touchDownTime) >= 800 && !pressed)
            {
                initClock();
                nowV = 0;
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
                if (alarmsAmount <= nowV)
                    alarmsAmount++;
                lcd.clear();
                selected = true;
                clickedS = false;
                clickable = false;
                timeDelay = (millis() + 150);
                delayed = false;
            }
        }

        if (leftClick && rightClick)
        {
            if (!clickedL || !clickedR)
                touchDownTime = millis();
            clickedL = true;
            clickedR = true;
            if ((millis() - touchDownTime) >= 850 && !pressed)
            {
                if (alarmsAmount > nowV)
                {
                    removeAlarm(nowV);
                    lcd.clear();
                    pressed = true;
                }
            }
        }
        else if (clickedL && clickedR)
        {
            if (pressed)
            {
                clickable = false;
                timeDelay = (millis() + 700);
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
        {
            if (!clickedL)
                touchDownTime = millis();
            clickedL = true;
            if ((millis() - touchDownTime) >= 700 && !pressed)
            {
                if (alarmsAmount > nowV)
                {
                    if (alarmMode[nowV] != 2)
                        alarmMode[nowV] = 2;
                    else
                        alarmMode[nowV] = 0;
                }
                lcd.clear();
                pressed = true;
            }
        }
        else if (rightClick)
            clickedR = true;
        else if (clickedL)
        {
            if (pressed)
            {
                clickedL = false;
                pressed = false;
            }
            else
            {
                if (nowV < alarmsAmount && nowV < 5)
                    nowV++;
                lcd.clear();
                clickedL = false;
            }
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
                    touchDownTime = millis();
                clickedS = true;
                if ((millis() - touchDownTime) >= 800 && !pressed)
                {
                    lcd.clear();
                    nowH = 0;
                    save();
                    selected = false;
                    pressed = true;
                }
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
                if (nowH >= 0 && nowH <= 1 && alarmTime[nowV][0] < 23.0f)
                    alarmTime[nowV][0]++;
                else if (nowH >= 3 && nowH <= 4 && alarmTime[nowV][1] < 55.0f)
                    alarmTime[nowV][1] += 5.0f;
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
                clickedR = false;
            }

            if (selectClick)
                clickedS = true;
            else if (clickedS)
            {
                chosenTime = false;
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
                clickedL = false;
            }

            if (rightClick)
                clickedR = true;
            else if (clickedR)
            {
                alarmDays[nowV][nowH - 6] = false;
                clickedR = false;
            }

            if (selectClick)
                clickedS = true;
            else if (clickedS)
            {
                chosenDate = false;
                lcd.clear();
                clickedS = false;
            }
        }
    }
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
    for (int i = 0; i < alarmsAmount; i++)
        if (alarmMode[i] == 1)
            alarmMode[i] = 0;
    if (alarmsAmount > 6 || alarmsAmount < 0)
    {
        alarmsAmount = 6;
        Serial.println("fixed");
    }
    data[0] = alarmsAmount;
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
    alarmsAmount = data[0];
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
    if (alarmsAmount > 6 || alarmsAmount < 0)
    {
        alarmsAmount = 6;
        Serial.println("fixed");
        save();
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

void removeAlarm(int i)
{
    for (int t = i; t < 6 - 1; t++)
    {
        for (int j = 0; j < 4; j++)
            alarmTime[t][j] = alarmTime[t + 1][j];
        for (int j = 0; j < 8; j++)
            alarmDays[t][j] = alarmDays[t + 1][j];
        alarmMode[t] = alarmMode[t + 1];
    }
    for (int j = 0; j < 4; j++)
        alarmTime[alarmsAmount - 1][j] = 0;
    for (int j = 0; j < 8; j++)
        alarmDays[alarmsAmount - 1][j] = 0;
    alarmMode[alarmsAmount - 1] = 0;
    alarmsAmount--;
    save();
}