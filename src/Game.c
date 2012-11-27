#include <game/Game.h>
#include <Debug.h>
#include <game/Filesystem.h>
#include <game/Accelerometer.h>
#include <game/Font.h>
#include <Collision.h>
#include <Random.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f4xx/stm32f4xx_rng.h>
#include <math.h>
#include "../inc/Sprites.h"

#define timers_count 20

void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);
inline void enim_add();
inline void enim_move();
inline void food_add();
void colltest();
void GameOver();

RLEBitmap const* spider_thing();

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

int x=280;
int y=5;

uint32_t timers[timers_count];

uint32_t buttonTimes[2];


//gegner
uint32_t enim_pos_x[10];
uint32_t enim_pos_y[10];
uint32_t enim_dir[10];
uint32_t enim_count;

uint32_t food_x;
uint32_t food_y;

uint32_t points;

RLEBitmap* enim_sprite[10];



uint32_t last_button_state=0;

uint32_t buttonsPressedTime(snes_button_state_t controller_state, uint32_t pushtime, uint32_t timer){
    if(controller_state.raw == 0){
        timers[timer]=0;
        return 0;
    }else{
        if(controller_state.raw == last_button_state){
            if( timers[timer] > pushtime){
                timers[timer] = 0;                
                return 1;
            }else{
                return 0;
            }
        }else{
            last_button_state = controller_state.raw;
            return 0;
        }
    }
}

void Init(struct Gamestate* state)
{
    for(uint32_t timer=0;timer<timers_count;timer++){
        timers[0]=0;
    }
    GetRandomInteger();
    enim_count = 0;
    points = 0;
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}



uint32_t spider_anim=1;

uint32_t sprite=1;

uint32_t direction=2;

uint32_t new_direction(snes_button_state_t controller_state){
    uint32_t newdirection=direction;
    if(controller_state.buttons.Left || controller_state.buttons.A){
        if(direction == 0) newdirection = 11;
        else newdirection--;
    }
    if(controller_state.buttons.Right || controller_state.buttons.B){
        if(direction == 11) newdirection = 0;
        else newdirection++;
    }
    return newdirection;
}

void move_spider(uint32_t direction){
    if(timers[0]>10){
        spider_anim++;
        timers[0]=0;
    }

    if(timers[3] > 4){
        timers[3] = 0;
        switch(direction){
            case 0: x+=3; break;
            case 1: x +=2; y +=1; break;
            case 2: x +=1; y +=2; break;
            case 3: y +=3; break;
            case 4: x -=1; y +=2; break;
            case 5: x -=2; y +=1; break;
            case 6: x -=3; break;
            case 7: x -=2; y-=1; break;
            case 8: x -=1; y -=2; break;
            case 9: y -=3; break;
            case 10: y -=2; x +=1; break;
            case 11: y-=1; x+=2; break;
        }
    }
}

void colltest(){

    //in bearbeitung!!!!!!!!!!!!!

    //collision spider enim;
    for(int i = 0; i < enim_count; i++){
        if(Collision_Sprite_Sprite(enim_pos_x[i], enim_pos_y[i], enim_sprite[i], x, y, spider_thing() ))
            {
                GameOver();
            }      

    }
    



}

void enim_move(){
    //timer passt tempo der gegner an. Auf das diese nicht ruckeln...
    if(timers[5] > 6){
        timers[5] = 0;
    for(int i = 0; i < enim_count; i++){

        switch(direction){
            case 0: enim_pos_x[i]+=3; break;
            case 1: enim_pos_x[i] +=2; enim_pos_y[i] +=1; break;
            case 2: enim_pos_x[i] +=1; enim_pos_y[i] +=2; break;
            case 3: enim_pos_y[i] +=3; break;
            case 4: enim_pos_x[i] -=1; enim_pos_y[i] +=2; break;
            case 5: enim_pos_x[i] -=2; enim_pos_y[i] +=1; break;
            case 6: enim_pos_x[i] -=3; break;
            case 7: enim_pos_x[i] -=2; enim_pos_y[i]-=1; break;
            case 8: enim_pos_x[i] -=1; enim_pos_y[i] -=2; break;
            case 9: enim_pos_y[i] -=3; break;
            case 10: enim_pos_y[i] -=2; x +=1; break;
            case 11: enim_pos_y[i]-=1; x+=2; break;
        
    }      
    }


    }




}

