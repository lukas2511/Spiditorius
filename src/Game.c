/**
* \file Game.c
* \brief Contains the game. What else should be in here?
* \author Stefan
* \author Lukas Schauer
* \author Marvin Teichmann
* \author Benjamin
*/
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

typedef uint8_t timer_id_type;
#define timers_count 20
#define SPIDER_ANIMATION_TIMER_ID 0
#define SPIDER_MOVE_TIMER_ID 3
#define ENEMY_GENERIC_TIMER_ID 5
#define SNES_BUTTON_TIMER_ID 2


uint8_t paused = 0;
void Init(struct Gamestate*);
void OnEnter(struct Gamestate* state);
void OnLeave(struct Gamestate* state);
void Update(uint32_t);
void Draw(Bitmap *);
void enim_add();
void enim_move();
void food_add();
const RLEBitmap const* enim_thing();
void colltest();
void GameOver();

uint32_t gameover = 0;

RLEBitmap const* spider_thing();

Gamestate InitState = { Init, OnEnter, OnLeave, Update, Draw };
Game* TheGame = &(Game) {&InitState};

int x=280;
int y=5;

uint32_t timers[timers_count];

//gegner
int enim_pos_x[10];
int enim_pos_y[10];
uint32_t enim_dir[10];
uint32_t enim_dir_lenght[10];
uint32_t enim_count = 0;

uint32_t food_x;
uint32_t food_y;

uint32_t points;

RLEBitmap* enim_sprite[10];

//temp to add test enemys
uint16_t enim_test = 1;

int glob_i = 0;
/**
* \brief Returns whether the current snes controller state has been stable for \a pushtime miliseconds
* \param controller_state is the current controller state
* \param pushtime is the threshhold needed to acknowledge the current state
* \param timer is the global timer which is going to be used
* \note The last button state is saved inside of the function.
*/
uint32_t buttonsPressedTime(snes_button_state_t controller_state, uint32_t pushtime, timer_id_type timer){
    static uint32_t last_button_state = 0;
    if(controller_state.raw == 0){
        timers[timer]=0;
        return 0;
    } else {
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
        timers[timer]=0;
    }
    GetRandomInteger();
    enim_count = 0;
    points = 0;
    gameover = 0;
}

void OnEnter(struct Gamestate* state)
{
}

void OnLeave(struct Gamestate* state)
{
}

uint32_t spider_anim = 1;
uint32_t sprite      = 1;
uint32_t direction   = 2;

/**
* \brief Updates the direction of the spider according to the given
*        \a controller_state
* \note The controller state should be verified with buttonsPressedTime()
*       first.
*/
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

