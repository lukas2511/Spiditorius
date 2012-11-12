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

struct spider_animation_t{
	const RLEBitmap* bitmaps[4];
	unsigned char count;
	unsigned char current;
} spider_animation = {{&spider_d_anim_1,&spider_d_anim_2,&spider_d_anim_3,&spider_d_anim_4},4,0};

inline RLEBitmap * getAnimation(spider_animation_t * an){
	RLEBitmap * tmp = an->bitmaps[an->current++];
	an->current %= an->count;
	return tmp;
}

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
    if(x>312) x=-8;
    if(y>192) y=-8;
}

void Draw(Bitmap *b)
{
    ClearBitmap(b);
    DrawFilledRectangle(b, 0, 0, 320, 200, RGB(255,255,255));

	RLEBitmap * spider_anim = getAnimation(&spider_animation);
	
    if(x<0 && y>0 && y<=184){
        // rand links
        DrawRLEBitmap(b, spider_anim, 320+x, y); // rechter rand
    }else if(x<0 && y<0){
        // ecke oben links
        DrawRLEBitmap(b, spider_anim, 320+x, y); // oben rechts
        DrawRLEBitmap(b, spider_anim, x, 200+y); // unten links
        DrawRLEBitmap(b, spider_anim, 320+x, 200+y); // unten rechts
    }else if(y<0 && x>0 && x<=304){
        // rand oben
        DrawRLEBitmap(b, spider_anim, x, 320+y); // rand unten
    }else if(y<0 && x>304){
        // ecke oben rechts
        DrawRLEBitmap(b, spider_anim, -(320-x), y); // oben links
        DrawRLEBitmap(b, spider_anim, x, 200+y); // unten rechts
        DrawRLEBitmap(b, spider_anim, -(320-x), 200+y); // unten links
    }else if(x>304 && y>0 && y<=184){
        // rand rechts
        DrawRLEBitmap(b, spider_anim, -(320+x), y); // rand links
    }else if(x>304 && y>184){
        // ecke unten rechts
        DrawRLEBitmap(b, spider_anim, -(320-x), y); // unten links
        DrawRLEBitmap(b, spider_anim, x, -(200-y)); // oben rechts
        DrawRLEBitmap(b, spider_anim, -(320-x), -(200-y)); // oben links
    }else if(y>184 && x>0 && x<=304){
        // rand unten
        DrawRLEBitmap(b, spider_anim, x, -(200-y)); // rand oben
    }else if(x<0 && y>184){
        // ecke unten links
        DrawRLEBitmap(b, spider_anim, 320+x, y); // unten rechts;
        DrawRLEBitmap(b, spider_anim, x, -(200-y)); // oben links;
        DrawRLEBitmap(b, spider_anim, 320+x, -(200-y)); // oben rechts;
    }

    DrawRLEBitmap(b, spider_anim, x, y);
}
