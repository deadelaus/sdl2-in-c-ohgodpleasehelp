#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#include <time.h>

#define GRAVITY 0.32f

typedef struct
{
  float x,y;
  float dx, dy;
  short life;
  char *name;
  int onLedge;
} Man;

typedef struct{
  int x, y;
} Star;

typedef struct{
  int x, y, w, h;
} Ledge;

typedef struct{

  //Players
  Man man;

  //Stars
  Star stars[100];

  //Ledges
  Ledge ledges[100];

  //Images
  SDL_Texture *star;
  SDL_Texture *manFrames[2];
  SDL_Texture *brick;

  //Renderer
  SDL_Renderer *renderer;
} GameState;

void loadGame(GameState* game){
    SDL_Surface *surface = NULL;
    //Load images and create rendering textures from them
    surface = IMG_Load("star.png");
    if(surface == NULL){
        printf("Cannot find star.png!\n\n");
        SDL_Quit();
        exit(1);
    }
    game->star = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("doomguy1.png");
    if(surface == NULL){
        printf("Cannot find doomguy1.png!\n\n");
        SDL_Quit;
        exit(1);
    }
    game->manFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("doomguy2.png");
    if(surface == NULL){
        printf("Cannot find doomguy2.ping!\n\n");
        SDL_Quit;
        exit(1);
    }
    game->manFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("brick.png");
    if(surface == NULL){
        printf("Cannot find brick.png!\n\n");
        SDL_Quit;
        exit(1);
    }
    game->brick = SDL_CreateTextureFromSurface(game->renderer, surface);
    SDL_FreeSurface(surface);

    game->man.x = 280;
    game->man.y = 200;
    game->man.dy = 0;
    game->man.dx = 0;
    game->man.onLedge = 0;

    //init stars;
    for(int i = 0; i < 100; i++)
    {
        game->stars[i].x = rand()%640;
        game->stars[i].y = rand()%480;
    }

    //init ledges
    for(int i = 0; i < 100; i++)
    {
        game->ledges[i].w = 256;
        game->ledges[i].h = 64;
        game->ledges[i].x = i*256;
        game->ledges[i].y = 400;
    }
    game->ledges[98].x = 2;
    game->ledges[98].y = 200;
    game->ledges[99].x = 350;
    game->ledges[99].y = 200;
}

void process(GameState *game)
{
    Man *man = &game->man;
    man->y += man->dy;
    man->x += man->dx;

    man->dy += GRAVITY;
}


void collisionDetect(GameState *game)
{
    //Check for collision with any of the ledges
    for(int i = 0; i < 100; i++)
    {
        float mw = 72, mh = 72;
        float mx = game->man.x, my = game->man.y;
        float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

        if(my+mh > by && my < by+bh)
        {
            //Rubbing against right edge
            if(mx < bx+bw && mx+mw > bw+bw && game->man.dx < 0)
            {
              //correct x
              game->man.x = bx+bw;
              mx = bx+bw;
              game->man.dx = 0;
            }
            //rubbing against left edge
            else if(mx+mw > bx && mx < bx && game->man.dx > 0)
            {
                //correct x
                game->man.x = bx-mw;
                mx = bx-mw;
                game->man.dx = 0;
            }
        }
        if(mx+mw/2 > bx && mx+mw/2 < bx+bw)
        {
            //are we bumping our head
            if(my < by+bh && my > by && game->man.dy < 0)
            {
              //correct y
              game->man.y = by+bh;
              my = by+bh;
              //bumped head, stop any jump velocity
              game->man.dy = 0;
              game->man.onLedge = 1;
            }
        }
        if(mx+mw > bx && mx < bx+bw)
        {
            //are we landing on the ledge
            if(my+mh > by && my < by && game->man.dy > 0)
            {
                //correct y
                game->man.y = by-mh;
                my = by-mh;
                //landed on the edge,stop and jump velocity
                game->man.dy = 0;
                game->man.onLedge = 1;
            }
        }
      }
    }


