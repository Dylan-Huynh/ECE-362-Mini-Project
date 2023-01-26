/*
 * support.h
 *
 *  Created on: Apr 24, 2022
 *      Author: patton37
 */

#ifndef SUPPORT_H_
#define SUPPORT_H_

extern const Picture background; // A 240x320 background image
//extern const Picture BG_gameOver; // The play area
extern const Picture ball; // A 19x19 purple ball with white boundaries
extern const Picture rubble1;
extern const Picture explosion;
extern const Picture GOtext;
extern const Picture GObottomText;
extern const Picture scoreText;
extern const Picture numbers;

int dodges; // the players score that is displayed in game over


struct obstacle {
    int firstx;
    int x;
    int y;
    int begin;
};

#define VOICES 15

struct {
    uint8_t in_use;
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
} voice[VOICES];

struct ship {
    int sx;
    int sy;
    int dx;
    int dy;
};


struct obstacle rubble[5];
struct ship ship1;


void move_ball(void);
int check_OOB(void);
void move_obstacles(void);
void stop_song(void);
#endif /* SUPPORT_H_ */
