/*
 * Teste 2.3
 * Este programa e um prototipo do jogo Bubble Shooter
 * 17/10/16
 *
 * Nome: Eduardo Melo
 * DRE: 117200581
 *
 * Nome: Matheus Pinheiro
 * DRE: 117249179
 *
 * Grupo: Eduardo Melo, Matheus Pinheiro
 *
 */

/*Using SDL, SDL_image, standard IO, and strings*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#define DELAY 5

#define WAV_PATH "./Sounds/Kick-Drum-1.wav"
#define MUS_PATH "./Sounds/NES Ver. BAYONETTA - Fly Me To The Moon ( Climax Mix ).mp3"
#define TTF_PATH "./TTF/Graph-35-pix.ttf"

/*
 * Constants
 */

/*Screen dimension constants*/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

/*Border Width*/
const int BORDER = 24;

/*Ball moving speed*/
const int MSPEED = 8;

/*Ball collision radius*/
const int COLRADIUS = 8;

/*Amount of different colors for the balls*/
const int COLORS = 6;

/*Ballgrid size*/
const int BALLX = 20;
const int BALLY = 20;

/*Initial ballgrid*/
const int GRIDX = 19;
const int GRIDY = 6;

/*(true/false)*/
const int false = 0;
const int true = 1;

/*Image size constants*/
const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;

typedef struct _BACKGROUND
{
  SDL_Surface* image;
} BACKGROUND;

typedef struct _PLAYER
{
    float posX;
    float posY;
    float stepX;
    float stepY;
    SDL_Surface* image;
    int color;
    int imgW;
    int imgH;
    float centerX;
    float centerY;
} PLAYER;

typedef struct _NPC
{
    float posX;
    float posY;
    int indexX;
    int indexY;
    int imgW;
    int imgH;
    SDL_Surface* image;
    int color;
    float distX;
    float distY;
    float dist;
    float centerX;
    float centerY;
    int coltype;
    int remain;
} NPC;

typedef struct _UIELEMENT
{
  float posX;
  float posY;
  int color;
  SDL_Surface* image;
} UIELEMENT;

/*
 * Global Variables
 */

int clicked = 0;
int play = 0;
int quit = 0;
int health = 0;
int maxhealth = 0;
int Sound;
int Score;
char NickString[16];

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/* Our wave file */
Mix_Chunk *wave = NULL;

/* Our music file */
Mix_Music *music = NULL;

/*The image character*/
PLAYER ball;

/*The next ball color*/
UIELEMENT nextball;

/*Life elements*/
UIELEMENT lifeballs[5];

/*Sound on/off element*/
UIELEMENT SoundElement;

/*End Game element*/
UIELEMENT EGelement;

/*Main Menu Arrow*/
UIELEMENT ArrowElement;

/*PlayMode UI*/
UIELEMENT PMUI;

/*EndGame UI*/
UIELEMENT EGUI;

/*EndGame Sound*/
UIELEMENT EGR;

/*EndGame MainMenu*/
UIELEMENT EGMen;

/*EndGame Ranking*/
UIELEMENT EGRank;

/*Credits UI*/
UIELEMENT CredElement;

/*Ranking UI*/
UIELEMENT RankElement;

/*Ball Grid [Y][X]*/
NPC ballgrid[20][20];

int ballCount = 0;
int currentCount = 0;

int ThreatLevel = 1;

/*interfaces*/
int interface;

/*Ball surface*/
SDL_Surface* BallSurface;

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

/*Background*/
BACKGROUND backg;

/*Keeps the NPC where the check started*/
NPC* destructionStart = NULL;

/*
 * function prototypes
 */

/*Starts up SDL and creates window*/
int init();

/*Loads media*/
int loadMedia();

int getstring = 1;

/*Frees media and shuts down SDL*/
void closing();

/*Loads individual image*/
SDL_Surface* loadSurface( char *path );

/*Create PLAYER*/
PLAYER createPLAYER( float posX, float posY, float stepX, float stepY, int color, SDL_Surface *image);

/*Create NPC*/
NPC createNPC(float posY, float posX, int indexY, int indexX, int color, SDL_Surface *image);

/*Create UI element*/
UIELEMENT createELEMENT(float posX, float posY, int color, SDL_Surface *image);

/*Prepares grid*/
void prepareGrid();

/*Move PLAYER*/
void movePLAYER();

/*Gets ball color*/
SDL_Surface* GetColor(int color);

/*Gets Player Score*/
void GetScore();

/*Gets player nick string*/
void GetInput(SDL_Event e);

/*Gets Threat Level*/
void GetThreatLevel();

/*Gets Life Surface*/
void GetLifeSurface();

/*checks ball collision*/
NPC* checkCollision();

/*checks ball collision against the wall*/
void WallCollision();

/*checks ball collision  against the ceiling*/
NPC* CeilingCollision();

/*checks ball collision against an NPC*/
NPC* NPCCollision();

/*calls NPC and Ceiling Collision*/
NPC* collision();

/*Destroys the surrounding balls of the same color*/
void checkAround(NPC* npc, int checkcolor);

/*Checks if there are enough balls to trigger destruction*/
void checkDestruction(NPC* npc, int checkcolor);

/*Turns npc->remain to 1*/
void checkIsland(NPC* npc);

/*Destroys Islands*/
void DestroyIsland(int ScoreOn);

/*Prepares game initialization and variables*/
int PrepareGame();

/*Finishes play mode*/
int PlayEnd();

/*Checks play mode win*/
int PlayWin();

/*Prints the screen surface and its updates*/
void RefreshScreen(void);

/*Prints grid in the terminal*/
void printGrid();

/*Moves the grid down when called*/
void gridDown();

/*Creates grid*/
void createGrid();

/*Gets Background surface*/
void makeBACKGROUND();

/*Cleans grid*/
void cleanGrid();

/*Displays player on screen*/
void drawPLAYER(PLAYER p);

/*Displays NPC on screen*/
void drawNPC(NPC n);

/*Displays Background*/
void drawBACKGROUND(BACKGROUND b);

/*Displayes UI element on screen*/
void drawELEMENT(UIELEMENT u, int imageW, int imageH);

/*(tries to) draw a star*/
void DrawStar();

/*Game Function*/
void Game();

/*End Game UI Function*/
void EndGameUI();

/*Prepare play Function*/
void PreparePlay();

/*Play mode Function*/
void Play();

/*Main Menu Function*/
void MainMenu();

/*Highscores Function*/
void Highscores();

/*Credits Function*/
void Credits();

/*Sound Button Function*/
void Buttons(SDL_Event e);

/*Shoot Ball Player*/
void shoot();


int main( int argc, char* args[] )
{
    int errortest;

    /*Prepares game initialization and variables*/
    errortest = PrepareGame();

    if(errortest)
    {
      return errortest;
    }

    Mix_PlayMusic( music, -1);

    /*While application is running*/
    while( !quit )
    {
        Game();
    }

    /*Free resources and closing SDL*/
    closing();
    return 0;
}

void movePLAYER()
{
    ball.posX += ball.stepX;
    ball.posY += ball.stepY;
    ball.centerX = ball.posX + IMAGE_WIDTH/2;
    ball.centerY = ball.posY + IMAGE_HEIGHT/2;

    /*Checks if there's any wall collision for it inside the function*/
    WallCollision();

}

