/*
 * Copyright 2010 Emmanuel Roussel
 * http://rousselmanu.free.fr
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


//TIMER 3 and 4 are used for the music.

#include <cc1110.h>
#include "music.h"
#include "types.h"		//for u8
#include "clock.h"	//for sleepMillis()

u8 g_music_index, g_music_cnt;

music_score *g_score;
u8 g_score_length=0;	//0;

void beep(){		//simple beep.
	u8 i;
  P1DIR |= 1<<0;   //beeper on P1.0
	for(i=0; i<100; i++){	//beep!
		P1_0=0;
		clock_delayms(2);
		P1_0=1;
		clock_delayms(2);
	}
}

void init_music(){
	g_music_index=0;
	g_music_cnt=0;
}

void play_music(music_score *score, u8 score_length){
	g_score=score;
	g_score_length=score_length;
//--- setup Timer 4
	T4CC0 = g_score[0].note;	//load the first note.

    // Enables global interrupts and interrupts from Timer 4.
    EA = 1;
    T4IE = 1;

    /* Timer 4 control. Sets the prescaler divider value to 4, starts the Timer,
     * enables overflow interrupts, clears the Timer and sets the mode to
     * up-down.
     */
    T4CTL=0b11111110;
    //T4CTL = T4CTL_DIV_16 | T4CTL_START | T4CTL_OVFIM |
     //   T4CTL_CLR | T4CTL_MODE_MODULO;
	
//--- setup Timer 3
    T3CC0 = 0xFF;
    EA = 1;
    T3IE = 1;

    T3CTL=0b11111110;
}

void stop_music(){
	T3CTL=0;
	T4CTL=0;
}

bit is_music_ON(){
	return T4CTL!=0;
}




void timer4_isr() __interrupt (T4_VECTOR) {
	P1_0=1-P1_0;	//toggles the pin connected to the sounder
    T4IF = 0;		//Clears the CPU interrupt flag.
}

void timer3_isr() __interrupt (T3_VECTOR) {
	static bit pause=0;
	T3IF = 0;//Clears the CPU interrupt flag.
	
	if(pause==1){
		if(g_music_cnt>=N_REST_BET_NOTES){
			T4CC0=g_score[g_music_index].note;	//load the next note.
			pause=0;	//toggle
			g_music_cnt=0;
			return;
		}
	}else{
		if(g_music_cnt>=g_score[g_music_index].duree){	//we have played this note long enough
			g_music_index++;			//go to the next note in the music score
			if(g_music_index>=g_score_length){		//end of the music score.
				g_music_index=0;
				stop_music();
			}
			T4CC0=0;
			pause=1;	//toggle : stops a bit beetween 2 notes.
			
			g_music_cnt=0;
			return;
		}
	}
	
	g_music_cnt++;
}







