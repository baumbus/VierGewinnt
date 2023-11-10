#include <Adafruit_NeoPixel.h>

#include "game.h"

#ifdef __AVR__
#include <avr/power.h>
#endif

#define TASTE_LINKS 7
#define TASTE_RECHTS 6
#define TASTE_GEGNER 3
#define TASTE_ICH 4
#define TASTE_ENTER 5
#define SPIELENDE 8

static int Spielbrett[15][15];
static int Spieler = 0;
static int Reihe = 0;
static int Spalte = 0;
static int Feld_Nummer = 0;
static int Cursor = 0;
static int Game_is_over = 0;
static int Zufallszahl;

void setup()
{
    strip.begin();
    strip.show();
    strip.setBrightness(20);
    pinMode(TASTE_LINKS, INPUT_PULLUP);
    pinMode(TASTE_RECHTS, INPUT_PULLUP);
    pinMode(TASTE_GEGNER, INPUT_PULLUP);
    pinMode(TASTE_ICH, INPUT_PULLUP);
    pinMode(TASTE_ENTER, INPUT_PULLUP);
    pinMode(SPIELENDE, OUTPUT);

    Serial.begin(9600);
}

void loop()
{
    // Zufallszahlen via Schlaufe .............................
    Zufallszahl++;
    if (Zufallszahl > 11)
    {
        Zufallszahl = 4;
    }
    delay(10);

    // Rand vorbelegen ........................................
    if (Spielbrett[1][1] == 0)
    {
        fill_outside();
    }

    // Eröffnungsentscheidung .................................
    if (Spieler == 0)
    {
        if (digitalRead(TASTE_GEGNER) == LOW)
        {
            delay(300);
            Spieler = 1;
        }
        if (digitalRead(TASTE_ICH) == LOW)
        {
            delay(300);
            Mein_Zufallszug();
        }
    }

    // Spiel ..................................................
    if (Game_is_over == 0)
    {
        switch (Spieler)
        {
        case 1:
        {
            Spieler_am_Zug();
            break;
        }
        case 2:
        {
            Mein_Zug();
            break;
        }
        }
    }

    // Spiel zu Ende ..........................................
    if (Game_is_over == 1)
    {
        do
        {
            digitalWrite(SPIELENDE, HIGH);
            delay(500);
            digitalWrite(SPIELENDE, LOW);
            delay(500);
        } while (Game_is_over == 0);
    }
}
// HAUPTPROGRAMM ENDE *****************************************************

void fill_outside() {
    int i, j;
    for (i = 1; i < 15; i++)
    {
        for (j = 1; j < 15; j++)
        {
            Spielbrett[i][j] = 0;
            if ((i < 5) or (i > 11))
            {
                Spielbrett[i][j] = -50;
            }
            if ((j < 4) or (j > 11))
            {
                Spielbrett[i][j] = -50;
            }
        }
    }
}

// Programm Mein_Zufallszug -----------------------------------------------
void Mein_Zufallszug()
{
    Spalte = Zufallszahl;
    for (int i = 5; i < 12; i++)
    {
        if ((Spielbrett[i][Spalte] == 0) and (Spielbrett[i + 1][Spalte] != 0))
        {
            Reihe = i;
        }
    }
    Spielbrett[Reihe][Spalte] = 1;
    Zug_ausfueren();
    Spieler = 1;
}

// Spieler am Zug ---------------------------------------------------------
void Spieler_am_Zug()
{
    if (Cursor == 0)
    {
        Cursor = 1;
        Reihe = 4;
        Spalte = 4;
        Cursor_Zeigen(1);
    }

    if (Cursor == 1)
    {
        if (digitalRead(TASTE_LINKS) == LOW)
        {
            delay(300);
            Cursor_Zeigen(0);
            Spalte--;
            if (Spalte < 4)
            {
                Spalte = 11;
            }
            Cursor_Zeigen(1);
        }

        if (digitalRead(TASTE_RECHTS) == LOW)
        {
            delay(300);
            Cursor_Zeigen(0);
            Spalte++;
            if (Spalte > 11)
            {
                Spalte = 4;
            }
            Cursor_Zeigen(1);
        }

        if (digitalRead(TASTE_ENTER) == LOW)
        {
            delay(300);
            for (int i = 5; i < 12; i++)
            {
                if ((Spielbrett[i][Spalte] == 0) and (Spielbrett[i + 1][Spalte] != 0))
                {
                    Reihe = i;
                }
            }
            Spielbrett[Reihe][Spalte] = 5;
            Cursor = 0;
            Spieler = 2;
            Zug_ausfueren();
            Vier_in_Reihe(20);
        }
    }
}