NPC* collision()
{
    /*char scoreString[16];*/
    NPC *n;

    /*Checks if there's any NPC collision for it inside the function*/
    n = NPCCollision();

    /*Checks if there's any ceiling collision for it inside the function*/
    if (n == NULL)
        n = CeilingCollision();
    if (n != NULL){
        /*printGrid();*/
        ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
        ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT) - 4;
        ball.stepY = 0;
        ball.stepX = 0;
        clicked = 0;
        health--;
        printGrid();
        GetThreatLevel();
        GetLifeSurface();

        if (PlayWin()){
            printf("\nnão fez mais que a sua obrigação\n");
        }

        printf("Score = %d\nThreatLevel = %d\n", Score, ThreatLevel);
    }
    return n;
}

void WallCollision()
{
    if ( (ball.posX + IMAGE_WIDTH > SCREEN_WIDTH - BORDER) ||
         (ball.posX < BORDER) )
    {
        ball.stepX =- ball.stepX;
        ball.posX += ball.stepX;
    }
}

NPC* CeilingCollision()
{
  int ballcolor;
  int newX;

  if (ball.posY < BORDER + 5)
  {
      newX = (int)((ball.posX) / IMAGE_WIDTH);
      if (ball.posX > newX*IMAGE_WIDTH + IMAGE_WIDTH/2) newX++;

      ballgrid[1][newX] = createNPC(
          IMAGE_HEIGHT-5,
          newX*IMAGE_WIDTH + IMAGE_WIDTH/4,
          1,
          newX,
          ball.color,
          ball.image
      );

      ball.image = NULL;

      ballCount = 0;
      currentCount = 0;
      checkDestruction(&ballgrid[1][newX], ballgrid[1][newX].color);
      drawNPC(ballgrid[1][newX]);

      ball.color = nextball.color;
      ball.image = nextball.image;
      ballcolor = rand() % COLORS + 1;
      nextball.color = ballcolor;
      nextball.image = GetColor(ballcolor);

      return &ballgrid[0][newX];
  }
  return NULL;
}

