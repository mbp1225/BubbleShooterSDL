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

/*Velocidade da bolha*/
const int MSPEED = 8;

/*Quantidade de bolhas na primeira linha*/
const int BALLX = 20;
const int BALLY = 20;

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
} NPC;

/*
 * Global Variables
 */

int clicked = 0;
int quit = 0;

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/*The image character*/
PLAYER ball;

/*Ball Grid [Y][X]*/
NPC ballgrid[20][20];

/*Ball surface*/
SDL_Surface* BallSurface;

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

/*Background Play UI surface*/
SDL_Surface* PUI;

/*Background*/
BACKGROUND backg;

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
void CeilingCollision();

/*checks ball collision against an NPC*/
void NPCCollision();

/*calls NPC and Ceiling Collision*/
void collision();

/*checks if the balls around are the same color*/
void checkAround(int color, NPC* colNPC);

/*Prepares game initialization and variables*/
int PrepareGame();

/*Prints the screen surface and its updates*/
void RefreshScreen(void);

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

void collision()
{
  /*Checks if there's any ceiling collision for it inside the function*/
  CeilingCollision();

  /*Checks if there's any NPC collision for it inside the function*/
  NPCCollision();
}

void WallCollision()
{
    if ( (ball.posX + IMAGE_WIDTH > SCREEN_WIDTH) ||
         (ball.posX < 0) )
    {
        ball.stepX =- ball.stepX;
        ball.posX += ball.stepX;
    }
}

void CeilingCollision()
{
  int ballcolor;
  int newX;

  if (ball.posY < 0)
  {
      newX = (int)((ball.posX) / IMAGE_WIDTH);
      if (ball.posX > newX*IMAGE_WIDTH + IMAGE_WIDTH/2) newX++;
      ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
      ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
      ball.stepY = 0;
      ball.stepX = 0;
      clicked = 0;

      ballgrid[0][newX] = createNPC(
          0,
          newX*IMAGE_WIDTH,
          0,
          newX,
          ball.color,
          ball.image
      );
      
      drawNPC(ballgrid[1][newX]);
      ballcolor = rand() % 6 + 1;
      ball.color = ballcolor;
      ball.image = GetColor(ballcolor);
  }
}

