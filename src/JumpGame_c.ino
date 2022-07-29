unsigned long time[18];
unsigned int points = 0;
unsigned int jump = 0;
unsigned int high = 1;
unsigned int op = 15;

void jumpGame()
{
    int sensorVal1 = digitalRead(3);
    int sensorVal2 = digitalRead(4);
    int sensorVal3 = digitalRead(2);
    clockStarted = false;

    time[0] = millis(); // Pobierz liczbe milisekund od startu programu
    time[1] = time[0] - time[2];

    if (time[1] >= 400)
    {
        // Zapamietaj aktualny time
        time[2] = time[0];
        points++;
        jump++;
        if (jump >= 3)
        {
            jump = 0;
            high = 1;
        }
        if (op == 0)
            op = 15;
        op--;

        Serial.println(points);
    }

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
}