/**
* \brief Updates the position of the spider according to the given \a direction.
*/
void move_spider(uint32_t direction){
    if(timers[SPIDER_ANIMATION_TIMER_ID] > 10){
        spider_anim++;
        timers[SPIDER_ANIMATION_TIMER_ID] = 0;
    }

    if(timers[SPIDER_MOVE_TIMER_ID] > 4){
        timers[SPIDER_MOVE_TIMER_ID] = 0;
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
	if(x<-8) x=320+x;
    if(y<-8) y=200+y;
    if(x>312) x=-8;
    if(y>192) y=-8;
}

/**
* \brief Tests whether the spider collides with an enemy
* \post GameOver state if the spider collides with an enemy
* \todo Test collision and implement GameOver
*/
void colltest(){
    //collision spider enim;

    // 2012-11-29 -- 15:18: Replaced local variable with global, so
    // that we can use enim_thing()
    for(glob_i = 0; glob_i < enim_count; glob_i++){
        if(Collision_Sprite_Sprite(enim_pos_x[glob_i], enim_pos_y[glob_i], enim_thing(), x, y, spider_thing()))
            {
                GameOver();
            }
    }
}

void enim_move(){
    //timer passt tempo der gegner an. Auf das diese nicht ruckeln...
    if(timers[ENEMY_GENERIC_TIMER_ID] > 6){
        timers[ENEMY_GENERIC_TIMER_ID] = 0;
    for(int i = 0; i < enim_count; i++){

		if (enim_dir_lenght[i] == 0) {
			enim_dir[i] = GetRandomInteger() % 11;
			enim_dir_lenght[i] = GetRandomInteger() % 16;
		} else enim_dir_lenght[i]--;

        switch(enim_dir[i]){
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
            case 10: enim_pos_y[i] -=2; enim_pos_y[i] +=1; break;
            case 11: enim_pos_y[i]-=1; enim_pos_y[i] +=2; break;
		}
		if(enim_pos_x[i]<-8) enim_pos_x[i]=320+enim_pos_x[i];
		if(enim_pos_y[i]<-8) enim_pos_y[i]=200+enim_pos_y[i];
		if(enim_pos_x[i]>312) enim_pos_x[i]=-8;
		if(enim_pos_y[i]>192) enim_pos_y[i]=-8;
    }


    }




}

const RLEBitmap const* spider_thing(){
	if (direction <= 1 || direction >= 11) sprite = 1;
	if (direction <= 4 && direction >= 2) sprite = 2;
	if (direction <= 7 && direction >= 5) sprite = 3;
	if (direction <= 10 && direction >= 8) sprite = 4;
	switch(sprite) {
		case 1: switch(spider_anim) {
			case 1: return spider_r_anim_1;
			case 2: return spider_r_anim_2;
			case 3: return spider_r_anim_3;
			case 4: return spider_r_anim_4;
		}; break;
		case 2: switch(spider_anim) {
			case 1: return spider_d_anim_1;
			case 2: return spider_d_anim_2;
			case 3: return spider_d_anim_3;
			case 4: return spider_d_anim_4;
		}; break;
		case 3: switch(spider_anim) {
			case 1: return spider_l_anim_1;
			case 2: return spider_l_anim_2;
			case 3: return spider_l_anim_3;
			case 4: return spider_l_anim_4;
		}; break;
		case 4: switch(spider_anim) {
			case 1: return spider_u_anim_1;
			case 2: return spider_u_anim_2;
			case 3: return spider_u_anim_3;
			case 4: return spider_u_anim_4;
		}; break;
	}
}

//temp
const RLEBitmap const* enim_thing(){
	uint8_t waspdir = 0;
	if (enim_dir[glob_i] <= 1 || enim_dir[glob_i] >= 11) waspdir = 0;
	if (enim_dir[glob_i] <= 4 && enim_dir[glob_i] >= 2) waspdir = 1;
	if (enim_dir[glob_i] <= 7 && enim_dir[glob_i] >= 5) waspdir = 2;
	if (enim_dir[glob_i] <= 10 && enim_dir[glob_i] >= 8) waspdir = 3;


	switch (waspdir) {
		case 3: return (spider_anim == 1 || spider_anim == 3) ? wasp_u0 : wasp_u1;
		case 0: return (spider_anim == 2 || spider_anim == 4) ? wasp_r0 : wasp_r1;
		case 1: return (spider_anim == 1 || spider_anim == 3) ? wasp_d0 : wasp_d1;
		case 2: return (spider_anim == 2 || spider_anim == 4) ? wasp_l0 : wasp_l1;
	}
}
/**
* \brief Updates the current game state
* \param delta is the time that has passed since the last update.
*/
void Update(uint32_t delta)
{
    if(!paused){
		  for(timer_id_type timer = 0;  timer < timers_count; timer++){
		      timers[timer] = timers[timer] + delta;
		  }
    } else {
		  timers[SNES_BUTTON_TIMER_ID] += delta;
	 }

    //! \todo Remove self-inflicted collision
    for(int count=1;count<=8;count++){
        Collision_Sprite_Sprite(0, 0, spider_thing(), 10*count, 10*count, spider_thing());
    }

    snes_button_state_t controller_state = GetControllerState1();

    if(buttonsPressedTime(controller_state,10,SNES_BUTTON_TIMER_ID)){
        direction=new_direction(controller_state);
        if(controller_state.buttons.Start)
		      paused = !paused;
    }
    move_spider(direction);
    enim_move();
		colltest();
	if(enim_test > 0) {
		enim_add();
		enim_test--;
	}
}

//! \brief Adds a new enemy and makes sure that the new enemy is not in range of Ixi
void enim_add(){
    glob_i = enim_count;

    // Generate new positions for the enemy as long as the current would harm the player
    do{
        enim_pos_x[enim_count] = GetRandomInteger() % 320;
        enim_pos_y[enim_count] = GetRandomInteger() % 200;
        enim_dir[enim_count] = GetRandomInteger() % 16;
    	enim_dir_lenght[enim_count] = GetRandomInteger() % 16;
    }while(
        Collision_BB_BB(enim_pos_x[glob_i], enim_pos_y[glob_i], enim_thing()->width,       enim_thing()->height,
                        x - 5,              y - 5,              spider_thing()->width + 5, spider_thing()->height+ 5)
    );
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

void GameOver () {
	gameover++;
}

/**
* \brief Takes the given bitmap \a b and displays the current game state.
*/
void Draw(Bitmap *b)
{
    ClearBitmap(b);
    DrawFilledRectangle(b, 0, 0, 320, 200, RGB(255,255,255));

	draw_transition (&spider_thing, b, &x, &y);

    // 2012-11-29 -- 15:12: removed local redefinition of glob_i, as it is
    // needed and referenced by enim_thing!
	for (glob_i = 0; glob_i < enim_count; glob_i++) {
		draw_transition (&enim_thing, b, &(enim_pos_x[glob_i]), &(enim_pos_y[glob_i]));
		DrawRLEBitmap(b, enim_thing(), enim_pos_x[glob_i], enim_pos_y[glob_i]);
	}

    if(spider_anim>4) spider_anim=1;

    DrawRLEBitmap(b, spider_thing(), x, y);
	
	//Debug Output
	setFont(fontblack8);
	char Buffer[200];
	sprintf (Buffer, "%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i\n%i", direction, sprite, enim_dir[0], enim_pos_x[0], enim_pos_y[0], gameover, x, y, enim_count);
	DrawText(b, Buffer, 23, 42);
}
