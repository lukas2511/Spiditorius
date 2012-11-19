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

int x=280;
int y=5;

uint32_t delay[3]={0,0,0};

int buttonTimes[2];

int buttonPressedCountStuff(int controller, int count){
    snes_button_state_t controller_state;
    // get controller state
    if(controller==0){
        controller_state = GetControllerState1();
    }else{
        controller_state = GetControllerState2();
    }
    // if no button is pressed, reset counter
    if(controller_state.raw == 0){
        buttonTimes[controller]=0;
    // otherwise update counter
    }else{
        buttonTimes[controller]++;
    }

    // if counter has reached the given number, reset counter and return true value
    if(buttonTimes[controller]==count){
        buttonTimes[controller]=0;
        return 1;
    // otherwise just return false value
    }else{
        return 0;
    }
}

void Init(struct Gamestate* state)
{
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

int spider_anim=1;

int sprite=1;

int direction=2;

void Update(uint32_t a)
{
    delay[0]=delay[0]+a;
    delay[1]=delay[1]+a;
    delay[2]=delay[2]+a;

    snes_button_state_t controller_state = GetControllerState1();

    direction=new_direction();
    move_spider(direction,a);

    if(x<-8) x=320+x;
    if(y<-8) y=200+y;
    if(x>312) x=-8;
    if(y>192) y=-8;
}

int new_direction(snes_button_state_t controller_state){
    int newdirection=direction;
    if(controller_state.buttons.Left==1){
        newdirection=(newdirection-1) % 8;
    }
    if(controller_state.buttons.Right==1){
        newdirection=(newdirection+1) % 8;
    }
    if(newdirection==-1) newdirection=7;
    return newdirection;
}

void move_spider(int direction,uint32_t a){
    if(delay[0]>10){
        spider_anim++;
        delay[0]=0;
    }
    switch(direction){
        case 0: sprite=1; y--; break; // up
        case 1: sprite=1; x++; y--; break; // up right
        case 2: sprite=4; x++; break; // right
        case 3: sprite=2; x++; y++; break; // down right
        case 4: sprite=2; y++; break; // down
        case 5: sprite=2; y++; x--; break; // down left
        case 6: sprite=3; x--; break; // left
        case 7: sprite=1; x--; y--; break; // up left
    }
}

const RLEBitmap* const spider_thing(){
    switch(spider_anim){
        case 1: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_1 : spider_u_anim_1 : spider_l_anim_1 : spider_r_anim_1;
        case 2: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_2 : spider_u_anim_2 : spider_l_anim_2 : spider_r_anim_2;
        case 3: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_3 : spider_u_anim_3 : spider_l_anim_3 : spider_r_anim_3;
        case 4: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_4 : spider_u_anim_4 : spider_l_anim_4 : spider_r_anim_4;
        default: return spider;
    }
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);
    DrawFilledRectangle(b, 0, 0, 320, 200, RGB(255,255,255));
	
    if(x<0 && y>0 && y<=184){
        // rand links
        DrawRLEBitmap(b, spider_thing(), 320+x, y); // rechter rand
    }else if(x<0 && y<0){
        // ecke oben links
        DrawRLEBitmap(b, spider_thing(), 320+x, y); // oben rechts
        DrawRLEBitmap(b, spider_thing(), x, 200+y); // unten links
        DrawRLEBitmap(b, spider_thing(), 320+x, 200+y); // unten rechts
    }else if(y<0 && x>0 && x<=304){
        // rand oben
        DrawRLEBitmap(b, spider_thing(), x, 200+y); // rand unten
    }else if(y<0 && x>304){
        // ecke oben rechts
        DrawRLEBitmap(b, spider_thing(), -(320-x), y); // oben links
        DrawRLEBitmap(b, spider_thing(), x, 200+y); // unten rechts
        DrawRLEBitmap(b, spider_thing(), -(320-x), 200+y); // unten links
    }else if(x>304 && y>0 && y<=184){
        // rand rechts
        DrawRLEBitmap(b, spider_thing(), -(320-x), y); // rand links
    }else if(x>304 && y>184){
        // ecke unten rechts
        DrawRLEBitmap(b, spider_thing(), -(320-x), y); // unten links
        DrawRLEBitmap(b, spider_thing(), x, -(200-y)); // oben rechts
        DrawRLEBitmap(b, spider_thing(), -(320-x), -(200-y)); // oben links
    }else if(y>184 && x>0 && x<=304){
        // rand unten
        DrawRLEBitmap(b, spider_thing(), x, -(200-y)); // rand oben
    }else if(x<0 && y>184){
        // ecke unten links
        DrawRLEBitmap(b, spider_thing(), 320+x, y); // unten rechts;
        DrawRLEBitmap(b, spider_thing(), x, -(200-y)); // oben links;
        DrawRLEBitmap(b, spider_thing(), 320+x, -(200-y)); // oben rechts;
    }

    if(spider_anim>4) spider_anim=1;

    DrawRLEBitmap(b, spider_thing(), x, y);
}