NPC* checkCollision()
{
    int i, j;
    float dist, distX, distY;

    for(i = 1; i < BALLY; i++)
    for (j = 1; j <= BALLX; j++)
    {
        if(ballgrid[i][j].color){

            distX = ballgrid[i][j].centerX - (ball.centerX);
            distY = ballgrid[i][j].centerY - (ball.centerY);
            dist = sqrt(pow(distX, 2) + pow(distY, 2));

            ballgrid[i][j].distX = distX;
            ballgrid[i][j].distY = distY;
            ballgrid[i][j].dist = dist;

            if (dist < IMAGE_WIDTH - COLRADIUS)
            {
                /*if(ball.color == ballgrid[i][j].color){*/

                /* printf("Ball color: %d\nBall Index: %d\n",ballgrid[i][j].color,j); */

                /* printf("ballgrid centerY = %f\nplayer centerY = %f\n", ballgrid[i][j].centerY, ball.centerY);*/
                  /*
                    COLTYPE CODES:
                         6 1
                        5 O 2
                         4 3
                  */
                  if (ball.centerX > ballgrid[i][j].centerX)
                  {
                    if (ball.centerY < IMAGE_HEIGHT/5 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 1;
                    else if (ball.centerY > (IMAGE_HEIGHT/3) * 2 + ballgrid[i][j].posY+1) ballgrid[i][j].coltype = 3;
                    else ballgrid[i][j].coltype = 2;
                  }

                  else
                  {
                    if (ball.centerY < IMAGE_HEIGHT/5 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 6;
                    else if (ball.centerY > (IMAGE_HEIGHT/3) * 2 + ballgrid[i][j].posY+1) ballgrid[i][j].coltype = 4;
                    else ballgrid[i][j].coltype = 5;
                  }

                  /* printf("coltype = %d\n", ballgrid[i][j].coltype); */
                  /* printf("player center: %f ballij center: %f\n", ball.centerX, ballgrid[i][j].centerX); */
                  return &ballgrid[i][j];
              }
          }
      }

    return NULL;
}

NPC* NPCCollision()
{
  int m, n; /* m = index added to NPC i ; n = index added to NPC j */
	int ballcolor;
	NPC *colNPC, *newNPC;

	colNPC = checkCollision();

  /*
    COLTYPE CODES:
         6 1
        5 O 2
         4 3
  */

    if (colNPC)
    {
      printf("Colidiu!\n");
        switch(colNPC->coltype)
        {
            case 1:
                if (colNPC->indexY%2 == 0){
                    m = -1;
                    n = 0;
                }
                else{
                    m = -1;
                    n = +1;
                }
            break;
            case 2:
                m = 0;
                n = +1;
            break;
            case 3:
                if (colNPC->indexY%2 == 0 || colNPC->indexX == 18){
                    m = +1;
                    n = 0;
                }
                else{
                    m = +1;
                    n = +1;
                }
            break;
            case 4:
                if (colNPC->indexY%2 == 0 && colNPC->indexX != 1){
                    m = +1;
                    n = -1;
                }
                else{
                    m = +1;
                    n = 0;
                }
            break;
            case 5:
                m = 0;
                n = -1;
            break;
            case 6:
                if (colNPC->indexY%2 == 0){
                    m = -1;
                    n = -1;
                }
                else{
                    m = -1;
                    n = 0;
                }
        }

        /*(making newNPC)*/
        ballgrid[(colNPC->indexY)+m][(colNPC->indexX)+n] = createNPC(
            ((colNPC->indexY)+m) * (IMAGE_HEIGHT-5),
            (colNPC->indexX) * IMAGE_WIDTH,
            ((colNPC->indexY)+m),
            ((colNPC->indexX)+n),
            ball.color,
            ball.image
        );
        newNPC = &ballgrid[(colNPC->indexY)+m][(colNPC->indexX)+n];
        /*from now on, newNPC can use this pointer*/

        ball.image = NULL;

        /*repositioning*/
        if(colNPC->coltype <= 3)
            newNPC->posX += IMAGE_WIDTH/2;
        if((colNPC->coltype == 2 || colNPC->coltype == 5) && ((colNPC->indexY)%2 == 1))
            newNPC->posX += IMAGE_WIDTH/2;
        if(colNPC->coltype == 5){
            newNPC->posX -= IMAGE_WIDTH/2;
        }
        newNPC->posX += n*IMAGE_WIDTH/2;

        newNPC->posX -= IMAGE_WIDTH/4;


        newNPC->centerX = newNPC->posX + IMAGE_WIDTH/2;
        newNPC->centerY = newNPC->posY + IMAGE_WIDTH/2;

        /*Checking if there's a ball over the border*/
        if (newNPC->posX < IMAGE_WIDTH && (newNPC->indexY)%2 == 1){
            newNPC->posX += IMAGE_WIDTH/2;
        }
        if (newNPC->posX > SCREEN_WIDTH-(BORDER + 3*IMAGE_WIDTH/2) && (newNPC->indexY)%2 == 0){
            newNPC->posX -= IMAGE_WIDTH/2;
        }

        ballCount = 0;
        currentCount = 0;
        checkDestruction(newNPC, newNPC->color);
    		colNPC->coltype = 0;

        ball.color = nextball.color;
        ball.image = nextball.image;
        ballcolor = rand() % COLORS + 1;
        nextball.color = ballcolor;
        nextball.image = GetColor(ballcolor);

        return newNPC;
    }
    return NULL;
}

/*Create UI element*/
UIELEMENT createELEMENT(float posX, float posY, int color, SDL_Surface *image)
{
  UIELEMENT u;

  u.posX = posX;
  u.posY = posY;
  u.color = color;
  u.image = image;
  return u;
}

/*Create PLAYER*/
PLAYER createPLAYER( float posX, float posY,
					 float stepX, float stepY,
					 int color, SDL_Surface *image)
{
    PLAYER p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.color = color;
    p.image = image;
    p.centerX = posX + IMAGE_WIDTH/2;
    p.centerY = posY + IMAGE_HEIGHT/2;
    return p;
}

/*Create NPC*/
NPC createNPC(float posY, float posX,
			  int indexY, int indexX,
			  int color, SDL_Surface *image)
{
	NPC n;

	n.posX = posX;
	n.posY = posY;
	n.indexY = indexY;
	n.indexX = indexX;
	n.color = color;
	n.image = image;
	n.centerX = posX + IMAGE_WIDTH/2;
	n.centerY = posY + IMAGE_HEIGHT/2;
	n.coltype = 0;
    n.remain = 0;

	return n;

}

/*makes BACKGROUND*/
void makeBACKGROUND()
{
  if (interface == 1) backg.image = loadSurface( "./Images/menuBG.png" );
  if (interface == 2) backg.image = loadSurface( "./Images/BG2.png" );
  if (interface == 3 || interface == 4) backg.image = loadSurface( "./Images/BG.png");
}

/*
    COLOR CODES
    0 = null
    1 = earth
    2 = saturn
    3 = neptune
    4 = venus
    5 = jupiter
    6 = mars
*/

/*
    INTERFACE CODES
    1 = main menu
    2 = play
    3 = highscores
    4 = credits
*/

SDL_Surface* GetColor(int color)
{
    SDL_Surface* ColorSurface;

    switch(color)
    {
        case 1:
            ColorSurface = loadSurface( "./Images/Earth.png" );
            break;
        case 2:
            ColorSurface = loadSurface( "./Images/Saturn.png" );
            break;
        case 3:
            ColorSurface = loadSurface( "./Images/Neptune.png" );
            break;
        case 4:
            ColorSurface = loadSurface( "./Images/Venus.png" );
            break;
        case 5:
            ColorSurface = loadSurface( "./Images/Jupiter.png" );
            break;
        case 6:
            ColorSurface = loadSurface( "./Images/Mars.png" );
            break;
    }

    return ColorSurface;
}

/*Clean Grid*/
void cleanGrid(){
    int i, j;

    for (i=1; i<BALLY; i++){
        for(j=1; j<BALLX; j++){
            ballgrid[i][j].indexX = 0;
            ballgrid[i][j].indexY = 0;
            ballgrid[i][j].posX = 0;
            ballgrid[i][j].posY = 0;
            ballgrid[i][j].centerX = 0;
            ballgrid[i][j].centerY = 0;
            ballgrid[i][j].color = 0;
        }
    }
}

/*Create Grid*/
void createGrid(int ballY)
{
	int i;

    /*LEMBRAR DE TROCAR ISTO QUANDO FOR PARA MATRIZ*/
	for (i = 1; i < ballY; i++)
	{
        gridDown();
        SDL_Delay(10*DELAY);
        RefreshScreen();
	}
}

/*Displays player on screen*/
void drawPLAYER(PLAYER p)
{
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = IMAGE_WIDTH;
	srcRect.h = IMAGE_HEIGHT;
	dstRect.x = p.posX;
	dstRect.y = p.posY;
	SDL_BlitSurface( p.image, &srcRect, gScreenSurface, &dstRect );
}

/*Displayes Background on screen*/
void drawBACKGROUND(BACKGROUND b)
{
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = SCREEN_WIDTH;
	srcRect.h = SCREEN_HEIGHT;
	dstRect.x = 0;
	dstRect.y = 0;
	SDL_BlitSurface( b.image, &srcRect, gScreenSurface, &dstRect );
}

/*Displayes UI element on screen*/
void drawELEMENT(UIELEMENT u, int imageW, int imageH)
{
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.w = imageW;
	srcRect.h = imageH;
	dstRect.x = u.posX;
	dstRect.y = u.posY;
	SDL_BlitSurface( u.image, &srcRect, gScreenSurface, &dstRect );
}

/*Displays NPC on screen*/
void drawNPC(NPC n)
{
    if(n.color)
    {
    	SDL_Rect srcRect, dstRect;
    	srcRect.x = 0; srcRect.y = 0;
        srcRect.w = IMAGE_WIDTH;
        srcRect.h = IMAGE_HEIGHT;
        dstRect.x = n.posX;
        dstRect.y = n.posY;
    	SDL_BlitSurface( n.image, &srcRect, gScreenSurface, &dstRect );
    }
}

void RefreshScreen()
{
    int i, j;
    drawBACKGROUND(backg);

    /*Main Menu Refresh Screen*/
    if (interface == 1){
        drawELEMENT(SoundElement, 38, 38);
        drawELEMENT(ArrowElement, 38, 38);
    }

    /*Play Refresh Screen*/
    if(interface == 2){
        GetScore();
        drawELEMENT(PMUI, SCREEN_WIDTH, SCREEN_HEIGHT);
        drawPLAYER(ball);
        drawELEMENT(SoundElement, 38, 38);
        drawELEMENT(EGelement, 38, 38);
        for (i=0; i < health; i++){
            drawELEMENT(lifeballs[i], 8, 10);
        }

        for (i = 0; i < BALLY; i++)
            for (j = 0; j < BALLX; j++)
                drawNPC(ballgrid[i][j]);
        drawELEMENT(nextball, IMAGE_WIDTH, IMAGE_HEIGHT);

        if(play == -1){
            drawELEMENT(EGUI, 200, 200);
            drawELEMENT(EGMen, 38, 38);
            drawELEMENT(EGR, 38, 38);
            drawELEMENT(EGRank, 38, 38);
        }

    }

    /*Highscores Refresh Screen*/
    if(interface == 3){
        drawELEMENT(RankElement, 440, 440);
        drawELEMENT(EGelement, 38, 38);
    }

    /*Highscores Refresh Screen*/
    if(interface == 4){
        drawELEMENT(CredElement, 300, 200);
        drawELEMENT(EGelement, 38, 38);
    }

    /*Update the surface*/
    SDL_UpdateWindowSurface( gWindow );

    /* Not so good solution, depends on your computer*/
    SDL_Delay(DELAY);
}


void shoot(){
    int Mx, My;

    SDL_GetMouseState( &Mx, &My );

    /*Click nao funciona se My dentro de safe zone*/
    if(My > SCREEN_HEIGHT- 45 ) return;

    /*Reinterpretando Mx e My com origem no centro de ball*/
    Mx = Mx - SCREEN_WIDTH/2;
    My = SCREEN_HEIGHT - My - IMAGE_HEIGHT/2;

    /*Reinterpretando x e y para que a hipotenusa seja 1 (calculando o step)*/
    ball.stepX = Mx / sqrt((Mx*Mx) + (My*My));
    ball.stepY = -My / sqrt((Mx*Mx) + (My*My));

    /* CASO NECESSARIO, SABER QUAL STEPX E STEPY
    printf("step X = %f\nstep Y = %f\n", ball.stepX, ball.stepY);
    printf("%d, %d\n", Mx, My);
    */

    /*
     * CRIANDO UMA EXCEÇÃO PARA ANGULOS EXTREMOS (PROXIMOS DE ACIMA DE 172 E ABAIXO DE 8 GRAUS)
     * sen 8 = 0.99
     * cos 8 = -0.139
     */

    if (ball.stepX > 0.997)
    {
    	ball.stepX = 0.997;
    	ball.stepY = -0.0774;
    }

    if (ball.stepX < -0.997)
    {
    	ball.stepX = -0.997;
    	ball.stepY = -0.0774;
    }

    ball.stepY*= MSPEED;
    ball.stepX*= MSPEED;
    clicked = 1;
    /* P/ TESTAR A BOLINHA IR RETO
      ball.stepX = 0;
      ball.stepY = -8; */
}

void Game(){

    if(!Sound) Mix_VolumeMusic(0);
    else Mix_VolumeMusic(100);

    switch(interface){
        case 1: /*MainMenu*/
            MainMenu();
            break;
        case 2: /*Play*/
            Play();
            break;
        case 3: /*Highscores*/
            Highscores();
            break;
        case 4: /*Credits*/
            Credits();
            break;
    }
    RefreshScreen();
}

void MainMenu(){
    SDL_Event e;

    while( SDL_PollEvent(&e) != 0)
    {
        Buttons(e);
        switch(e.type){
            case SDL_QUIT:
                quit = true;
            break;
            case SDL_KEYDOWN:
                if(e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
            break;
        }
    }
}

void Highscores(){
    SDL_Event e;

    while( SDL_PollEvent(&e) != 0)
    {
        Buttons(e);
        switch(e.type){
            case SDL_QUIT:
                quit = true;
            break;
            case SDL_KEYDOWN:
                if(e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
            break;
        }
    }
}

void Credits(){
    SDL_Event e;

    while( SDL_PollEvent(&e) != 0)
    {
        Buttons(e);
        switch(e.type){
            case SDL_QUIT:
                quit = true;
            break;
            case SDL_KEYDOWN:
                if(e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;
            break;
        }
    }
}

void Buttons(SDL_Event e){
    int Mx, My;
    int ballcolor;

    SDL_GetMouseState( &Mx, &My );
    if(interface == 1){
        /*Sound Element Button*/
        SoundElement.posX = 3;
        SoundElement.posY = (SCREEN_HEIGHT - 41) + 4;
        if(Mx < (41) && Mx > (3)
        && My > (SCREEN_HEIGHT- 41) && My < (SCREEN_HEIGHT - 41 + 38)){
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
            if(Sound) Sound = false;
            else Sound = true;
            }
            if (Sound == true)
                SoundElement.image = loadSurface("./Images/soundOnHoverBlue.png");
            else if (Sound == false)
                SoundElement.image = loadSurface("./Images/soundOffHoverBlue.png");
            }
        else{
            if (Sound == true)
                SoundElement.image = loadSurface("./Images/soundOnBlue.png");
            else if (Sound == false)
                SoundElement.image = loadSurface("./Images/soundOffBlue.png");
            }

        /*Main Menu Buttons*/
        if(!(Mx > 110 && Mx < 210 && My > 210 && My < 359)){
            ArrowElement.image = NULL;
        }
        else if(Mx > 110 && Mx < 210){
            ArrowElement.image = loadSurface("./Images/arrow.png");
            if(My > 210 && My < 235){
                ArrowElement.posY = 208;
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                interface = 2;
                makeBACKGROUND();
                }
            }
            if(My > 250 && My < 275){
                ArrowElement.posY = 250;
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                    interface = 3;
                    makeBACKGROUND();
                }
            }
            if(My > 290 && My < 317){
                ArrowElement.posY = 292;
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                    interface = 4;
                    makeBACKGROUND();
                }
            }
            if(My > 332 && My < 359){
                ArrowElement.posY = 334;
                if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                    quit = true;
                }
            }
        }


    }

    if(interface == 2 || interface == 3 || interface == 4){
        /*End Game Button*/
        if((Mx < (SCREEN_WIDTH - 2*IMAGE_WIDTH + 38) && Mx > (SCREEN_WIDTH - 2*IMAGE_WIDTH -2)
        && My > (SCREEN_HEIGHT- 41) && My < (SCREEN_HEIGHT - 41 + 38))
        || (Mx < (268) && Mx > (238) && My < (274) && My > (238) )){
            if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
              interface = 1;
              play = 0;
              ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
              ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT) - 4;
              ball.stepX = 0;
              ball.stepY = 0;
              ballcolor = rand() % COLORS + 1;
              ball.color = ballcolor;
              ball.image = GetColor(ballcolor);
              ballcolor = rand() % COLORS + 1;
              nextball.image = GetColor(ballcolor);
              nextball.color = ballcolor;
              ArrowElement.image = NULL;
              ThreatLevel = 1;

              cleanGrid();
              makeBACKGROUND();
              EGelement.image = loadSurface("./Images/menuBlue.png");
            }
            if(!(Mx < (268) && Mx > (238) && My < (274) && My > (238) )){
                switch(ThreatLevel){
                    case 1: EGelement.image = loadSurface("./Images/menuHoverBlue.png"); break;
                    case 2: EGelement.image = loadSurface("./Images/menuHoverYellow.png"); break;
                    case 3: EGelement.image = loadSurface("./Images/menuHoverRed.png"); break;
                }
            }
        }
            else{
                switch(ThreatLevel){
                    case 1: EGelement.image = loadSurface("./Images/menuBlue.png"); break;
                    case 2: EGelement.image = loadSurface("./Images/menuYellow.png"); break;
                    case 3: EGelement.image = loadSurface("./Images/menuRed.png"); break;
                }
            }

        /*Sound Element Button*/
        SoundElement.posX = 32;
        SoundElement.posY = (SCREEN_HEIGHT - 41) + 6;
          if(Mx < (66) && Mx > (28)
          && My > (SCREEN_HEIGHT- 41) && My < (SCREEN_HEIGHT - 41 + 38)){
              if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
              if(Sound) Sound = false;
              else Sound = true;
              }
              if (Sound == true){
                  switch(ThreatLevel){
                      case 1: SoundElement.image = loadSurface("./Images/soundOnHoverBlue.png"); break;
                      case 2: SoundElement.image = loadSurface("./Images/soundOnHoverYellow.png"); break;
                      case 3: SoundElement.image = loadSurface("./Images/soundOnHoverRed.png"); break;
                  }
              }
              else if (Sound == false){
                  switch(ThreatLevel){
                      case 1: SoundElement.image = loadSurface("./Images/soundOffHoverBlue.png"); break;
                      case 2: SoundElement.image = loadSurface("./Images/soundOffHoverYellow.png"); break;
                      case 3: SoundElement.image = loadSurface("./Images/soundOffHoverRed.png"); break;
                  }
              }
          }
          else{
              if (Sound == true){
                  switch(ThreatLevel){
                      case 1: SoundElement.image = loadSurface("./Images/soundOnBlue.png"); break;
                      case 2: SoundElement.image = loadSurface("./Images/soundOnYellow.png"); break;
                      case 3: SoundElement.image = loadSurface("./Images/soundOnRed.png"); break;
                  }
              }
              else if (Sound == false){
                  switch(ThreatLevel){
                      case 1: SoundElement.image = loadSurface("./Images/soundOffBlue.png"); break;
                      case 2: SoundElement.image = loadSurface("./Images/soundOffYellow.png"); break;
                      case 3: SoundElement.image = loadSurface("./Images/soundOffRed.png"); break;
                  }
              }
          }

          /*EGMen Button*/
          if(Mx < (268) && Mx > (238)
          && My < (274) && My > (238)){
              if(ThreatLevel == 3) EGMen.image = loadSurface("./Images/menuHoverRed.png");
              if(ThreatLevel == 1) EGMen.image = loadSurface("./Images/menuHoverBlue.png");
          }
          else{
              if(ThreatLevel == 3) EGMen.image = loadSurface("./Images/menuRed.png");
              if(ThreatLevel == 1) EGMen.image = loadSurface("./Images/menuBlue.png");
          }

          /*Replay Button*/
          if(Mx < (335) && Mx > (305)
          && My < (306) && My > (272)){
              if(ThreatLevel == 3) EGR.image = loadSurface("./Images/returnHoverRed.png");
              if(ThreatLevel == 1) EGR.image = loadSurface("./Images/returnHoverBlue.png");
              if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                EGUI.image = NULL;
                EGR.image = NULL;
                EGRank.image = NULL;
                EGMen.image = NULL;
                play = 0;
                /*PreparePlay();*/
            }
          }
          else{
              if(ThreatLevel == 3) EGR.image = loadSurface("./Images/returnRed.png");
              if(ThreatLevel == 1) EGR.image = loadSurface("./Images/returnBlue.png");
          }

          /*Ranking Button
          372, 30
          238, 34*/
          if(Mx < (402) && Mx > (372)
          && My < (274) && My > (238)){
              if(ThreatLevel == 3) EGRank.image = loadSurface("./Images/rankHoverRed.png");
              if(ThreatLevel == 1) EGRank.image = loadSurface("./Images/rankHoverBlue.png");
              if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT){
                EGUI.image = NULL;
                EGR.image = NULL;
                EGRank.image = NULL;
                EGMen.image = NULL;
                interface = 3;
                ThreatLevel = 1;
                printf("%d\n", ThreatLevel);
                play = 0;
              }
          }
          else{
              if(ThreatLevel == 3) EGRank.image = loadSurface("./Images/rankRed.png");
              if(ThreatLevel == 1) EGRank.image = loadSurface("./Images/rankBlue.png");
          }
    }
}