// Cursor Zeigen ----------------------------------------------------------
void Cursor_Zeigen(int EinAus)
{
    Feld_Nummer = (Reihe - 4) * 8 + (Spalte - 3) - 1;
    if (EinAus == 0)
    {
        strip.setPixelColor(Feld_Nummer, 0, 0, 0);
    }
    if (EinAus == 1)
    {
        strip.setPixelColor(Feld_Nummer, 0, 0, 255);
    }
    strip.show();
}

// Computer am Zug --------------------------------------------------------
void Mein_Zug()
{
    int Punkte_horizontal[2]; // 1 Arduino, 2 Gegenspieler
    int Punkte_vertikal[2];
    int Punkte_diagonal_L[2];
    int Punkte_diagonal_R[2];
    int Punkte_Zahl = 0;
    int Best_Punkte = 0;
    int Best_Reihe = 0;
    int Best_Spalte = 0;
    int Punkte_addiert;

    // Alle Felder durchsuchen ..............................
    for (Spalte = 4; Spalte < 12; Spalte++)
    {
        for (Reihe = 5; Reihe < 12; Reihe++)
        {
            if ((Spielbrett[Reihe][Spalte] == 0) and (Spielbrett[Reihe + 1][Spalte] != 0))
            {

                // Feldbewertung ..................................
                Punkte_horizontal[1] = 0;
                Punkte_horizontal[2] = 0;
                for (int i = -3; i < 1; i++)
                {
                    Punkte_addiert = Summe_Punkte_horizontal(Reihe, Spalte + i);
                    if ((Punkte_addiert > Punkte_horizontal[1]) and (Punkte_addiert < 5))
                    {
                        Punkte_horizontal[1] = Punkte_addiert;
                    }
                    if ((Punkte_addiert > Punkte_horizontal[2]) and (Punkte_addiert > 4))
                    {
                        Punkte_horizontal[2] = Punkte_addiert;
                    }
                }

                Punkte_vertikal[1] = 0;
                Punkte_vertikal[2] = 0;
                for (int i = -3; i < 1; i++)
                {
                    Punkte_addiert = Summe_Punkte_vertikal(Reihe + i, Spalte);
                    if ((Punkte_addiert > Punkte_vertikal[1]) and (Punkte_addiert < 5))
                    {
                        Punkte_vertikal[1] = Punkte_addiert;
                    }
                    if ((Punkte_addiert > Punkte_vertikal[2]) and (Punkte_addiert > 4))
                    {
                        Punkte_vertikal[2] = Punkte_addiert;
                    }
                }

                Punkte_diagonal_L[1] = 0;
                Punkte_diagonal_L[2] = 0;
                for (int i = -3; i < 1; i++)
                {
                    Punkte_addiert = Summe_Punkte_diagonal_L(Reihe + i, Spalte + i);
                    if ((Punkte_addiert > Punkte_diagonal_L[1]) and (Punkte_addiert < 5))
                    {
                        Punkte_diagonal_L[1] = Punkte_addiert;
                    }
                    if ((Punkte_addiert > Punkte_diagonal_L[2]) and (Punkte_addiert > 4))
                    {
                        Punkte_diagonal_L[2] = Punkte_addiert;
                    }
                }

                Punkte_diagonal_R[1] = 0;
                Punkte_diagonal_R[2] = 0;
                for (int i = -3; i < 1; i++)
                {
                    Punkte_addiert = Summe_Punkte_diagonal_R(Reihe - i, Spalte + i);
                    if ((Punkte_addiert > Punkte_diagonal_R[1]) and (Punkte_addiert < 5))
                    {
                        Punkte_diagonal_R[1] = Punkte_addiert;
                    }
                    if ((Punkte_addiert > Punkte_diagonal_R[2]) and (Punkte_addiert > 4))
                    {
                        Punkte_diagonal_R[2] = Punkte_addiert;
                    }
                }

                // Beste Position ermitteln .......................
                Punkte_Zahl = 0;
                // 1 Pixel = 1 Punkt ..............................
                if (Punkte_horizontal[1] == 1)
                {
                    Punkte_Zahl = 1;
                }
                if (Punkte_vertikal[1] == 1)
                {
                    Punkte_Zahl = 1;
                }
                if (Punkte_diagonal_L[1] == 1)
                {
                    Punkte_Zahl = 1;
                }
                if (Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 1;
                }

                // Kreuzung 1 x 1 Pixel = 2 Punkte ................
                if (Punkte_horizontal[1] == 1 and Punkte_vertikal[1] == 1)
                {
                    Punkte_Zahl = 2;
                }
                if (Punkte_horizontal[1] == 1 and Punkte_diagonal_L[1] == 1)
                {
                    Punkte_Zahl = 2;
                }
                if (Punkte_horizontal[1] == 1 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 2;
                }
                if (Punkte_vertikal[1] == 1 and Punkte_diagonal_L[1] == 1)
                {
                    Punkte_Zahl = 2;
                }
                if (Punkte_vertikal[1] == 1 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 2;
                }
                if (Punkte_diagonal_L[1] == 1 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 2;
                }

                // 2 Pixel - 3 Punkte .............................
                if (Punkte_horizontal[1] == 2)
                {
                    Punkte_Zahl = 3;
                }
                if (Punkte_vertikal[1] == 2)
                {
                    Punkte_Zahl = 3;
                }
                if (Punkte_diagonal_L[1] == 2)
                {
                    Punkte_Zahl = 3;
                }
                if (Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 3;
                }

                // Kreuzung 2 x 1 Pixel = 4 Punkte ................
                if (Punkte_horizontal[1] == 2 and Punkte_vertikal[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_horizontal[1] == 1 and Punkte_vertikal[1] == 2)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_horizontal[1] == 2 and Punkte_diagonal_L[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_horizontal[1] == 1 and Punkte_diagonal_L[1] == 2)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_horizontal[1] == 2 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_horizontal[1] == 1 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_vertikal[1] == 2 and Punkte_diagonal_L[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_vertikal[1] == 1 and Punkte_diagonal_L[1] == 2)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_vertikal[1] == 2 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_vertikal[1] == 1 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_diagonal_L[1] == 2 and Punkte_diagonal_R[1] == 1)
                {
                    Punkte_Zahl = 4;
                }
                if (Punkte_diagonal_L[1] == 1 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 4;
                }

                // Kreuzung 2 x 2 Pixel = 5 Punkte ................
                if (Punkte_horizontal[1] == 2 and Punkte_vertikal[1] == 2)
                {
                    Punkte_Zahl = 5;
                }
                if (Punkte_horizontal[1] == 2 and Punkte_diagonal_L[1] == 2)
                {
                    Punkte_Zahl = 5;
                }
                if (Punkte_horizontal[1] == 2 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 5;
                }
                if (Punkte_vertikal[1] == 2 and Punkte_diagonal_L[1] == 2)
                {
                    Punkte_Zahl = 5;
                }
                if (Punkte_vertikal[1] == 2 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 5;
                }
                if (Punkte_diagonal_L[1] == 2 and Punkte_diagonal_R[1] == 2)
                {
                    Punkte_Zahl = 5;
                }

                // 3 Pixel beim Gegner = 6 Punkte ..................
                if (Punkte_horizontal[2] == 15)
                {
                    Punkte_Zahl = 6;
                }
                if (Punkte_vertikal[2] == 15)
                {
                    Punkte_Zahl = 6;
                }
                if (Punkte_diagonal_L[2] == 15)
                {
                    Punkte_Zahl = 6;
                }
                if (Punkte_diagonal_R[2] == 15)
                {
                    Punkte_Zahl = 6;
                }

                // 3 Pixel = 7 Punkte ..............................
                if (Punkte_horizontal[1] == 3)
                {
                    Punkte_Zahl = 7;
                }
                if (Punkte_vertikal[1] == 3)
                {
                    Punkte_Zahl = 7;
                }
                if (Punkte_diagonal_L[1] == 3)
                {
                    Punkte_Zahl = 7;
                }
                if (Punkte_diagonal_R[1] == 3)
                {
                    Punkte_Zahl = 7;
                }

                // Entscheidung ....................................
                if (Punkte_Zahl > Best_Punkte)
                {
                    Best_Punkte = Punkte_Zahl;
                    Best_Reihe = Reihe;
                    Best_Spalte = Spalte;
                }

                // Nichts gefunden .................................
                if (Best_Punkte == 0)
                {
                    Best_Reihe = Reihe;
                    Best_Spalte = Zufallszahl;
                }
            }
        }
    }

    Reihe = Best_Reihe;
    Spalte = Best_Spalte;
    Spielbrett[Reihe][Spalte] = 1;
    Zug_ausfueren();
    Spieler = 1;
    Cursor = 0;
    // gewonnen ? .............................................
    Vier_in_Reihe(4);
}

