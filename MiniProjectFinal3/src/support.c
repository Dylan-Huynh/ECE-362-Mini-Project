#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"
#include "support.h"
#include "obstacles.h"
#include <math.h>
//int check_OOB();

static void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | ~(1 << (c + 4));
}

int read_rows()
{
    return (~GPIOC->IDR) & 0xf;
}

// Copy a subset of a large source picture into a smaller destination.
// sx,sy are the offset into the source picture.
void pic_subset(Picture *dst, const Picture *src, int shipx, int shipy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+shipy < 0)
            continue;
        if (y+shipy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+shipx < 0)
                continue;
            if (x+shipx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+shipy) + x + shipx];
        }
    }
}

// Overlay a picture onto a destination picture.
// xoffset,yoffset are the offset into the destination picture that the
// source picture is placed.
// Any pixel in the source that is the 'transparent' color will not be
// copied.  This defines a color in the source that can be used as a
// transparent color.
void pic_overlay(Picture *dst, int xoffset, int yoffset, const Picture *src, int transparent)
{
    for(int y=0; y<src->height; y++) {
        int dy = y+yoffset;
        if (dy < 0)
            continue;
        if (dy >= dst->height)
            break;
        for(int x=0; x<src->width; x++) {
            int dx = x+xoffset;
            if (dx < 0)
                continue;
            if (dx >= dst->width)
                break;
            unsigned short int p = src->pix2[y*src->width + x];
            if (p != transparent)
                dst->pix2[dy*dst->width + dx] = p;
        }
    }
}





// This C macro will create an array of Picture elements.
// Really, you'll just use it as a pointer to a single Picture
// element with an internal pix2[] array large enough to hold
// an image of the specified size.
// BE CAREFUL HOW LARGE OF A PICTURE YOU TRY TO CREATE:
// A 100x100 picture uses 20000 bytes.  You have 32768 bytes of SRAM.
#define TempPicturePtr(name,width,height) Picture name[(width)*(height)/6+2] = { {width,height,2} }
void erase(int x, int y)
{
    TempPicturePtr(tmp,29,29); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    //pic_overlay(tmp, 5,5, &ball, 0xffff); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}

void update(int x, int y)
{
    LCD_DrawPicture(x-ball.width/2,y-ball.height/2, &ball); // Draw the ball


}

void update2(int x, int y )
{
    TempPicturePtr(tmp,35,40); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp,8,10, &ball, 0x000); // Overlay the ball

    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
}
void update2_Explosion(int x, int y )
{
    int framex = 0;
    int framey= 0;

    TempPicturePtr(tmp,40,40); // Create a temporary
    TempPicturePtr(tmp2,40,40); // Create a temporary
    for(int i = 0 ; i < 12 ; i++){
        pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy part of the background

    pic_subset(tmp2, &explosion,framex ,framey); // Copy part of the explosion
    pic_overlay(tmp, 0,0, tmp2, 0x000); // Overlay the ball
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw
    nano_wait(100000000);
    framex += 40;
    }
}


void update2_Numbers(int x, int y )
{





}