void Play(){
    SDL_Event e;
    interface = 2;

    if(!play){
        PreparePlay();
    }

    while( SDL_PollEvent( &e ) != 0 )
    {
        Buttons(e);
        if( e.type == SDL_QUIT )
        {
            quit = true;
        }
        else if( e.type == SDL_KEYDOWN )
        {
            switch( e.key.keysym.sym )
            {
                case SDLK_UP: /* printf("UP\n"); */
                break;
                case SDLK_DOWN: if(play == 1) gridDown();
                break;
                case SDLK_ESCAPE: quit = true;
                break;
            }
        }
        else if ((e.type == SDL_MOUSEBUTTONDOWN) && e.button.button == SDL_BUTTON_LEFT && !clicked) shoot();
    }

    if(clicked)
    {
		movePLAYER();
		collision();
        /*checkAround(n);*/
	}

    if(maxhealth == 0){
      maxhealth = 6;
    }
      if (health<0){
          gridDown();
          maxhealth --;
          health = maxhealth;
          GetLifeSurface();
      }

      if(PlayEnd() == 1){
            printf("\n\n\n\n\n\n\n\n\n\n\t\t\t\t   FRACASSADO\n\n\n\n\n\n\n\n\n\n\n\n\n");
            /*interface = 1;*/
            /*ThreatLevel = 1;*/
            /*makeBACKGROUND();*/
      }
}