int processEvents(SDL_Window *window, GameState *game){
    SDL_Event event;
    int done = 0;
    while(SDL_PollEvent(&event)) //check for events
       {
        switch(event.type)
        {
           case SDL_WINDOWEVENT_CLOSE:
           {
            if(window)
            {
             SDL_DestroyWindow(window);
             window = NULL;
             done = 1;
            }
            }
            break;
           case SDL_KEYDOWN:
            {
             switch (event.key.keysym.sym)
              {
               case SDLK_ESCAPE:
               done = 1; //quit game
               break;
               case SDLK_UP:
                if(game->man.onLedge)
                {
                  game->man.dy = -12;
                  game->man.onLedge = 0;
                }
               }
            }
           break;
           case SDL_QUIT:
            done = 1;
            break;
          }
       }
     //Walking
     const Uint8 *state = SDL_GetKeyboardState(NULL);
     if (state[SDL_SCANCODE_LEFT] ){
       game->man.dx -= 0.5;
       if(game->man.dx < -6)
       {
           game->man.dx = -6;
       }
      }
     else if (state[SDL_SCANCODE_RIGHT] ){
       game->man.dx += 0.5;
       if(game->man.dx > 6)
       {
           game->man.dx = 6;
       }
      }
      else
      {
        game->man.dx *= 0.8f;
        if(fabsf(game->man.dx)< 0.1f)
        {
            game->man.dx = 0;
        }
      }
     /*if (state[SDL_SCANCODE_UP] ){
       game->man.y -= 5;
      }
     if (state[SDL_SCANCODE_DOWN] ){
       game->man.y += 5;
      }*/
  return done;
}

void doRender(SDL_Renderer *renderer, GameState *game){
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); //renderer pointer,red value,green value,blue value,opacity value. Set draw color to red

    SDL_RenderClear(renderer); //Clear the screen (to red)

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //Set draw color to white

    for(int i = 0; i<100; i++)
    {
        SDL_Rect ledgeRect = {game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h};
        SDL_RenderCopy(renderer, game->brick, NULL, &ledgeRect);
    }

    //draw rectangle at mans position
    SDL_Rect rect = {game->man.x, game->man.y, 72, 72}; //rectangle left upper corner x,y coordinates;width,height
    SDL_RenderCopyEx(renderer, game->manFrames[0], NULL, &rect, 0, NULL, 0);

    //draw star image
    /*for(int i = 0; i < 100; i++)
    {
    SDL_Rect starRect = {game->stars[i].x, game->stars[i].y, 64, 64}; //star x,y positions;width,height
    SDL_RenderCopy(renderer, game->star, NULL, &starRect);
    }*/

    SDL_RenderPresent(renderer); //Done drawing, "present"(show) the rectangle
}

int main(int argc, char *argv[]){
    GameState gameState;
    SDL_Window *window = NULL;       //Declare Window
    SDL_Renderer *renderer = NULL;   //Declare Renderer

    SDL_Init(SDL_INIT_VIDEO);  //Initialize SDL2

    srand((int)time(NULL));

    window = SDL_CreateWindow("Game Window",    //Window title
                             SDL_WINDOWPOS_UNDEFINED, //initial x position
                             SDL_WINDOWPOS_UNDEFINED, //initial y position
                             640, //width in pixels
                             480, //length in pixels
                             0 //flags
                             );

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    gameState.renderer = renderer;
    loadGame(&gameState);

     //The window is open: enter program loop (See SDL_PollEvenr)
    int done = 0;

    //event loop
    while(!done)
    {
      //check for events
      done = processEvents(window, &gameState);

      process(&gameState);

      collisionDetect(&gameState);

      //render display
      doRender(renderer, &gameState);

    //SDL_Delay(10); //Wait 100 miliseconds or 0.1 seconds before continuing the code so we can see the rectangle
    }
    //shutdown game and unload all memory
    SDL_DestroyTexture(gameState.star);

    SDL_DestroyWindow(window); //Close and destroy window
    SDL_DestroyRenderer(renderer);//Close and destroy renderer

    SDL_Quit(); //Clean up
    return 0;
}

