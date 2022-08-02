#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <TimeLib.h>

int sec = second();
int min = minute();
int h = hour();
boolean spaceWarriorStarted = false;
boolean jumpStarted = false;
boolean clockStarted = true;

int sensor = A1;
int temp;

LiquidCrystal_I2C lcd(0x27, 16, 2);

// There was supposed to be time coming from the gps
// but the module is not working and there was supposed to be temperature
// but I wired it wrong and the module burned out

void setup()
{
    Serial.begin(9600);
    pinMode(2, INPUT_PULLUP);
    pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(A0, OUTPUT);
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    lcd.noBlink();
    lcd.noCursor();
    clockSetup();
}

void loop()
{
    if (clockStarted)
        clock();
    if (spaceWarriorStarted || jumpStarted)
        game();
}

void clockSetup()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("00:00:00");

    lcd.setCursor(0, 1);
    lcd.print((String)day() + "/" + month() + "/" + year());

    // temp = (analogRead(sensor) * 5.0) / 1024.0 * 100.0;
    temp = 28;
    Serial.println((String) "Temperatura (C): " + temp);
    lcd.setCursor(12, 0);
    lcd.print(temp);
    lcd.setCursor(14, 0);
    lcd.print((String) char(223) + "C");
}

void clock()
{
    int sensorVal1 = digitalRead(3);
    int sensorVal2 = digitalRead(4);
    int sensorVal3 = digitalRead(2);

    if (h != hour())
    {
        lcd.setCursor(0, 0);
        if (hour() < 10)
            lcd.print((String) "0" + hour());
        else
            lcd.print(hour());
        h = hour();
    }

    if (min != minute())
    {
        lcd.setCursor(3, 0);
        if (minute() < 10)
            lcd.print((String) "0" + minute());
        else
            lcd.print(minute());
        min = minute();
    }

    if (sec != second())
    {
        lcd.setCursor(6, 0);
        if (second() < 10)
            lcd.print((String) "0" + second());
        else
            lcd.print(second());
        sec = second();

        // temp = (analogRead(sensor) * 5.0) / 1024.0 * 100.0;
        temp = 28;
        lcd.setCursor(12, 0);
        lcd.print(temp);

        lcd.setCursor(0, 1);
        lcd.print((String)day() + "/" + month() + "/" + year());
    }

    if (sensorVal3 == LOW)
        jumpStarted = true;
    if (sensorVal2 == LOW)
        spaceWarriorStarted = true;
}

boolean endGame = false;
long time[4];
int points = 0;
int position[3];
int lives = 3;
boolean fired = false;
int jump = 0;
int high[2] = {1, 1};
int op = 0;

void game()
{
    int sensorVal1 = digitalRead(3);
    int sensorVal2 = digitalRead(4);
    int sensorVal3 = digitalRead(2);

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

            if (millis() - time[1] >= 450)
            {
                time[1] = millis();
                position[0]++;
            }

            if (millis() - time[2] >= 550)
            {
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
            if (millis() - time[0] >= 400)
            {
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

            if (millis() - time[1] >= 250)
            {
                time[1] = millis();
                if (position[1] == 0)
                    position[1] = 15;
                if (op >= 2)
                    position[1]--;
            }

            if (millis() - time[2] >= 600)
            {
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
                position[i] = 0;
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
            lcd.begin();
            lcd.backlight();
            lcd.clear();
            lcd.noBlink();
            lcd.noCursor();
            clockSetup();
            h = 0;
            min = 0;
            sec = 0;
        }
    }
}