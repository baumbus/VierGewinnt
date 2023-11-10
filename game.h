#ifndef GAME_H
#define GAME_H

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

#ifdef __AVR__
#include <avr/power.h>
#endif

#define PLAYER_ONE_LEFT 1
#define PLAYER_ONE_RIGHT 2
#define PLAYER_ONE_ENTER 3

#define PLAYER_TWO_LEFT 4
#define PLAYER_TWO_RIGHT 5
#define PLAYER_TWO_ENTER 6

#define RESET 7
#define START 8

#define LED_PIN 9
#define LED_COUNT 42

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal_I2C lcd(0x27, 16, 2);

enum CellState {
    EMPTY,
    PLAYER_ONE,
    PLAYER_TWO
};

enum GameState {
    NOT_FINISHED,
    PLAYER_ONE_WON,
    PLAYER_TWO_WON,
    TIE  
};

static CellState game_state[15][15];
static GameState game_result; 
static bool versus_com;
static bool game_done;



#endif