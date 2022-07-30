unsigned long time[18];
unsigned int points = 0;
unsigned int jump = 0;
unsigned int high = 1;
unsigned int op = 15;
boolean endGame = false;

void jumpGame()
{
    int sensorVal1 = digitalRead(3);
    int sensorVal2 = digitalRead(4);
    int sensorVal3 = digitalRead(2);

    if (!endGame)
    {
        time[0] = millis(); // Pobierz liczbe milisekund od startu programu
        time[1] = time[0] - time[2];

        if (time[1] >= 400)
        {
            // Zapamietaj aktualny time
            time[2] = time[0];
            points++;
            if (high == 0)
            {
                jump++;
            }
            if (op == 0)
                op = 15;
            op--;

            Serial.println(points);
            Serial.println(op);
            Serial.println(high);
            clockStarted = true;
        }
        clockStarted = false;
        time[3] = millis(); // Pobierz liczbe milisekund od startu programu
        time[4] = time[3] - time[5];

        if (time[4] >= 450)
        {
            // Zapamietaj aktualny time
            time[5] = time[3];
        }

        time[11] = millis(); // Pobierz liczbe milisekund od startu programu
        time[12] = time[11] - time[6];

        if (time[12] >= 550)
        {
            // Zapamietaj aktualny time
            time[6] = time[11];
        }

        time[10] = millis(); // Pobierz liczbe milisekund od startu programu
        time[13] = time[10] - time[7];

        if (time[13] >= 80)
        {
            // Zapamietaj aktualny time
            time[7] = time[10];
            if (jump >= 5)
            {
                jump = 0;
                high = 1;
            }
            lcd.clear();
            lcd.setCursor(4, high);
            lcd.print("/");
            lcd.setCursor(op, 1);
            lcd.print("|");
        }
        if (sensorVal1 == LOW && high == 1)
        {
            high = 0;
        }

        endGame = (op == 4 && high == 1);
        clockStarted = (op == 4 && high == 1);
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
        }

        if (sensorVal1 == LOW)
        {
            clockStarted = true;
            jumpStarted = true;
            endGame = false;
            points = 0;
            jump = 0;
            high = 1;
            op = 15;
            lcd.clear();
            lcd.setCursor(4, high);
            lcd.print("/");
            lcd.setCursor(op, 1);
            lcd.print("|");
            delay(800);
        }
        if (sensorVal2 == LOW || sensorVal3 == LOW)
        {
            points = 0;
            jump = 0;
            high = 1;
            op = 15;
            endGame = false;
            clockStarted = true;
            jumpStarted = false;
            lcd.begin();
            lcd.backlight();
            lcd.clear();
            lcd.noBlink();
            lcd.noCursor();
            clockSetup();
        }
    }
}