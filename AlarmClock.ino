#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <TimeLib.h>

unsigned long czas[18];

unsigned int krok = 0;
unsigned int wys = 0;
unsigned int wys2 = 0;
unsigned int krok2 = 15;
unsigned int ok = 0;
unsigned int krok3 = 0;
unsigned int zycia = 3;
unsigned int punkty = 0;
int sec = second();
int min = minute();
int h = hour();
boolean gameStarted = false;
boolean clockStarted = true;

int czujnik = A1; // pin analogowy A1 połączony z sygnałem z czujnika
float VOLT;
float TEMP;

time_t prevDisplay;

String stringOne;

LiquidCrystal_I2C lcd(0x27, 16, 2);

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
    if (gameStarted)
        game();
    else if (clockStarted)
        clock();
}

void clockSetup()
{
    stringOne = String("00:00:00");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(stringOne);

    stringOne = String(day()) + "/" + String(month()) + "/" + String(year());

    lcd.setCursor(0, 1);
    lcd.print(stringOne);
    lcd.setCursor(10, 1);
    lcd.print(TEMP);
    lcd.setCursor(15, 1);
    lcd.print("C");

    int odczyt = analogRead(czujnik);  // odczytanie wartości z  czujnika
    VOLT = (odczyt * 5.0) / 1024.0;    // przeliczenie odczytanej wartości na napięcie w woltach (dla podłączenia pod 5 V)
    TEMP = VOLT * 100;                 // konwersja z napięcia na temperaturę, rozdzielczość czujnika wynosi 10 mV na stopień
    Serial.print("Temperatura (C): "); // wyświetlenie jej na monitorze
    Serial.println(TEMP);
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
            stringOne = String("0" + String(hour(), DEC));
        else
            stringOne = String(hour(), DEC);
        lcd.print(stringOne);
        h = hour();
    }

    if (min != minute())
    {
        lcd.setCursor(3, 0);
        if (minute() < 10)
            stringOne = String("0" + String(minute(), DEC));
        else
            stringOne = String(minute(), DEC);
        lcd.print(stringOne);
        min = minute();
    }

    if (sec != second())
    {
        lcd.setCursor(6, 0);
        if (second() < 10)
            stringOne = String("0" + String(second(), DEC));
        else
            stringOne = String(second(), DEC);
        lcd.print(stringOne);
        sec = second();
    }

    if (sensorVal1 == LOW)
        gameStarted = true;

    if (sensorVal3 == LOW)
        tone(A0, 20);
    if (sensorVal2 == LOW)
        tone(A0, 10);
    if (sensorVal3 == HIGH && sensorVal2 == HIGH)
        noTone(A0);
}

void game()
{
    int sensorVal1 = digitalRead(3);
    int sensorVal2 = digitalRead(4);
    int sensorVal3 = digitalRead(2);

    if (zycia > 0)
    {
        czas[0] = millis(); // Pobierz liczbe milisekund od startu programu
        czas[1] = czas[0] - czas[2];

        if (czas[1] >= 400)
        {
            // Zapamietaj aktualny czas
            czas[2] = czas[0];

            if (ok == 1)
                krok2--;
        }

        czas[3] = millis(); // Pobierz liczbe milisekund od startu programu
        czas[4] = czas[3] - czas[5];

        if (czas[4] >= 450)
        {
            // Zapamietaj aktualny czas
            czas[5] = czas[3];

            krok++;
            Serial.println("650");
        }

        czas[11] = millis(); // Pobierz liczbe milisekund od startu programu
        czas[12] = czas[11] - czas[6];

        if (czas[12] >= 550)
        {
            // Zapamietaj aktualny czas
            czas[6] = czas[11];

            krok3++;
        }

        czas[10] = millis(); // Pobierz liczbe milisekund od startu programu
        czas[13] = czas[10] - czas[7];

        if (czas[13] >= 80)
        {
            // Zapamietaj aktualny czas
            czas[7] = czas[10];
            lcd.clear();
            lcd.setCursor(15, wys);
            lcd.print("<");
            lcd.setCursor(krok, 0);
            lcd.print("*");
            lcd.setCursor(krok3, 1);
            lcd.print("*");
            if (ok == 1)
            {
                lcd.setCursor(krok2, wys2);
                lcd.print("-");
            }
        }

        if (krok2 == 0)
        {
            wys2 = wys;
            lcd.clear();
            krok2 = 15;
            ok = 0;
            lcd.setCursor(krok, 0);
            lcd.print("*");
            lcd.setCursor(15, wys);
            lcd.print("<");
        }

        if (krok3 == 16)
        {
            zycia--;
            krok3 = 0;
        }

        if (krok == 16)
        {
            zycia--;
            krok = 0;
        }
        if (ok == 1)
        {
            if ((krok3 == krok2) && (wys2 == 1))
            {
                krok3 = 0;
                wys2 = wys;
                lcd.clear();
                krok2 = 15;
                ok = 0;
                punkty++;
                lcd.setCursor(15, wys);
                lcd.print("<");
            }

            if ((krok == krok2) && (wys2 == 0))
            {
                krok = 0;
                wys2 = wys;
                lcd.clear();
                krok2 = 15;
                ok = 0;
                punkty++;
                lcd.setCursor(15, wys);
                lcd.print("<");
            }
        }
        if (sensorVal1 == LOW)
        {
            wys2 = wys;
            krok2 = 14;
            ok = 1;
        }
        if (sensorVal2 == LOW)
            wys = 0;

        if (sensorVal3 == LOW)
            wys = 1;
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
            lcd.print(punkty);
        }

        if (sensorVal1 == LOW)
        {
            clockStarted = true;
            gameStarted = true;
            punkty = 0;
            zycia = 3;
            krok = 0;
            krok3 = 0;
            krok2 = 15;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("*");
            lcd.setCursor(0, 1);
            lcd.print("*");
            delay(800);
        }
        if (sensorVal2 == LOW || sensorVal3 == LOW)
        {
            punkty = 0;
            zycia = 3;
            krok = 0;
            krok3 = 0;
            krok2 = 15;
            clockStarted = true;
            gameStarted = false;
            lcd.begin();
            lcd.backlight();
            lcd.clear();
            lcd.noBlink();
            lcd.noCursor();
            clockSetup();
        }
    }
}