//Generates the game over prompt and holds the game in an infinite loop
void generate_GameOver(){

    int tog = 1;
    int xx = 120 - 70;
    int yy = 170; //

    TempPicturePtr(tmp,145,20); // Create a temporary GOtext
    TempPicturePtr(tmp2,145,20); // Create a temporary GObottomText
    //TempPicturePtr(tmp3,61,10); //Create a temporary  numbers
    TempPicturePtr(tmp4,45,20); //Create a temporary scoreText

    pic_subset(tmp, &background, 0, 170); // Copy the background
    pic_subset(tmp2, &background, 0, 170-20); // Copy the background
    //pic_subset(tmp3, &background, 0 + 50 , 170-60);
    pic_subset(tmp4, &background, 0, 170-60);

    update2_Numbers(0 + 50, 170-60 );

    pic_overlay(tmp, 0,0, &GOtext, 0x000); // Overlay the top text
    pic_overlay(tmp2, 0,0, &GObottomText, 0x000); // Overlay the bottom text
    //pic_overlay(tmp3, 0,0, &numbers, 0x000); // Overlay the top text
    pic_overlay(tmp4, 0,0, &scoreText, 0x000); // Overlay the bottom text

//{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{

       int x = 100;
       int y = 320/2.0 - 70 ;
    int framex = 0;
       int framey= 0;

       LCD_DrawPicture(0,0,&background);
       TempPicturePtr(tmp5,7,10); // Create a temporary
       TempPicturePtr(tmp6,7,10); // Create a temporary

          //dodges = 1234;

          int length;
          for(int i = 1 ; i <= 20; i++)
          {
              if ((dodges / pow(10.0, i))< 1 ){
                  length = i - 1 ;
                  break;
              }
          }

          int current = 0;
          //int remaining;
          int offset;
          for(int i = 0  ; i <= length ; i ++) {
                current = dodges % 10;
                framex = 6 * (9 - current);
                offset =  6 * (i - 1);

                pic_subset(tmp5, &background, x-tmp5->width/2 , y-tmp5->height/2); // Copy part of the background
                pic_subset(tmp6, &numbers,framex ,framey); // Copy part of the explosion
                pic_overlay(tmp5, 0 ,0, tmp6, 0x000); // Overlay the ball
                LCD_DrawPicture((170-tmp5->width/2) + offset, 120-tmp5->height/2, tmp5); // Draw
                nano_wait(50000000);
                dodges /= 10;
          }






//              nums[j] = dodges %(length);
//              pic_subset(tmp5, &background, x-tmp5->width/2, y-tmp5->height/2); // Copy part of the background
//
//
//
//              pic_subset(tmp6, &numbers,framex ,framey); // Copy part of the explosion
//              pic_overlay(tmp5, 0,0, tmp6, 0x000); // Overlay the ball
//              LCD_DrawPicture(x-tmp5->width/2 + 6 * (length - i),y-tmp5->height/2, tmp5); // Draw


    //{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{



    //LCD_DrawPicture(0,0,&background);
    while(1){

        nano_wait(1000000);
        //This is overly complicated only because I wanted to have the prompt blink

            LCD_DrawPicture(xx,yy, tmp); // Draw
            LCD_DrawPicture(xx,yy-20, tmp2); // Draw
            LCD_DrawPicture(xx+140,yy-60, tmp4); // Draw
            //LCD_DrawPicture(xx+130,yy-60, tmp5); // Draw






        for(int c=0; c<4; c++) {
                   drive_column(c);
                   nano_wait(1000000); // wait 1 ms
                   int r = read_rows();
                   if (c == 2) { // column 2

                       if (r & 4) { // '5' re-center the ball

                           NVIC_SystemReset();
                       }
                   }
        }
    }
}


//Updates and draws the rubble as it goes from top to bottom
void update2_rubble(int x, int y)
{
    TempPicturePtr(tmp,20,30); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 0,0 , &rubble1, 0x000); // Overlay the rubble
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw

}