int init() {
    /*Initialization flag*/
    int success = true;

    srand(time(NULL));

    /*Initialize SDL*/
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        /*Create window*/ /*##Trocar o nome*/
        gWindow = SDL_CreateWindow( "Omega Shooter", SDL_WINDOWPOS_UNDEFINED,
									SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }

        else
        {
            /*Initialize JPG and PNG loading */
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) )
            {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }

            else
            {
                /*Get window surface*/
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }

            /*Initialize TTF*/
            if (TTF_Init() == -1){
        		printf("SDL could not initialize TTF! SDL Error: %s\n", SDL_GetError());
                success = false;
	        }

            /*Initialize SDL_mixer */
            if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
            	success = false;

            /* Load our sound effect */
            wave = Mix_LoadWAV(WAV_PATH);
            if (wave == NULL)
            	success = false;

            /* Load our music */
            music = Mix_LoadMUS(MUS_PATH);
            if (music == NULL)
            	success = false;

            /*surfaceMessage = TTF_RenderText_Solid(font, "000000000000", ttfColor);*/

        }
    }



    return success;
}

int loadMedia()
{
    /*Loading success flag*/
    int success = true;
    /*uint32_t colorKey;*/

    if( BallSurface == NULL)
    {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }

    else
    {
        Uint32 colorkey = SDL_MapRGBA( BallSurface->format, 0x00, 0x00, 0x00, 0xFF );
        SDL_SetColorKey( BallSurface,1, colorkey );
    }
    return success;
}

void closing()
{
    /*int i, j;*/
    printf("entered closing\n");
    SDL_FreeSurface(gScreenSurface);
    gScreenSurface = NULL;

    printf("freed Screen surface\n");

    /*Free loaded image*/
    if(BallSurface != NULL)
        SDL_FreeSurface( BallSurface );
    printf("freed ballsurface\n");

    /*for (i=0; i<BALLY; i++)
        for(j=0; j<BALLX; j++)
            if(ballgrid[i][j].image != NULL)*/
                /*SDL_FreeSurface(ballgrid[i][j].image);*/
    /*printf("freed all npc surfaces\n");*/

    if(ball.image != NULL)
        /*SDL_FreeSurface(ball.image);*/
    printf("freed ball.npc surface\n");

    if(nextball.image != NULL)
        SDL_FreeSurface(nextball.image);
    printf("freed nextball surface\n");

    /*for (i=0; i<6; i++)
        if(lifeballs[i].image != NULL)
            SDL_FreeSurface(lifeballs[i].image);
    printf("freed lifeballs surfaces\n");*/

    if (SoundElement.image != NULL)
        SDL_FreeSurface(SoundElement.image);
    printf("freed soundelement surface\n");

    if (EGelement.image != NULL)
        SDL_FreeSurface(EGelement.image);
    printf("freed EGelement surface\n");

    if (EGelement.image != NULL)
        SDL_FreeSurface(ArrowElement.image);
    printf("freed arrow element surface\n");

    if (EGelement.image != NULL)
        SDL_FreeSurface(backg.image);
    printf("freed bakcground image surface\n");

    /*Destroy window*/
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    printf("freed gWindow\n");

    /* clean up our resources */
	/*Mix_FreeChunk(wave);*/
    /*if(music != NULL)
	   Mix_FreeMusic(music);
    printf("freed music\n");*/

	/* quit SDL_mixer */
	/*Mix_CloseAudio();
    printf("closed audio\n");*/

    /*Quit SDL subsystems*/
    IMG_Quit();
    printf("quitted img\n");
    TTF_Quit();
    printf("quitted ttf\n");
    Mix_Quit();
    printf("quitted mix\n");
    SDL_Quit();
    printf("quitted SDL\n");
}

