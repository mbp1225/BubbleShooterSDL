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
 * Nome: Rafael da Silva Fernandes
 * DRE: 117196229
 *
 * Grupo: Eduardo Melo, Matheus Pinheiro, Rafael Fernandes
 *
 */

/*Using SDL, SDL_image, standard IO, and strings*/
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*
 * Constants
 */

/*Screen dimension constants*/
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
/*Border Width*/
const int BORDER = 24;

/*Ball moving speed*/
const int MSPEED = 14;

/*Ball collision radius*/
const int COLRADIUS = 8;

/*Amount of different colors for the balls*/
const int COLORS = 6;

/*Quantidade de bolhas total*/
const int BALLX = 20;
const int BALLY = 20;

/*Quantidade de bolhas na grid inicial*/
const int GRIDX = 19;
const int GRIDY = 5;

const int false = 0;
const int true = 1;

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

/*
 * Global Variables
 */

int clicked = 0;
int quit = 0;
int health;

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/*The image character*/
PLAYER ball;

/*Ball Grid [Y][X]*/
NPC ballgrid[20][20];

int ballCount = 0;

/*Ball surface*/
SDL_Surface* BallSurface;

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

/*Background Play UI surface*/
SDL_Surface* PUI;

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

/*Frees media and shuts down SDL*/
void closing();

/*Loads individual image*/
SDL_Surface* loadSurface( char *path );

/*Create PLAYER*/
PLAYER createPLAYER( float posX, float posY, float stepX, float stepY, int color, SDL_Surface *image);

/*Create NPC*/
NPC createNPC(float posY, float posX, int indexY, int indexX, int color, SDL_Surface *image);

/*Prepares grid*/
void prepareGrid();

/*Create grid*/
void createGrid(int ballY, int ballX);

/*Move PLAYER*/
void movePLAYER();

/*Gets ball color*/
SDL_Surface* GetColor(int color);

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
void DestroyIsland();

/*Prepares game initialization and variables*/
int PrepareGame();

/*Prints the screen surface and its updates*/
void RefreshScreen(void);

/*Prints grid in the terminal*/
void printGrid();

/*Moves the grid down when called*/
void gridDown();

/*Displays player on screen*/
void drawPLAYER(PLAYER p);

/*Displays NPC on screen*/
void drawNPC(NPC n);

/*Displays Player UI*/
void drawBACKGROUND(BACKGROUND b);