int foog = 10;
void move_ball(  )
{
    //INITIALIZATIONS__________________________________________________
    // Draw the background.


    int dx=0;
    int dy=0;
    int gameOver = 0;
    //_________________________________________________________________


    //Receive Direction Input
        for(int c=0; c<4; c++) {

            drive_column(c);
            nano_wait(5000000); // wait 1 ms
            int r = read_rows();
            if (c==3) { // leftmost column

                if (r & 4) { // '4'
                    ship1.dx += 1;
                }

            } else if (c == 2) { // column 2
                if (r & 8) { // '2'
                    ship1.dy += 1;
                }
                if (r & 4) { // '5' re-center the ball

                }
                if (r & 2) { // '8'
                    ship1.dy -= 1;
                }
            } else if (c == 1) { // column 3

                if (r & 4) { // '6'
                    ship1.dx -= 1;
                }

            }
        }

        int foog = 6;

        if(ship1.dy > foog ){
            ship1.dy = foog;
        } else if(ship1.dy < -foog )
        {
            ship1.dy = -foog;
        }

        if (ship1.dx > foog)
        {
            ship1.dx = foog;
        } else if(ship1.dx < -foog )
        {
            ship1.dx = -foog;
        }


        //Update Velocity based on input
            if (ship1.dx !=0 || ship1.dy != 0) {
                if(dy >= 2 || dx >= 2)
                {
                    ship1.sx += 1;
                    ship1.sy += 1;
                } else {
                    ship1.sx += ship1.dx ;
                    ship1.sy += ship1.dy;
                }
                }
            if (gameOver == 1) {
                   ship1.sx = 120;
                   ship1.sy = 20;
                   stop_song();
                   generate_GameOver();
            }

         //Move the ship based on input

            update2(ship1.sx,ship1.sy);




}


//Gees' Code w/ some of Drew's _-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-

int x_pos = 120;
float x_velo = 0;
int y_pos = 20;


float y_velo = 0;
int difficulty = 5;
int time = 0;
float width = 12;
#define NUM_OBSTACLES 10
Obstacle obj[NUM_OBSTACLES];
int num_objs = 0;
int dodges = 0;

int obj_height = 5;
int obj_width = 5;

void generate_obstacles()
{
                            //Seed rand with the timer

    srand(TIM14->CNT);
    for (int i = 0; i < NUM_OBSTACLES; i++) {

            obj[i].x_pos = rand() % 240;
            obj[i].y_pos = 360 ;
           obj[i].status = false;
       }




}




//NOTE: x and y coordinates passed are the coordinates OF THE SHIP and are used to check collisions
void move_obstacles(void)
{


        for(int j = 0; j < difficulty; j++) {
                      if (obj[j].status == false) {
                          obj[j].status = true;
                          obj[j].x_pos = rand() % 240 ;
                          obj[j].y_pos = 320;
                          //obj[j].x_vel = 0;
                          obj[j].y_vel = (rand() % (10) +1) ;  //(difficulty));

                          update2_rubble(obj[j].x_pos,obj[j].y_pos);

                          if (obj[j].y_vel  > 6){
                              obj[j].y_vel = 5;
                          }


                          break;
                      }
                  }








        for(int i = 0; i < NUM_OBSTACLES; i++) {
            obj[i].y_pos -= obj[i].y_vel;
                        update2_rubble(obj[i].x_pos, obj[i].y_pos);

            //OOB check
            if (obj[i].y_pos < -20 ) {
                obj[i].status = false;
                num_objs--;
                dodges++;
            }

            //Collision Check
            if((abs(obj[i].x_pos - ship1.sx) < 20) &&  (20 > abs(obj[i].y_pos - ship1.sy))  )
            {
            	stop_song();
                update2_Explosion(obj[i].x_pos, obj[i].y_pos);
                update2_Explosion(ship1.sx, ship1.sy);
                generate_GameOver();
            }

            //Update Position
            //obj[i].x_pos += obj[i].x_vel;


        }

  // update difficulty using timer every x number of seconds




    //return dodges;  // moves off the screen
}
//-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-
//Checks for Out Of Bounds
int check_OOB() {

    if (ship1.sx > 240 - 5 || ship1.sx < 0 + 5){
    	stop_song();
                                update2_Explosion(ship1.sx, ship1.sy);
                                generate_GameOver();
                                //gameOver = 1;
                                return 0;
                            }
    if (ship1.sy > 320 - 5 || ship1.sy < 0 + 5){


    	stop_song();
                                update2_Explosion(ship1.sx, ship1.sy);
                                generate_GameOver();
                                //gameOver = 1;
                                return 0;

    }

    return 1;

}

int randomX(){
    return ((rand() % (230+1)) + 1) ;
}