SDL_Surface* loadSurface( char *path )
{
    /*The final optimized image*/
    SDL_Surface* optimizedSurface = NULL;

    /*Load image at specified path*/
    SDL_Surface* loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
    }

    else
    {
        /*Convert surface to screen format*/
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL )
        {
            printf( "Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError() );
        }

        else
        {
            Uint32 colorkey = SDL_MapRGBA( optimizedSurface->format, 0x00, 0x00, 0x00, 0xFF);
            SDL_SetColorKey( optimizedSurface,1, colorkey );
        }

        /*Get rid of old loaded surface*/
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}

int PrepareGame()
{
  int ballcolor;
  int i;

  /*Start up SDL and create window*/
  if( !init() )
  {
      printf( "Failed to initialize!\n" );
      return 1;
  }


  ballcolor = rand() % COLORS + 1;
  BallSurface = GetColor(ballcolor);

  /*Load media*/
  if( !loadMedia() )
  {
      printf( "Failed to load media!\n" );
      return 2;
  }


  interface = 1;
  /*####*/
  Sound = true;

  /*Create Background*/
  makeBACKGROUND();

  /*Create PLAYER*/

  ball = createPLAYER((SCREEN_WIDTH/2 - IMAGE_WIDTH/2),
                   (SCREEN_HEIGHT - IMAGE_HEIGHT) - 4,
                    0,
                    0,
                    ballcolor,
                    GetColor(ballcolor));

  /*Create nextball*/
  ballcolor = rand() % COLORS + 1;
  BallSurface = GetColor(ballcolor);

  nextball = createELEMENT(
              193,
              (SCREEN_HEIGHT - IMAGE_HEIGHT) + 1,
              ballcolor,
              GetColor(ballcolor));

  for(i=0; i<5; i++){
    lifeballs[i] = createELEMENT(
                    (SCREEN_WIDTH/3) - 24 + (i*7) + 56,
                    (i%2)?(SCREEN_HEIGHT - IMAGE_HEIGHT) + 13:(SCREEN_HEIGHT - IMAGE_HEIGHT) +1,
                    0,
                    loadSurface("./Images/LifeBlue.png"));
  }

  SoundElement = createELEMENT(
                28 + 4,
                (SCREEN_HEIGHT - 41) + 6,
                0,
                NULL);

  EGelement = createELEMENT(
                  SCREEN_WIDTH - 2*IMAGE_WIDTH -2 + 4,
                  (SCREEN_HEIGHT - 41) + 6,
                  0,
                  NULL);

  ArrowElement = createELEMENT(
                    79,
                    -38,
                    0,
                    NULL);

  PMUI = createELEMENT(
                    0,
                    0,
                    0,
                    NULL);

  EGUI = createELEMENT(
                    SCREEN_WIDTH/2 - 100,
                    SCREEN_HEIGHT/2 - 125,
                    0,
                    NULL);

 EGMen = createELEMENT(
                    238,
                    238,
                    0,
                    NULL);

 EGR = createELEMENT(
                    305,
                    272,
                    0,
                    NULL);

 EGRank = createELEMENT(
                    372,
                    238,
                    0,
                    NULL);

 CredElement = createELEMENT(
                    SCREEN_WIDTH/2-150,
                    SCREEN_HEIGHT/2-100,
                    0,
                    loadSurface("./Images/credits.png"));

 RankElement = createELEMENT(
                    SCREEN_WIDTH/2-220,
                    SCREEN_HEIGHT/2-220,
                    0,
                    loadSurface("./Images/TopPlayers.png"));

  return 0;
}

void PreparePlay(){
    /*Create Ball Grid*/
    maxhealth = 5;
    ThreatLevel = 1;
    health = maxhealth;
    GetLifeSurface();
    Score = 0;
    clicked = 0;
    createGrid(GRIDY);
    ball.image = GetColor(ball.color);
    nextball.image = GetColor(nextball.color);
    play = 1;
}

int PlayWin(){
    int i, j;
    for(i=1, j=0; j<BALLX; j++){
        if(ballgrid[i][j].color){
            return 0;
        }
    }
    ball.image = NULL;
    nextball.image = NULL;
    clicked = 1;
    play = -1;
    Score<<=1;
    cleanGrid();
    EndGameUI();
    return 1;
}

int PlayEnd(){
    int i, j, stop = false;
    for(i=16, j=0; j<BALLX; j++){
        if(ballgrid[i][j].color){
            stop = true;
            break;
        }
    }
    if (!stop) return 0;
    for (i=1; i<BALLY-1; i++){
        for(j=1; j<GRIDX; j++){
            ballgrid[i][j].color = 0;
        }
        SDL_Delay(DELAY*5);
        RefreshScreen();
    }
    ball.image = NULL;
    nextball.image = NULL;
    clicked = 1;
    play = -1;
    cleanGrid();
    EndGameUI();
    return 1;
}

void printGrid(){
    int i, j;
    for(i=0; i<BALLY; i++){
        for(j=0; j<BALLX; j++){
            if(i%2==0) printf("%d ", ballgrid[i][j].color);
            else printf(" %d", ballgrid[i][j].color);
        }
        printf("\n");
    }
    printf("---\n");
}

void gridDown()
{
    int i, j, ballcolor;

    for (i = BALLY-1; i > 0; i--)
	{
		for (j=1; j < GRIDX; j++)
		{
            if (ballgrid[i][j].color){
                /*SDL_FreeSurface(ballgrid[i][j].image);*/
    			ballgrid[i+1][j] = createNPC(
    				(i+1)*(IMAGE_HEIGHT - 5),
    				j*IMAGE_WIDTH + ((i+1)%2 * IMAGE_WIDTH/2) - IMAGE_WIDTH/4,
    				i+1,
    				j,
                    ballgrid[i][j].color,
    				GetColor(ballgrid[i][j].color)
                );
                ballgrid[i][j].color = 0;
                /* printf("Ball Created\n"); */
    			drawNPC(ballgrid[i+1][j]);
            }
		}
	}
    /* ### */
    for (j=1; j <= GRIDX; j++)
    {
        ballcolor = rand() % COLORS + 1;
        BallSurface = GetColor(ballcolor);
        ballgrid[1][j] = createNPC(
            1*(IMAGE_HEIGHT - 5),
            j*IMAGE_WIDTH + (1%2 * IMAGE_WIDTH/2) - IMAGE_WIDTH/4,
            1,
            j,
            ballcolor,
            BallSurface
        );
        drawNPC(ballgrid[1][j]);
    }
    ballgrid[1][GRIDX].color = 0;
    for(j = 1; j<GRIDX; j++){
        if (ballgrid[1][j].color){
            checkIsland(&ballgrid[1][j]);
        }

    }
    /*printGrid();*/
    DestroyIsland(false);
    GetThreatLevel();
    GetLifeSurface();
}

