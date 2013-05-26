#include "SDL.h"
#include "SDL_opengl.h"
#include <iostream>
#include <vector>
#include <sstream>

struct Brick {
  double h = 20;
  double w = 50;
  double x = 0;
  double y = 0;
  bool active = true;
};

struct Ball {
  double r = 15;
  double x = 50;
  double y = 50;
  double vx = 4;
  double vy = 5;
};

class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
};

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

bool checkBallCollision(Ball* a, Brick* b) {
    /*if (a->y + a->r >= b->y || a->y <= b->y + b->h) {
        a->vy = -a->vy;
    }
    else if (a->x + a->r >= b->x || a->x <= b->x + b->w)  {
        a->vx = -a->vx;
    }*/
  if ( a->y + a->r < b->y ) return false; //if A is more to the left than B
  else if ( a->y > b->y + b->h ) return false; //if A is more to the right than B
  else if ( a->x + a->r < b->x ) return false; //if A is higher than B
  else if ( a->x > b->x + b->w ) return false; //if A is lower than B

  return true; //There is a collision because none of above returned false
}

int main (int argc, char* args[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    //Set the size of components for OpenGL
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Set title of application
    SDL_WM_SetCaption("Breakout", NULL);
    //Set dimensions, colors, renderer
    SDL_SetVideoMode(600,400,32, SDL_OPENGL);

    //Set the screen's clear color
    //Double values 0-1
    //Red, Green, Blue, Alpha(transparency)
    glClearColor(1,1,1,1); //white

    //Set the lower left corner of viewport in pixels,
    //as well as the dimensions of the window
    glViewport(0,0,600,400);

    //Set OpenGL to smooth shading so things like gradients look nice
    glShadeModel(GL_SMOOTH);

    //Leting OpenGL know we're using 2D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();  //saves this option

    //Since 2D, we can disable depth checking
    glDisable(GL_DEPTH_TEST);

    //The frames per second
    const int FRAMES_PER_SECOND = 60;

    Brick player;
    Ball gameball;
    player.x = 250;
    player.y = 380;
    player.w = 100;

    int numBricks = 100;
    std::vector<Brick> bricks;

    for (int i = 0, x = 25, y = 10; i < numBricks; i++, x += 55) {
        Brick temp;

        if (x >= 575) {
            x = 25;
            y += 25;
        }
        temp.x = x;
        temp.y = y;
        bricks.push_back(temp);
    }

    bool fpsCap = true;
    //Keep track of the frame count
    int frame = 0;
    //Timer used to calculate the frames per second
    Timer fps;
    //Timer used to calculate the frames per second
    Timer totalfps;
    //Timer used to update the caption
    Timer update;

    bool isRunning = true;
    //Generic event var for handling events
    SDL_Event event;
    //Start the update timer
    update.start();
     //Start the frame timer
        totalfps.start();


    while(isRunning) {
        //Start the frame timer
        fps.start();

    //Event handling
        Uint8* keystate = SDL_GetKeyState(NULL);

        //continuous-response keys
        if(keystate[SDLK_a]) {
            player.x -= 5;
        }
        if(keystate[SDLK_d]) {
             player.x += 5;
        }

        //On event
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    //window x button was pressed
                    isRunning = false;
                    break;
                case SDL_KEYUP:
                    //a key was released
                    switch(event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            isRunning = false;
                            break;
                        case SDLK_LEFT:
                            player.x -= 5;
                            break;
                        case SDLK_RIGHT:
                            player.x += 5;
                            break;
                        default:
                            //unused key, do nothing
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

    //Game Logic
        //boundaries x axis
        if (player.x < 0) {
            player.x = 0;
        }
        else if (player.x + player.w > 600) {
            player.x = 600 - player.w;
        }

        if (gameball.x + gameball.r > 600 || gameball.x < 0) {
            gameball.vx = -gameball.vx;
        }

        if (gameball.y < 0 || gameball.y + gameball.r > 400) {
            gameball.vy = -gameball.vy;
        }

        /*
        if (gameball.y > player.y - player.h && gameball.x < player.x + player.w && gameball.x > player.x) {
            gameball.vy = -gameball.vy;
        }
        else if (gameball.x > player.x && gameball.y < player.y && gameball.y > player.y - player.h) {
            gameball.vx = -gameball.vx;
        }*/

        gameball.x += gameball.vx;
        gameball.y += gameball.vy;
        if (checkBallCollision(&gameball, &player)) {
            gameball.vy = -gameball.vy;
        }
        for ( int i = 0; i < bricks.size(); i++ ) {
            if (bricks[i].active && checkBallCollision(&gameball, &bricks[i])) {
                gameball.vy = -gameball.vy;
                bricks[i].active = false;
            }
        }

    //Render Logic
        //Clear the screen with set color
        glClear(GL_COLOR_BUFFER_BIT);
        //Gives us the 'grid' we draw stuff on
        glPushMatrix();
        //Set the matrix size by multiplying an orthographic matrix
        glOrtho(0,600,400,0,-1,1);

        //Set the color of primitives
        glColor4f(.5,1,1,1);

        //Draw paddle
        //Set the color of primitives
        glColor4f(.5,1,1,1);
        glBegin(GL_QUADS);//start drawing primitive
        glVertex2f(player.x, player.y);//each vertex of the rectangle
        glVertex2f(player.x, player.y + player.h);
        glVertex2f(player.x + player.w, player.y + player.h);
        glVertex2f(player.x + player.w, player.y);
        glEnd(); //end drawing


        //Draw ball
        glColor4f(1,0,0,1);
        glBegin(GL_QUADS);
        glVertex2f(gameball.x, gameball.y);
        glVertex2f(gameball.x + gameball.r, gameball.y);
        glVertex2f(gameball.x + gameball.r, gameball.y + gameball.r);
        glVertex2f(gameball.x, gameball.y + gameball.r);
        glEnd();

        //Render the bricks
        glColor4f(0,1,1,1);
        glBegin(GL_QUADS);

        for ( int i = 0; i < bricks.size(); i++ ) {
            if (bricks[i].active) {
                glVertex2f(bricks[i].x, bricks[i].y);
                glVertex2f(bricks[i].x + bricks[i].w, bricks[i].y);
                glVertex2f(bricks[i].x + bricks[i].w, bricks[i].y + bricks[i].h);
                glVertex2f(bricks[i].x, bricks[i].y + bricks[i].h);
            }
        }

        glEnd(); //End drawing

        //Pops the matrix back to render on the screen
        glPopMatrix();
        frame++;

        //If a second has passed since the caption was last updated
        if( update.get_ticks() > 1000 )
        {
            //The frame rate as a string
            std::stringstream caption;

            //Calculate the frames per second and create the string
            caption << "Average Frames Per Second: " << frame / ( totalfps.get_ticks() / 1000.f );

            //Reset the caption
            SDL_WM_SetCaption(caption.str().c_str(), NULL );

            //Restart the update timer
            update.start();
        }

        if(fpsCap == true && (fps.get_ticks() < 1000 / FRAMES_PER_SECOND)) {
            //Sleep the remaining frame time
            SDL_Delay((1000 / FRAMES_PER_SECOND) - fps.get_ticks());
        }

        //Pushes the write buffer to the screen
        SDL_GL_SwapBuffers();
    }

    //Wait on a key
    std::cin.get();
    SDL_Quit();
    return 0;
}