/*Game Function*/
void game();

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

    /*While application is running*/
    while( !quit )
    {
        game();
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
    NPC *n;

    /*Checks if there's any NPC collision for it inside the function*/
    n = NPCCollision();

    /*Checks if there's any ceiling collision for it inside the function*/
    if (n == NULL)
        n = CeilingCollision();
    if (n != NULL){
        /*printGrid();*/
        ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
        ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
        ball.stepY = 0;
        ball.stepX = 0;
        clicked = 0;
        health--;
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
      checkDestruction(&ballgrid[1][newX], ballgrid[1][newX].color);
      drawNPC(ballgrid[1][newX]);
      ballcolor = rand() % COLORS + 1;
      ball.color = ballcolor;
      ball.image = GetColor(ballcolor);

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
                    if (ball.centerY < IMAGE_HEIGHT/5 + ballgrid[i][j].posY +1) ballgrid[i][j].coltype = 1;
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
        checkDestruction(newNPC, newNPC->color);
		colNPC->coltype = 0;
		ballcolor = rand() % COLORS + 1;
		ball.color = ballcolor;
		ball.image = GetColor(ballcolor);

        return newNPC;
    }
    return NULL;
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
BACKGROUND  makeBACKGROUND()
{
  BACKGROUND b;
  b.image = loadSurface( "./Images/BG.png" );
  return b;
}

/*
    COLOR CODES
    0 = null
    1 = earth
    2 = moon
    3 = neptune
    4 = uranus
    5 = jupiter
    6 = mars
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

/*Create Grid*/
void createGrid(int ballY, int ballX)
{
	int i, j;
    int ballcolor;
    SDL_Surface* BallSurface;

    /*LEMBRAR DE TROCAR ISTO QUANDO FOR PARA MATRIZ*/
	for (i = 1; i < ballY; i++)
	{
		for (j=1; j < ballX; j++)
		{
			ballcolor = rand() % COLORS + 1;
    		BallSurface = GetColor(ballcolor);
			ballgrid[i][j] = createNPC(
				i*(IMAGE_HEIGHT - 5),
				j*IMAGE_WIDTH + (i%2 * IMAGE_WIDTH/2) - IMAGE_WIDTH/4,
				i,
				j,
                ballcolor,
				BallSurface
            );
			drawNPC(ballgrid[i][j]);
		}
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

  /*Fill the surface white*/

  SDL_FillRect( gScreenSurface, NULL,
  SDL_MapRGBA( BallSurface->format, 0x00, 0x00, 0x00, 0xFF ));

  drawBACKGROUND(backg);

  drawPLAYER(ball);

  for (i = 0; i < BALLY; i++)
    for (j = 0; j < BALLX; j++)
        drawNPC(ballgrid[i][j]);

  /*Update the surface*/
  SDL_UpdateWindowSurface( gWindow );

  /* Not so good solution, depends on your computer*/
  SDL_Delay(1);
}


void shoot(){
  int Mx, My;

  SDL_GetMouseState( &Mx, &My );
  /*Reinterpretando Mx e My com origem no centro de ball*/
  Mx = Mx - SCREEN_WIDTH/2;
  My = SCREEN_HEIGHT - My - IMAGE_HEIGHT/2;
  /*Click nao funciona se My < centro da bolinha*/

	if(My > 0)
	{
		/*Reinterpretando x e y para que a hipotenusa seja 1 (calculando o step)*/
		ball.stepX = Mx / sqrt((Mx*Mx) + (My*My));
		ball.stepY = -My / sqrt((Mx*Mx) + (My*My));

		/* CASO NECESSARIO, SABER QUAL STEPX E STEPY
		printf("step X = %f\nstep Y = %f\n", ball.stepX, ball.stepY);
		printf("%d, %d\n", Mx, My);
		*/

		/*
		 * CRIANDO UMA EXCEÇÃO PARA ANGULOS ACIMA DE 172 E ABAIXO DE 8 GRAUS
		 * sen 8 = 0.99
		 * cos 8 = -0.139
		 */

		if (ball.stepX > 0.99)
		{
			ball.stepX = 0.99;
			ball.stepY = -0.139;
		}

		if (ball.stepX < -0.99)
		{
			ball.stepX = -0.99;
			ball.stepY = -0.139;
		}

		ball.stepY*= MSPEED;
		ball.stepX*= MSPEED;
		clicked = 1;
		/* P/ TESTAR A BOLINHA IR RETO
		  ball.stepX = 0;
		  ball.stepY = -8; */
	}
}

void game(){
    /*NPC *n;*/
    /*Por enquanto, este é o play(){*/
    SDL_Event e;

    while( SDL_PollEvent( &e ) != 0 )
    {
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
                case SDLK_DOWN: gridDown();
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

    if (!health){
	    gridDown();
        health = 5;
    }

    RefreshScreen();
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
        gWindow = SDL_CreateWindow( "Bubble Shooter", SDL_WINDOWPOS_UNDEFINED,
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
    /*Free loaded image*/
    SDL_FreeSurface( BallSurface );
    BallSurface = NULL;

    /*Destroy window*/
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    /*Quit SDL subsystems*/
    IMG_Quit();
    SDL_Quit();
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

  /*Create Background*/
  backg = makeBACKGROUND();

  /*Create Ball Grid*/
  createGrid(GRIDY, GRIDX);

  /*Create PLAYER*/

  ball = createPLAYER((SCREEN_WIDTH/2 - IMAGE_WIDTH/2),
                   (SCREEN_HEIGHT - IMAGE_HEIGHT),
                    0,
                    0,
                    ballcolor,
                    BallSurface);

  return 0;
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
    SDL_Delay(50);

    for (i = BALLY-1; i > 0; i--)
	{
		for (j=1; j < GRIDX; j++)
		{
            if (ballgrid[i][j].color){
                SDL_FreeSurface(ballgrid[i][j].image);
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
    printGrid();
    DestroyIsland();
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
    SDL_Delay(25);
    RefreshScreen();

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
                npc->color = 0;
                ballgrid[(npc->indexY)+1][(npc->indexX)+n].color = 0;
                SDL_FreeSurface(ballgrid[(npc->indexY)+1][(npc->indexX)+n].image);
                checkAround (&ballgrid[(npc->indexY)+1][(npc->indexX)+n], checkcolor);
            }
            /*case 1*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n].color == checkcolor){
                npc->color = 0;
                ballgrid[(npc->indexY)-1][(npc->indexX)+n].color = 0;
                SDL_FreeSurface(ballgrid[(npc->indexY)-1][(npc->indexX)+n].image);
                checkAround (&ballgrid[(npc->indexY)-1][(npc->indexX)+n], checkcolor);
            }
            /*case 6*/
            if(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color == checkcolor){
                npc->color = 0;
                ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].color = 0;
                SDL_FreeSurface(ballgrid[(npc->indexY)-1][(npc->indexX)+n-1].image);
                checkAround (&ballgrid[(npc->indexY)-1][(npc->indexX)+n-1], checkcolor);
            }
            /*case 4*/
            if(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color == checkcolor){
                npc->color = 0;
                ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].color = 0;
                SDL_FreeSurface(ballgrid[(npc->indexY)+1][(npc->indexX)+n-1].image);
                checkAround (&ballgrid[(npc->indexY)+1][(npc->indexX)+n-1], checkcolor);
            }
        }
    }
    /*case 2*/
    if(ballgrid[(npc->indexY)][(npc->indexX)-1].color == checkcolor){
        npc->color = 0;
        ballgrid[(npc->indexY)][(npc->indexX)-1].color = 0;
        SDL_FreeSurface(ballgrid[(npc->indexY)][(npc->indexX)-1].image);
        checkAround (&ballgrid[(npc->indexY)][(npc->indexX)-1], checkcolor);
    }
    /*case 5*/
    if(ballgrid[(npc->indexY)][(npc->indexX)+1].color == checkcolor){
        npc->color = 0;
        ballgrid[(npc->indexY)][(npc->indexX)+1].color = 0;
        SDL_FreeSurface(ballgrid[(npc->indexY)][(npc->indexX)+1].image);
        checkAround (&ballgrid[(npc->indexY)][(npc->indexX)+1], checkcolor);
    }

	return ;
}

void checkDestruction(NPC* npc, int checkcolor)
{
    int n;
    int j;
    static int currentCount = 0;
    currentCount++;
    /* printf("currentCount = %d\n", currentCount); */

    if (ballCount == 0)
    {
        destructionStart = npc;
    }
    else if (currentCount > 3)
    {
        currentCount = 0;
        return;
    }
    else if (ballCount > 2)
    {
        /* printf("ballCount = %d\n", ballCount); */
        ballCount = 0;
        checkAround(destructionStart,destructionStart->color);
        for(j = 1; j<GRIDX; j++){
            if (ballgrid[1][j].color){
                checkIsland(&ballgrid[1][j]);
            }

        }
        printGrid();
        DestroyIsland();
        destructionStart = NULL;
        currentCount = 0;
        health++;
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

void DestroyIsland(){
    int i, j;
    for (i=1; i<BALLY-1; i++)
        for(j=1; j<GRIDX; j++){
            if (ballgrid[i][j].remain){
                ballgrid[i][j].remain = 0;
            }
            else{
                if (ballgrid[i][j].color){
                    SDL_Delay(25);
                    RefreshScreen();
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
    printGrid();
}