NPC* checkCollision()
{
    int i, j;
    float dist, distX, distY;

    for(i = 0; i < BALLY; i++)
      for (j = 0; j < BALLX; j++)
      {
          if(ballgrid[i][j].color){

              distX = ballgrid[i][j].posX + IMAGE_WIDTH/2 - (ball.posX + IMAGE_WIDTH/2);
              distY = ballgrid[i][j].posY +IMAGE_HEIGHT/2 - (ball.posY + IMAGE_HEIGHT/2);
              dist = sqrt(pow(distX, 2) + pow(distY, 2));

              ballgrid[i][j].distX = distX;
              ballgrid[i][j].distY = distY;
              ballgrid[i][j].dist = dist;
              if (dist < IMAGE_WIDTH - 4)
              {
                  if(ball.color == ballgrid[i][j].color){
                      /*
                      ##CHECK AROUND LIGAR DEPOIS!
                      checkAround(ball.color, &ballgrid[i][j]);
                      */
                      printf("Ball color: %d\nBall Index: %d\n",ballgrid[i][j].color,j);
                  }
                  printf("ballgrid centerY = %f\nplayer centerY = %f\n", ballgrid[i][j].centerY, ball.centerY);
                  /*
                    COLTYPE CODES:
                         6 1
                        5 O 2
                         4 3
                  */
                  if (ball.centerX > ballgrid[i][j].centerX)
                  {
                    if (ball.centerY < IMAGE_HEIGHT/3 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 1;
                    else if (ball.centerY > (IMAGE_HEIGHT/3) * 2 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 3;
                    else ballgrid[i][j].coltype = 2;
                  }
                  
                  else
                  {  
                    if (ball.centerY < IMAGE_HEIGHT/3 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 6;
                    else if (ball.centerY > (IMAGE_HEIGHT/3) * 2 + ballgrid[i][j].posY) ballgrid[i][j].coltype = 4;
                    else ballgrid[i][j].coltype = 5;
                  }
                  
                  printf("coltype = %d\n", ballgrid[i][j].coltype);
                  printf("player center: %f ballij center: %f\n", ball.centerX, ballgrid[i][j].centerX);
                  return &ballgrid[i][j];
              }
          }
      }
      
    return NULL;
}

void NPCCollision()
{
	int ballcolor;
	NPC *colNPC;

	colNPC = checkCollision();

  /*
    COLTYPE CODES:
         6 1
        5 O 2
         4 3
  */

    if (colNPC)
    {
		ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
		ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
		ball.stepX = 0;
		ball.stepY = 0;
		clicked = 0;

		switch(colNPC->coltype)
		{
		  /*  case 1:
			  ballgrid[(colNPC->indexY)-1][(colNPC->indexX)+1] = createNPC(
				((colNPC->indexY)-1) * (IMAGE_HEIGHT - 5),
				colNPC->indexX * IMAGE_WIDTH,
				((colNPC->indexY)-1),
				((colNPC->indexX)+1),
				ball.color,
				ball.image
			  );
			  
			  if (!(ballgrid[(colNPC->indexY)-1][(colNPC->indexX)+1].indexY)%2)
				ballgrid[(colNPC->indexY)-1][(colNPC->indexX)+1].posX += IMAGE_WIDTH/2;
			  break;
			  case 2:
			  ballgrid[(colNPC->indexY)][(colNPC->indexX)+1] = createNPC(
				(colNPC->indexY) * (IMAGE_HEIGHT - 5),
				colNPC->indexX * IMAGE_WIDTH,
				(colNPC->indexY),
				((colNPC->indexX)+1),
				ball.color,
				ball.image
			  );
			  if (!(ballgrid[(colNPC->indexY)][(colNPC->indexX)+1].indexY)%2)
				ballgrid[(colNPC->indexY)][(colNPC->indexX)+1].posX += IMAGE_WIDTH/2;
			  break;*/
			case 4: /*certo*/
			  ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1] = createNPC(
					((colNPC->indexY)+1) * (IMAGE_HEIGHT - 5),
					colNPC->indexX * IMAGE_WIDTH - IMAGE_WIDTH/2,
					((colNPC->indexY)+1),
					((colNPC->indexX)-1),
					ball.color,
					ball.image
			  );
			  
			  if ((ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].indexY) % 2 == 0)
			  {
				  ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].posX += IMAGE_WIDTH/2;
				  ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].centerX += IMAGE_WIDTH/2;
				  ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].indexX++;
			  }
			  
			  printf("col NPC index X: %d\ncol NPC index Y: %d\n", colNPC->indexX, colNPC->indexY);
			  printf("new NPC index X: %d\nnew NPC index Y: %d\n", ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].indexX, 
																   ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].indexY);
			  printf("que = %d\n", (ballgrid[(colNPC->indexY)+1][(colNPC->indexX)-1].indexY) % 2);
			  /*CreateNPC Parameters*/
			  /*NPC createNPC(float posY, float posX, int indexY, int indexX, int color, SDL_Surface *image);*/
			  /*if (colNPC->indexY) ballgrid[(colNPC->indexY)+1][(colNPC->indexX)+1].posX += IMAGE_WIDTH;*/
			  break;
			case 3: /*certo*/
			  ballgrid[(colNPC->indexY)+1][(colNPC->indexX)] = createNPC(
				((colNPC->indexY)+1) * (IMAGE_HEIGHT - 5),
				colNPC->indexX * IMAGE_WIDTH + IMAGE_WIDTH/2,
				((colNPC->indexY)+1),
				((colNPC->indexX)),
				ball.color,
				ball.image
			  );
			  if ((ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].indexY)%2==0){
				ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].posX += IMAGE_WIDTH/2;
				ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].centerX += IMAGE_WIDTH/2;
				ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].indexX++;
			  }
			  printf("col NPC index X: %d\ncol NPC index Y: %d\n", colNPC->indexX, colNPC->indexY);
			  printf("new NPC index X: %d\nnew NPC index Y: %d\n", ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].indexX, ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].indexY);
			  printf("que = %d\n", (ballgrid[(colNPC->indexY)+1][(colNPC->indexX)].indexY)%2);
			  break;
			/*case 5:
			  ballgrid[(colNPC->indexY)][(colNPC->indexX)-1] = createNPC(
				(colNPC->indexY) * (IMAGE_HEIGHT - 5),
				colNPC->indexX * IMAGE_WIDTH,
				(colNPC->indexY),
				((colNPC->indexX)-1),
				ball.color,
				ball.image
			  );
			  if (!(ballgrid[(colNPC->indexY)][(colNPC->indexX)-1].indexY)%2)
				ballgrid[(colNPC->indexY)][(colNPC->indexX)-1].posX += IMAGE_WIDTH/2;
			  break;
			case 6:
			  ballgrid[(colNPC->indexY)-1][(colNPC->indexX)-1] = createNPC(
				((colNPC->indexY)-1) * (IMAGE_HEIGHT - 5),
				colNPC->indexX * IMAGE_WIDTH,
				((colNPC->indexY)-1),
				((colNPC->indexX)-1),
				ball.color,
				ball.image
			  );
			  if (!(ballgrid[(colNPC->indexY)-1][(colNPC->indexX)-1].indexY)%2)
				ballgrid[(colNPC->indexY)-1][(colNPC->indexX)-1].posX += IMAGE_WIDTH/2;
			  break;*/
		}
		colNPC->coltype = 0;
		ballcolor = rand() % 6 + 1;
		ball.color = ballcolor;
		ball.image = GetColor(ballcolor);
	}
}

      /*

      if (ball.posX > ballgrid[colNPC->indexY][colNPC->indexX].posX)
      {
          ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
          ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
          ball.stepX = 0;
          ball.stepY = 0;
          clicked = 0;
          */
          /*checkAround(ball.color,col+1);*/
          /*
          ballgrid[(colNPC->indexY) + 1][colNPC->indexX] = createNPC(
                      ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5),
                      colNPC->indexX*IMAGE_WIDTH + IMAGE_WIDTH/2,
                      (colNPC->indexY) + 1,
                      colNPC->indexX,
                      ball.color,
                      ball.image);

          printf("%d\n", ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5));*/
          /*if(ballgrid[(colNPC->indexY) + 1][colNPC->indexX].indexY % 2) ballgrid[(colNPC->indexY) + 1][colNPC->indexX].posX += IMAGE_WIDTH/2;*/
          /*drawNPC(ballgrid[(colNPC->indexY) + 1][colNPC->indexX]);
      }
     
      else
      {
          ball.posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
          ball.posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
          ball.stepX = 0;
          ball.stepY = 0;
          clicked = 0;
          */
          /*checkAround(ball.color,col-1);*/
          /*
          ballgrid[colNPC->indexY+1][(colNPC->indexX) - 1] = createNPC(
                      ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5),
                      ((colNPC->indexX) - 1)*IMAGE_WIDTH + IMAGE_WIDTH/2,
                      (colNPC->indexY) + 1,
                      (colNPC->indexX) - 1,
                      ball.color,
                      ball.image);

          printf("%d\n", ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5));*/
          /*if(ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1].indexY % 2) ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1].posX += IMAGE_WIDTH/2;*/
          /*drawNPC(ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1]);
      }
      ballcolor = rand()%6+1;
      ball.color = ballcolor;
      ball.image = GetColor(ballcolor);

    }
}*/

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
	
	return n;

}

