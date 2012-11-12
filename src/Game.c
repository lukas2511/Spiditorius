#include <game/Game.h>
#include <game/Debug.h>
#include <game/Filesystem.h>
#include <game/Accelerometer.h>
#include <game/Font.h>

#include <stdio.h>
#include <string.h>
#include <stm32f4xx/stm32f4xx_rng.h>
#include <math.h>
#include "../inc/Sprites.h"

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

int x=5;
int y=5;

void Init(struct Gamestate* state)
{
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

void Update(uint32_t a)
{
    snes_button_state_t controller_state = GetControllerState1();
    if(controller_state.buttons.Up){
        y--;
    }
    if(controller_state.buttons.Down){
        y++;
    }
    if(controller_state.buttons.Left){
        x--;
    }
    if(controller_state.buttons.Right){
        x++;
    }

    if(x<-8) x=320+x;
    if(y<-8) y=200+y;
    if(x>320) x=-8;
    if(y>200) y=-8;
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);

    if(x<0){
        DrawRLEBitmap(b, spider, 320+x, y);
    }
    if(y<0){
        DrawRLEBitmap(b, spider, x, 200+y);
        if(x<0){
            DrawRLEBitmap(b, spider, 320+x, 200+y);
        }
    }
    if(x>312){
        DrawRLEBitmap(b, spider, -(320-x), y);        
    }
    DrawRLEBitmap(b, spider, x, y);
}
