#include "stm32f0xx.h"
#include <stdint.h>
#include <stdlib.h>
#include "lcd.h"
#include "support.h"

void check_OOB(int x, int y, int *dx, int *dy, int *gameOver);

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
void pic_subset(Picture *dst, const Picture *src, int sx, int sy)
{
    int dw = dst->width;
    int dh = dst->height;
    for(int y=0; y<dh; y++) {
        if (y+sy < 0)
            continue;
        if (y+sy >= src->height)
            break;
        for(int x=0; x<dw; x++) {
            if (x+sx < 0)
                continue;
            if (x+sx >= src->width)
                break;
            dst->pix2[dw * y + x] = src->pix2[src->width * (y+sy) + x + sx];
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


extern const Picture background; // A 240x320 background image
//extern const Picture BG_gameOver; // The play area
extern const Picture ball; // A 19x19 purple ball with white boundaries
extern const Picture rubble1;
extern const Picture explosion;
extern const Picture GOtext;
extern const Picture GObottomText;


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
    TempPicturePtr(tmp,40,40); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &ball, 0x000); // Overlay the ball

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
    nano_wait(90000000);
    framex += 40;
    }
}

//Generates the game over prompt and holds the game in an infinite loop
void generate_GameOver(){

    int tog = 1;
    int x = 120 - 70;
    int y = 170; //

    TempPicturePtr(tmp,145,20); // Create a temporary 29x29 image.
    TempPicturePtr(tmp2,145,20); // Create a temporary 29x29 image.

    pic_subset(tmp, &background, 0, 170); // Copy the background
    pic_subset(tmp2, &background, 0, 170-20); // Copy the background

    pic_overlay(tmp, 0,0, &GOtext, 0x000); // Overlay the top text
    pic_overlay(tmp2, 0,0, &GObottomText, 0x000); // Overlay the bottom text
    LCD_DrawPicture(0,0,&background);
    while(1){

        nano_wait(1000000);
        //This is overly complicated only because I wanted to have the prompt blink

            LCD_DrawPicture(x,y, tmp); // Draw
            LCD_DrawPicture(x,y-20, tmp2); // Draw




        for(int c=0; c<4; c++) {
                   drive_column(c);
                   //nano_wait(10000000); // wait 1 ms
                   int r = read_rows();
                   if (c == 2) { // column 2

                       if (r & 4) { // '5' re-center the ball
                           move_ball();
                           NVIC_SystemReset();


                       }
                   }



        }
    }
}


//Updates and draws the rubble as it goes from top to bottom
void update2_rubble(int x, int y)
{
    TempPicturePtr(tmp,30,30); // Create a temporary 29x29 image.
    pic_subset(tmp, &background, x-tmp->width/2, y-tmp->height/2); // Copy the background
    pic_overlay(tmp, 5,5, &rubble1, 0x000); // Overlay the rubble
    LCD_DrawPicture(x-tmp->width/2,y-tmp->height/2, tmp); // Draw

}

//the obstacle struct contains an (x,y) coordinate and a begin flag that indicates if it has started to move



void move_ball(  )
{
    //INITIALIZATIONS__________________________________________________
    // Draw the background.
    LCD_DrawPicture(0,0,&background);
    int x = 120;
    int y = 20;
    int dx=0;
    int dy=0;
    int gameOver = 0;


    int toggle = 0;

    update(x,y);

    //_________________________________________________________________
    startTimer();

    for(;;)
        for(int c=0; c<4; c++) {

            //toggle++; //Toggle for background scroll

            //initialize obstacles:


//                                   {
//                {randomX(), 340, 0},  {randomX(), 340, 0 },  {randomX(), 340, 0},  {randomX(), 340, 0 },  {randomX(), 340, 0 }
//                                   };




            drive_column(c);
            nano_wait(10000000); // wait 1 ms
            int r = read_rows();
            if (c==3) { // leftmost column
                if (r & 8) { // '1'
                    dy -= 1; dx -= 1;
                }
                if (r & 4) { // '4'
                    dx -= 1;
                }
                if (r & 2) { // '7'
                    dy += 1; dx -= 1;
                }
            } else if (c == 2) { // column 2
                if (r & 8) { // '2'
                    dy -= 1;
                }
                if (r & 4) { // '5' re-center the ball
                    //erase(x,y);
//                    dx = 1; dy = 1;
//                    x = 119; y = 159;

                }
                if (r & 2) { // '8'
                    dy += 1;
                }
            } else if (c == 1) { // column 3
                if (r & 8) { // '3'
                    dy -= 1; dx += 1;
                }
                if (r & 4) { // '6'
                    dx += 1;
                }
                if (r & 2) { // '9'
                    dy += 1; dx += 1;
                }
            }

            //OUT OF BOUNDS CHECK<><><><><><><>
            if (dx !=0 || dy != 0) {
                x += dx;
                y += dy;
                //______________________________
                //Scrolling background
                if (toggle % 3 == 1 ){

                    //LCD_DrawPicture(0,0,&background);

                } //else if (toggle % )


                //_______________________________

               if (gameOver == 1)
               {
                    x = 120;
                    y = 20;
                    generate_GameOver();

               }  else {
                check_OOB(x, y, &dx, &dy, &gameOver);
                update2(x,y);
                move_obstacles(x,y);

                //ya = ya - 1;
                //update2_AS(xa,ya, 25, 25);
               }


            }
        }
}

//NOTE: x and y coordinates passed are the coordinates OF THE SHIP and are used to check collisions
void move_obstacles(int sx,int sy)
{
    for(int i = 0 ; i < 5; i++)
    {
        if (rubble[i].begin == 1) {
            rubble[i].y -= 1;
            update2_rubble(rubble[i].x,rubble[i].y);

            //Collision Check
            if(abs(rubble[i].x - sx) < 20 &&  abs(rubble[i].y - sy) < 20)
            {
                update2_Explosion(rubble[i].x,rubble[i].y );
                update2_Explosion(sx, sy);
                generate_GameOver();
            }
            if(rubble[i].begin >= 320)
            {
                rubble[i].begin = 0;
                rubble[i].y  = 0;
            }
        }

    }

}

//Checks for Out Of Bounds
void check_OOB(int x, int y, int *dx, int *dy, int *gameOver) {

    if (x > 240 - 5 || x < 0 + 5){

                                update2_Explosion(x, y);
                                generate_GameOver();
                                gameOver = 1;

                            }
    if (y > 320 - 5 || y < 0 + 5){



                                update2_Explosion(x, y);
                                generate_GameOver();
                                gameOver = 1;

    }

}

int randomX(){

    srand(time(0));
    return (rand() % (230+1)) + 1; ;
}