void checkIsland(NPC* npc)
{
    int n;
    npc->remain = 1;
    /* printf("AA\n"); */

    /*
      COLTYPE CODES:
           6 1
          5 O 2
           4 3
    */

	if (npc->indexX < 0 && npc->indexX > BALLX) return;

    for(n = 0; n <= 1; n++){
        if((npc->indexY)%2 == n){
            /*case 3*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n].color && ballgrid[(npc->indexY)+1][(npc->indexX)+n].remain == 0){
                checkIsland (&ballgrid[(npc->indexY)+1][(npc->indexX)+n]);
            }
            /*case 1*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n].color && ballgrid[(npc->indexY)-1][(npc->indexX)+n].remain == 0){
                checkIsland (&ballgrid[(npc->indexY)-1][(npc->indexX)+n]);
            }
            /*case 6*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color && ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].remain == 0){
                checkIsland (&ballgrid[(npc->indexY)-1][(npc->indexX)+n-1]);
            }
            /*case 4*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color && ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].remain == 0){
                checkIsland (&ballgrid[(npc->indexY)+1][(npc->indexX)+n-1]);
            }
        }
    }
    /*case 2*/
    if(ballgrid[(npc->indexY)][(npc->indexX)-1].color && ballgrid[(npc->indexY)][(npc->indexX)-1].remain == 0){
        checkIsland (&ballgrid[(npc->indexY)][(npc->indexX)-1]);
    }
    /*case 5*/
    if(ballgrid[(npc->indexY)][(npc->indexX)+1].color && ballgrid[(npc->indexY)][(npc->indexX)+1].remain == 0){
        checkIsland (&ballgrid[(npc->indexY)][(npc->indexX)+1]);
    }

	return ;
}


void checkAround(NPC* npc, int checkcolor)
{
    int n;

    SDL_Delay(5*DELAY);
    RefreshScreen();

    /*
      COLTYPE CODES:
           6 1
          5 O 2
           4 3
    */

	if (npc->indexX < 0 && npc->indexX > BALLX){
    return;
  }

    for(n = 0; n <= 1; n++){
        if((npc->indexY)%2 == n){
            /*case 3*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n].color == checkcolor){
                npc->color = 0;
                Score += 20;
                ballgrid[(npc->indexY)+1][(npc->indexX)+n].color = 0;
                /*SDL_FreeSurface(ballgrid[(npc->indexY)+1][(npc->indexX)+n].image);*/
                checkAround (&ballgrid[(npc->indexY)+1][(npc->indexX)+n], checkcolor);
            }
            /*case 1*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n].color == checkcolor){
                npc->color = 0;
                Score += 20;
                ballgrid[(npc->indexY)-1][(npc->indexX)+n].color = 0;
                /*SDL_FreeSurface(ballgrid[(npc->indexY)-1][(npc->indexX)+n].image);*/
                checkAround (&ballgrid[(npc->indexY)-1][(npc->indexX)+n], checkcolor);
            }
            /*case 6*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color == checkcolor){
                npc->color = 0;
                Score += 20;
                ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color = 0;
                /*SDL_FreeSurface(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].image);*/
                checkAround (&ballgrid[(npc->indexY)-1][(npc->indexX)+n-1], checkcolor);
            }
            /*case 4*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color == checkcolor){
                npc->color = 0;
                Score += 20;
                ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color = 0;
                /*SDL_FreeSurface(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].image);*/
                checkAround (&ballgrid[(npc->indexY)+1][(npc->indexX)+n-1], checkcolor);
            }
        }
    }
    /*case 2*/
    if(ballgrid[(npc->indexY)][(npc->indexX)-1].color == checkcolor){
        npc->color = 0;
        Score += 20;
        ballgrid[(npc->indexY)][(npc->indexX)-1].color = 0;
        /*SDL_FreeSurface(ballgrid[(npc->indexY)][(npc->indexX)-1].image);*/
        checkAround (&ballgrid[(npc->indexY)][(npc->indexX)-1], checkcolor);
    }
    /*case 5*/
    if(ballgrid[(npc->indexY)][(npc->indexX)+1].color == checkcolor){
        npc->color = 0;
        Score += 20;
        ballgrid[(npc->indexY)][(npc->indexX)+1].color = 0;
        /*SDL_FreeSurface(ballgrid[(npc->indexY)][(npc->indexX)+1].image);*/
        checkAround (&ballgrid[(npc->indexY)][(npc->indexX)+1], checkcolor);
    }

	return ;
}