// Nach 4er-Kombination suchen --------------------------------------------
void Vier_in_Reihe(int Wert)
{
    int Summe = 0;

    for (int i = -3; i < 1; i++)
    {
        Summe = Summe_Punkte_horizontal(Reihe, Spalte + i);
        if (Summe == Wert)
        {
            Game_is_over = 1;
        }
    }

    for (int i = -3; i < 1; i++)
    {
        Summe = Summe_Punkte_vertikal(Reihe + i, Spalte);
        if (Summe == Wert)
        {
            Game_is_over = 1;
        }
    }

    for (int i = -3; i < 1; i++)
    {
        Summe = Summe_Punkte_diagonal_L(Reihe + i, Spalte + i);
        if (Summe == Wert)
        {
            Game_is_over = 1;
        }
    }

    for (int i = -3; i < 1; i++)
    {
        Summe = Summe_Punkte_diagonal_R(Reihe - i, Spalte + i);
        if (Summe == Wert)
        {
            Game_is_over = 1;
        }
    }
}

// ------------------------------------------------------------------------
int Summe_Punkte_horizontal(int R, int S)
{
    int Summe = 0;
    for (int i = S; i < S + 4; i++)
    {
        Summe = Summe + Spielbrett[R][i];
    }
    return Summe;
}

