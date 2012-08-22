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

#ifndef MUSIC_H
#define MUSIC_H

#define u8 unsigned char
#define u16 unsigned int
#define u32 unsigned long int

#define N6_C		194		//C, 6th octave
#define N6_CD		183		//C sharp
#define N6_D		173		//D
#define N6_DD		163		//D sharp
#define N6_E		154
#define N6_F		145
#define N6_FD		137
#define N6_G		129
#define N6_GD		122
#define N6_A		115
#define N6_AD		109
#define N6_B		103

#define N7_C		97		//C, 7th octave
#define N7_CD		92
#define N7_D		86
#define N7_DD		82
#define N7_E		77
#define N7_F		73
#define N7_FD		69
#define N7_G		65
#define N7_GD		61
#define N7_A		58
#define N7_AD		54
#define N7_B		51

#define N_CROCHE	60		//= eighth-note
#define N_NOIRE		120		//= quarter-note
#define N_BLANCHE	240		//= Half-note

#define N_REST_BET_NOTES 10	//rest time between two notes



typedef static const struct music_score{
	u8 note;
	u8 duree;
} music_score;


void beep();
void init_music();
void play_music(music_score *score, u8 score_length);
void stop_music();
bit is_music_ON();

void timer4_isr() __interrupt (T4_VECTOR);
void timer3_isr() __interrupt (T3_VECTOR);

#endif
