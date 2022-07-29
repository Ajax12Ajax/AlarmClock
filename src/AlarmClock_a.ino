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
    if (clockStarted)
        clock();
    if (spaceWarriorStarted)
        spaceWarriorGame();
    if (jumpStarted)
        jumpGame();
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

    // if (sensorVal1 == LOW)

    if (sensorVal3 == LOW)
        jumpStarted = true;
    if (sensorVal2 == LOW)
        spaceWarriorStarted = true;
    // if (sensorVal3 == HIGH && sensorVal2 == HIGH)
    //     noTone(A0);
}