const RLEBitmap const* spider_thing(){
    switch(spider_anim){
        case 1: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_1 : spider_u_anim_1 : spider_l_anim_1 : spider_r_anim_1;
        case 2: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_2 : spider_u_anim_2 : spider_l_anim_2 : spider_r_anim_2;
        case 3: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_3 : spider_u_anim_3 : spider_l_anim_3 : spider_r_anim_3;
        case 4: return (sprite!=4) ? (sprite!=3) ? (sprite==2) ? spider_d_anim_4 : spider_u_anim_4 : spider_l_anim_4 : spider_r_anim_4;
        default: return spider;
    }
}

void Update(uint32_t a)
{
    for(uint32_t timer=0;timer<timers_count;timer++){
        timers[timer]=timers[timer]+a;
    }

    for(int count=1;count<=8;count++){
        Collision_Sprite_Sprite(0, 0, spider_thing(), 10*count, 10*count, spider_thing());
    }


    snes_button_state_t controller_state = GetControllerState1();

    if(buttonsPressedTime(controller_state,10,2)){
        direction=new_direction(controller_state);
    }
    move_spider(direction);
    enim_move();

    if(x<-8) x=320+x;
    if(y<-8) y=200+y;
    if(x>312) x=-8;
    if(y>192) y=-8;
}

void enim_add(){
    enim_pos_x[enim_count] = GetRandomInteger() % 320;
    enim_pos_y[enim_count] = GetRandomInteger() % 200;
    enim_dir[enim_count] = GetRandomInteger() % 16;
    enim_count++;
}

void food_add(){
    food_x = GetRandomInteger() % 320;
    food_y = GetRandomInteger() % 200;
}

void draw_transition (const struct RLEBitmap * (*toDraw)(), Bitmap *b, int *x, int *y) {
	if((*x)<0 && (*y)>0 && (*y)<=184){
        // rand links
        DrawRLEBitmap(b, (*toDraw)(), 320+(*x), (*y)); // rechter rand
    }else if((*x)<0 && (*y)<0){
        // ecke oben links
        DrawRLEBitmap(b, (*toDraw)(), 320+(*x), (*y)); // oben rechts
        DrawRLEBitmap(b, (*toDraw)(), (*x), 200+(*y)); // unten links
        DrawRLEBitmap(b, (*toDraw)(), 320+(*x), 200+(*y)); // unten rechts
    }else if((*y)<0 && (*x)>0 && (*x)<=304){
        // rand oben
        DrawRLEBitmap(b, (*toDraw)(), (*x), 200+(*y)); // rand unten
    }else if((*y)<0 && (*x)>304){
        // ecke oben rechts
        DrawRLEBitmap(b, (*toDraw)(), -(320-(*x)), (*y)); // oben links
        DrawRLEBitmap(b, (*toDraw)(), (*x), 200+(*y)); // unten rechts
        DrawRLEBitmap(b, (*toDraw)(), -(320-(*x)), 200+(*y)); // unten links
    }else if((*x)>304 && (*y)>0 && (*y)<=184){
        // rand rechts
        DrawRLEBitmap(b, (*toDraw)(), -(320-(*x)), (*y)); // rand links
    }else if((*x)>304 && (*y)>184){
        // ecke unten rechts
        DrawRLEBitmap(b, (*toDraw)(), -(320-(*x)), (*y)); // unten links
        DrawRLEBitmap(b, (*toDraw)(), (*x), -(200-(*y))); // oben rechts
        DrawRLEBitmap(b, (*toDraw)(), -(320-(*x)), -(200-(*y))); // oben links
    }else if((*y)>184 && (*x)>0 && (*x)<=304){
        // rand unten
        DrawRLEBitmap(b, (*toDraw)(), (*x), -(200-(*y))); // rand oben
    }else if((*x)<0 && (*y)>184){
        // ecke unten links
        DrawRLEBitmap(b, (*toDraw)(), 320+(*x), (*y)); // unten rechts;
        DrawRLEBitmap(b, (*toDraw)(), (*x), -(200-(*y))); // oben links;
        DrawRLEBitmap(b, (*toDraw)(), 320+(*x), -(200-(*y))); // oben rechts;
    }
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);
    DrawFilledRectangle(b, 0, 0, 320, 200, RGB(255,255,255));
    
	draw_transition (&spider_thing, b, &x, &y);
	
	
    if(spider_anim>4) spider_anim=1;

    DrawRLEBitmap(b, spider_thing(), x, y);
}