void checkDestruction(NPC* npc, int checkcolor)
{
    int n;
    int j;

    printf("Entrou no check!\n");

    currentCount++;
    printf("currentCount = %d\n", currentCount);
    printf("ballCount = %d\n", ballCount);

    if (currentCount == 1 && ballCount == 1) return;

    if (ballCount == 0)
    {
        destructionStart = npc;
    }
    else if (currentCount >= 3)
    {
        currentCount = 0;
        return;
    }
    if (ballCount > 2)
    {
        /* printf("ballCount = %d\n", ballCount); */
        ballCount = 0;
        Score -= 10;
        checkAround(destructionStart,destructionStart->color);
        for(j = 1; j<GRIDX; j++){
            if (ballgrid[1][j].color){
                checkIsland(&ballgrid[1][j]);
            }

        }
        /*printGrid();*/
        DestroyIsland(true);
        destructionStart = NULL;
        currentCount = 0;
        health++;
        printf("Destruiu!\n");
        if (PlayWin()){
            printf("\nnão fez mais que a sua obrigação\n");
        }
        return;
    }
    /*
      COLTYPE CODES:
           6 1
          5 O 2
           4 3
    */

	if (npc->indexX < 0 && npc->indexX > BALLX) return;

    for(n = 0; n <= 1; n++){
        if((npc->indexY)%2 == n){
            /*case 3*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n].color == checkcolor){
                ballCount++;
                checkDestruction (&ballgrid[(npc->indexY)+1][(npc->indexX)+n], checkcolor);
            }
            /*case 1*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n].color == checkcolor){
                ballCount++;
                checkDestruction (&ballgrid[(npc->indexY)-1][(npc->indexX)+n], checkcolor);
            }
            /*case 6*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color == checkcolor){
                ballCount++;
                checkDestruction (&ballgrid[(npc->indexY)-1][(npc->indexX)+n-1], checkcolor);
            }
            /*case 4*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color == checkcolor){
                ballCount++;
                checkDestruction (&ballgrid[(npc->indexY)+1][(npc->indexX)+n-1], checkcolor);
            }
        }
    }
    /*case 2*/
    if(ballgrid[(npc->indexY)][(npc->indexX)-1].color == checkcolor){
        ballCount++;
        checkDestruction (&ballgrid[(npc->indexY)][(npc->indexX)-1], checkcolor);
    }
    /*case 5*/
    if(ballgrid[(npc->indexY)][(npc->indexX)+1].color == checkcolor){
        ballCount++;
        checkDestruction (&ballgrid[(npc->indexY)][(npc->indexX)+1], checkcolor);
    }


	return ;
}

void DestroyIsland(int ScoreOn){
    int i, j;
    for (i=1; i<BALLY-1; i++)
        for(j=1; j<GRIDX; j++){
            if (ballgrid[i][j].remain){
                ballgrid[i][j].remain = 0;
            }
            else{
                if (ballgrid[i][j].color){
                    SDL_Delay(5*DELAY);
                    RefreshScreen();
                    if(ScoreOn) Score += 100;
                }
                ballgrid[i][j].indexX = 0;
                ballgrid[i][j].indexY = 0;
                ballgrid[i][j].posX = 0;
                ballgrid[i][j].posY = 0;
                ballgrid[i][j].centerX = 0;
                ballgrid[i][j].centerY = 0;
                ballgrid[i][j].color = 0;
                /*SDL_FreeSurface(ballgrid[i][j].image);*/
            }
        }
    /*printGrid();*/
}

void GetThreatLevel(){
    int i, j, stop = false;
    for (i=BALLY-1; (i>0 && stop == false); i--)
        for(j=1; j<GRIDX; j++){
                if(ballgrid[i][j].color && i == 13){ ThreatLevel = 3; stop = true; break; }
                else if(ballgrid[i][j].color && i == 10){ ThreatLevel = 2; stop = true; break; }
                else if(ballgrid[i][j].color && i < 10){ ThreatLevel = 1; stop = true; break; }
            }
    switch(ThreatLevel){
        case 1:
            PMUI.image = loadSurface( "./Images/uiBlue.png");
            EGelement.image = loadSurface( "./Images/menuBlue.png");
            if(Sound) SoundElement.image = loadSurface("./Images/soundOnBlue.png");
            else SoundElement.image = loadSurface("./Images/soundOffBlue.png");

        break;
        case 2:
            PMUI.image = loadSurface( "./Images/uiYellow.png");
            EGelement.image = loadSurface( "./Images/menuYellow.png");
            if(Sound) SoundElement.image = loadSurface("./Images/soundOnYellow.png");
            else SoundElement.image = loadSurface("./Images/soundOffYellow.png");
        break;
        case 3:
            PMUI.image = loadSurface( "./Images/uiRed.png");
            EGelement.image = loadSurface( "./Images/menuRed.png");
            if(Sound) SoundElement.image = loadSurface("./Images/soundOnRed.png");
            else SoundElement.image = loadSurface("./Images/soundOffRed.png");
        break;
    }
}

void GetLifeSurface(){
    int i;
    switch(ThreatLevel){
        case 1:
            for(i=0; i<health; i++)
                lifeballs[i].image = loadSurface( "./Images/LifeBlue.png" );
        break;
        case 2:
            for(i=0; i<health; i++)
                lifeballs[i].image = loadSurface( "./Images/LifeYellow.png" );
        break;
        case 3:
            for(i=0; i<health; i++)
                lifeballs[i].image = loadSurface( "./Images/LifeRed.png" );
        break;
    }
}

void GetScore(){
    /*The TTF_Font*/
    TTF_Font* font = NULL;
    TTF_Font* bigfont = NULL;

    /*The TTF Color*/
    SDL_Color ttfColor;

    /*The Message Surface*/
    SDL_Surface* surfaceMessage = NULL;
    SDL_Surface* bigSurface = NULL;

    /*The Message Rect*/
    SDL_Rect Message_Rect;

    char scoreString[16];

    /* Load TTF font */
    font = TTF_OpenFont(TTF_PATH, 10);
    if(font == NULL)
        exit(748);

    switch(ThreatLevel){
        case 1: ttfColor.r = 145;
                ttfColor.g = 223;
                ttfColor.b = 224;
        break;
        case 2:
                ttfColor.r = 213;
                ttfColor.g = 216;
                ttfColor.b = 118;
        break;
        case 3:
                ttfColor.r = 224;
                ttfColor.g = 68;
                ttfColor.b = 68;
        break;
    }

	Message_Rect.x = SCREEN_WIDTH/2 + 69;
	Message_Rect.y = SCREEN_HEIGHT-20;
	Message_Rect.w = 100;
	Message_Rect.h = 38;

    sprintf(scoreString, "%012d", Score);
    /*printf("String Score = %s\n", scoreString);*/
    surfaceMessage = TTF_RenderText_Solid(font,scoreString, ttfColor);
    if(!surfaceMessage){
        printf("Failed to render Text!\n");
        /*exit(734);*/
    }

    SDL_BlitSurface( surfaceMessage, NULL, gScreenSurface, &Message_Rect );
    SDL_FreeSurface(surfaceMessage);

    if (play == -1){
        bigfont = TTF_OpenFont(TTF_PATH, 20);
        Message_Rect.x = SCREEN_WIDTH/2 - 115;
        Message_Rect.y = SCREEN_HEIGHT/2 + 90;
        Message_Rect.w = 200;
        Message_Rect.h = 76;
        sprintf(scoreString, "Score: %08d", Score);
        bigSurface = TTF_RenderText_Solid(bigfont,scoreString, ttfColor);
        SDL_BlitSurface(bigSurface, NULL, gScreenSurface, &Message_Rect);
        TTF_CloseFont( bigfont );
    }

    TTF_CloseFont( font );
    /*surfaceMessage = NULL;*/
}

void EndGameUI(){
    int Mx, My;
    switch(ThreatLevel){
        case 1:
            EGUI.image = loadSurface("./Images/victory.png");
            EGR.image = loadSurface("./Images/returnBlue.png");
            EGMen.image = loadSurface("./Images/menuBlue.png");
            EGRank.image = loadSurface("./Images/rankBlue.png");
        break;
        case 3:
            EGUI.image = loadSurface("./Images/defeat.png");
            EGR.image = loadSurface("./Images/returnRed.png");
            EGMen.image = loadSurface("./Images/menuRed.png");
            EGRank.image = loadSurface("./Images/rankRed.png");
        break;
    }
    SDL_GetMouseState( &Mx, &My );



}

void GetInput(SDL_Event e){
    /*The TTF_Font*/
    TTF_Font* font = NULL;

    /*The TTF Color*/
    SDL_Color ttfColor;

    /*The Message Surface*/
    SDL_Surface* NickSurface = NULL;

    /*The Message Rect*/
    SDL_Rect Message_Rect;

    printf("%s\n", NickString);

    /* Load TTF font */
    font = TTF_OpenFont(TTF_PATH, 20);
    if(font == NULL)
        exit(748);

    switch(ThreatLevel){
        case 1: ttfColor.r = 145;
                ttfColor.g = 223;
                ttfColor.b = 224;
        break;
        case 2:
                ttfColor.r = 213;
                ttfColor.g = 216;
                ttfColor.b = 118;
        break;
        case 3:
                ttfColor.r = 224;
                ttfColor.g = 68;
                ttfColor.b = 68;
        break;
    }

	Message_Rect.x = SCREEN_WIDTH/2;
	Message_Rect.y = SCREEN_HEIGHT/2;
	Message_Rect.w = 100;
	Message_Rect.h = 76;

            switch(e.type){
                case SDL_TEXTINPUT:
                    if(strlen(NickString)<16){
                        strcat(NickString, e.text.text);
                    }
                break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_BACKSPACE:
                            NickString[strlen(NickString)-1] = '\0';
                        break;
                        case SDLK_RETURN:
                            getstring = false;
                        break;
                    }
                break;
            }


        /*NickString[i+1] = '\0';*/
        NickSurface = TTF_RenderText_Solid(font, NickString, ttfColor);
        SDL_BlitSurface( NickSurface, NULL, gScreenSurface, &Message_Rect );
        SDL_FreeSurface(NickSurface);

        TTF_CloseFont( font );
}

void DrawStar(){
    SDL_Surface* StarSurface = NULL;
    SDL_Rect Star_Rect;

    static int spriteimg = 0;

    if(spriteimg == 0){
        Star_Rect.x = SCREEN_WIDTH/2;
        Star_Rect.y = SCREEN_HEIGHT/2;
        Star_Rect.w = 4;
        Star_Rect.h = 4;
    }

        if (spriteimg<50) StarSurface = loadSurface("./Images/star1_0.png");
        if (spriteimg < 100 && spriteimg >= 0) StarSurface = loadSurface("./Images/star1_1.png");
        if (spriteimg < 150 && spriteimg >= 100) StarSurface = loadSurface("./Images/star1_2.png");

    SDL_BlitSurface( StarSurface, NULL, gScreenSurface, &Star_Rect );
    SDL_FreeSurface(StarSurface);

    spriteimg++;
    if (spriteimg > 150) spriteimg = 0;
}