/*makes BACKGROUND*/
BACKGROUND  makeBACKGROUND()
{
  BACKGROUND b;
  b.image = loadSurface( "./Images/bg.png" );
  return b;
}

/*
    COLOR CODES
    0 = null
    1 = red
    2 = yellow
    3 = green
    4 = turquoise
    5 = blue
    6 = pink
*/

SDL_Surface* GetColor(int color)
{
    SDL_Surface* ColorSurface;

    switch(color)
    {
        case 1:
            ColorSurface = loadSurface( "./Images/red.png" );
            break;
        case 2:
            ColorSurface = loadSurface( "./Images/yellow.png" );
            break;
        case 3:
            ColorSurface = loadSurface( "./Images/green.png" );
            break;
        case 4:
            ColorSurface = loadSurface( "./Images/turquoise.png" );
            break;
        case 5:
            ColorSurface = loadSurface( "./Images/blue.png" );
            break;
        case 6:
            ColorSurface = loadSurface( "./Images/pink.png" );
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
	for (i = 0; i < 1; i++)
	{
		for (j=0; j < ballX; j++)
		{
			ballcolor = rand() % 6 + 1;
    		BallSurface = GetColor(ballcolor);
			ballgrid[i][j] = createNPC(
				i*IMAGE_HEIGHT,
				j*IMAGE_WIDTH,
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
  SDL_MapRGB( gScreenSurface->format, 0xFF, 0xFF, 0xFF ) );

  drawBACKGROUND(backg);

  drawPLAYER(ball);

  for (i = 0; i < BALLY; i++)
    for (j = 0; j < BALLX; j++)
        drawNPC(ballgrid[i][j]);

  /*Update the surface*/
  SDL_UpdateWindowSurface( gWindow );

  /* Not so good solution, depends on your computer*/
  SDL_Delay(5);
}


void shoot(){
  int Mx, My;

  SDL_GetMouseState( &Mx, &My );
  /*Reinterpretando Mx e My com origem no centro de ball*/
  Mx = Mx - SCREEN_WIDTH/2;
  My = SCREEN_HEIGHT - My - IMAGE_HEIGHT/2;
  printf("%d\n", Mx);
  printf("%d\n", My);
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

    /*Por enquanto, este é o play(){*/
    SDL_Event e;

    while( SDL_PollEvent(&e) ) 
    {
        switch (e.type) 
        {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (e.button.button == SDL_BUTTON_LEFT && !clicked) shoot();
                break;
        }
    }

    if(clicked)
    {
		movePLAYER();
		collision();
		/*
		repositioning();
		checkAround();
	}
	* 
    if (!health){
		newline();
        health = 5;
    }
      
    newball();*/
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
        gWindow = SDL_CreateWindow( "Pelo menos o Catarina tem que tirar 1.5", SDL_WINDOWPOS_UNDEFINED, 
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
            Uint32 colorkey = SDL_MapRGBA( optimizedSurface->format, 0, 0, 0, 0xFF);
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


  ballcolor = rand() % 6 + 1;
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
  createGrid(BALLY, BALLX);

  /*Create PLAYER*/

  ball = createPLAYER((SCREEN_WIDTH/2 - IMAGE_WIDTH/2),
                   (SCREEN_HEIGHT - IMAGE_HEIGHT),
                    0,
                    0,
                    ballcolor,
                    BallSurface);

  return 0;
}

void checkAround(int color, NPC* colNPC)
{
	ballgrid[colNPC->indexY][colNPC->indexX].color = 0;
	ball.color = 0;

	if (colNPC->indexX < 0 && colNPC->indexX > BALLX)
	{
		return ;
	}
	else
	{
		if (ballgrid[colNPC->indexY][(colNPC->indexX)-1].color == color)
		{
            printf("Left is same\n");
			if ((colNPC->indexX) - 1 > 0)
			{
				checkAround(color, &ballgrid[colNPC->indexY][(colNPC->indexX)-1]);
			}
            /*ballgrid[0][Xindex-1].color = 0;*/
	        SDL_FreeSurface( ballgrid[colNPC->indexY][(colNPC->indexX)-1].image );
		}
		if (ballgrid[colNPC->indexY][(colNPC->indexX)+1].color == color)
		{
            printf("Right is same\n");
			if ((colNPC->indexX) + 1 < BALLX)
			{
				checkAround(color, &ballgrid[colNPC->indexY][(colNPC->indexX)+1]);
			}
			/*ballgrid[0][Xindex+1].color = 0;*/
            SDL_FreeSurface( ballgrid[colNPC->indexY][(colNPC->indexX)+1].image );
		}
	}
	return ;
}