// ------------------------------------------------------------------------
int Summe_Punkte_vertikal(int R, int S)
{
    int Summe = 0;
    for (int i = R; i < R + 4; i++)
    {
        Summe = Summe + Spielbrett[i][S];
    }
    return Summe;
}

// ------------------------------------------------------------------------
int Summe_Punkte_diagonal_L(int R, int S)
{
    int Summe = Spielbrett[R][S] + Spielbrett[R + 1][S + 1] + Spielbrett[R + 2][S + 2] + Spielbrett[R + 3][S + 3];
    return Summe;
}

// ------------------------------------------------------------------------
int Summe_Punkte_diagonal_R(int R, int S)
{
    int Summe = Spielbrett[R][S] + Spielbrett[R - 1][S + 1] + Spielbrett[R - 2][S + 2] + Spielbrett[R - 3][S + 3];
    return Summe;
}

// Zug ausführen **********************************************************
void Zug_ausfueren()
{
    for (int i = 4; i < Reihe + 1; i++)
    {
        Feld_Nummer = (i - 4) * 8 + (Spalte - 3) - 1;
        for (int j = 1; j < 4; j++)
        {
            strip.setPixelColor(Feld_Nummer, 0, 0, 255);
            strip.show();
            delay(50);
            strip.setPixelColor(Feld_Nummer, 0, 0, 0);
            strip.show();
            delay(50);
        }
    }
    // Rot Arduino  .............................................
    if (Spielbrett[Reihe][Spalte] == 1)
    {
        strip.setPixelColor(Feld_Nummer, 255, 0, 0);
    }
    // Grün Spieler  ............................................
    if (Spielbrett[Reihe][Spalte] == 5)
    {
        strip.setPixelColor(Feld_Nummer, 0, 255, 0);
    }
    strip.show();
}