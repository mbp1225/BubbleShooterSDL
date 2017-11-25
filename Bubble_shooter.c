/*
 * Teste 2.3
 * Este programa e um prototipo do jogo Bubble Shooter
 * 17/10/16
 *
 * Nome e DRE dos integrantes do grupo:
 * 
 * Nome: Eduardo Melo
 * DRE: 117200581
 *
 * Nome: Matheus Pinheiro
 * DRE: 117249179
 *
 * Nome: Rafael Fernandes
 * DRE: 117196229
 *
 */
 
 /*Precisa eliminar as ilhas de bolas?*/
 
 /*Como ja tinha falado com o Catarina, troquei todo
  * i e j para
  * x e y e inverti ja todos os eixos
  * assim fica mais claro o que estamos fazendo, 
  * alem de conferir se os eixos estao certos*/

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
const int MSPEED = 1;

/*Quantidade de bolhas no eixo x, e consequentemente quantas colunas tera no eixo y*/
const int BALLX = 20;
const int BALLY = 20;

const int false = 0;
const int true = 1;

const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;

/*Distancia entre as bolas e a parede*/
const int BALL_WALL_DIST = 24;

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
    float centroX;
    float centroY;
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

/*certo eh [X][Y] >>> Ball Grid [Y][X]*/
NPC ballgrid[20][20];

SDL_Surface* BallSurface;/*Ball surface*/

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* PUI = NULL;

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
void movePLAYER(PLAYER *p);

/*Gets ball color*/
SDL_Surface* GetColor(int color);

/*checks ball collision*/
NPC* checkCollision();

/*checks ball collision against the wall*/
void WallCollision();

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
void drawBACKGROUND();

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

void movePLAYER(PLAYER *p)
{
    int ballcolor;
    int col;
    NPC *colNPC;

    if (clicked)
    {
        p->posX += p->stepX;
        p->posY += p->stepY;

        /*Checks if there's any wall collision for it inside the function*/
        WallCollision();

        /*Ceiling Collision*/
        if ( (p->posY + IMAGE_HEIGHT > SCREEN_HEIGHT) ||
             (p->posY < BALL_WALL_DIST) )
        {
            col = (int)((p->posX)/IMAGE_WIDTH);
            
            if (p->posX > col*IMAGE_WIDTH + IMAGE_WIDTH/2) col++;
            
            p->posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
            p->posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
            p->stepY = 0;
            p->stepX = 0;
            clicked = 0;

            ballgrid[0][col] = createNPC(
                BALL_WALL_DIST,
				col*IMAGE_WIDTH - (BALL_WALL_DIST/2),
                0,
                col,
                p->color,
                p->image
            );
            
            drawNPC(ballgrid[1][col]);
            ballcolor = rand() % 6 + 1;
            ball.color = ballcolor;
            p->image = GetColor(ballcolor);
        }

        colNPC = checkCollision();

          if (colNPC)
          {
            if (p->posX > ballgrid[colNPC->indexX][colNPC->indexY].posX)
            {
                p->posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
                p->posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
                p->stepX = 0;
                p->stepY = 0;
                clicked = 0;

                /*checkAround(ball.color,col+1);*/
				
				/*>>> Consertar <<<*/
                ballgrid[(colNPC->indexX)][(colNPC->indexY) + 1] = createNPC(
                                    ((colNPC->indexX) + 1)*IMAGE_HEIGHT + BALL_WALL_DIST,
                                    (colNPC->indexY)*IMAGE_WIDTH + BALL_WALL_DIST + IMAGE_WIDTH/2,
    				                /*((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5),
                        		    colNPC->indexX*IMAGE_WIDTH + IMAGE_WIDTH/2,*/
    				                (colNPC->indexY) + 1,
    				                colNPC->indexX,
                                    p->color,
    				                p->image);
    				                
                printf("%d\n", ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5));
                                    
                /*if(ballgrid[(colNPC->indexY) + 1][colNPC->indexX].indexY % 2) ballgrid[(colNPC->indexY) + 1][colNPC->indexX].posX += IMAGE_WIDTH/2;*/
                drawNPC(ballgrid[(colNPC->indexX) + 1][colNPC->indexY]);
            }
            
            else
            {
                p->posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
                p->posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
                p->stepX = 0;
                p->stepY = 0;
                clicked = 0;

                /*checkAround(ball.color,col-1);*/

                ballgrid[(colNPC->indexX) + 1][(colNPC->indexY) - 1] = createNPC(
					((colNPC->indexX) + 1)*IMAGE_HEIGHT + BALL_WALL_DIST,
					((colNPC->indexY) - 1)*IMAGE_WIDTH + BALL_WALL_DIST + IMAGE_WIDTH/2,
					/*((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5),
					((colNPC->indexX) - 1)*IMAGE_WIDTH + IMAGE_WIDTH/2,*/
					(colNPC->indexX) + 1,
					(colNPC->indexY) - 1,
					p->color,
					p->image);
    				                
				printf("%d\n", ((colNPC->indexY) + 1) * (IMAGE_HEIGHT-5));
                                    
                /*if(ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1].indexY % 2) ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1].posX += IMAGE_WIDTH/2;*/
                drawNPC(ballgrid[(colNPC->indexY) + 1][(colNPC->indexX) - 1]);
            }
            
            ballcolor = rand() % 6 + 1;
            ball.color = ballcolor;
            p->image = GetColor(ballcolor);
            
        }
    }
}

void WallCollision()
{
    if ( (ball.posX + IMAGE_WIDTH > SCREEN_WIDTH - 24) ||
         (ball.posX < BALL_WALL_DIST) )
    {
        ball.stepX = -ball.stepX;
        ball.posX += ball.stepX;
    }
}

NPC* checkCollision()
{
    int x, y;
    float dist, distX, distY;

    for(x = 0; x < BALLX; x++)
      for (y = 0; y < BALLY; y++)
      {
          if(ballgrid[x][y].color){
              distX = (ballgrid[x][y].posX - ball.posX);
              distY = (ballgrid[x][y].posY - ball.posY);
              dist = sqrt(pow(distX, 2) + pow(distY, 2));
              ballgrid[x][y].distX = distX;
              ballgrid[x][y].distY = distY;
              ballgrid[x][y].dist = dist;
              if (dist < IMAGE_WIDTH - 10)
              {
                  /*printf("ballcolor = %d\nballgrid %d color = %d\n", ball.color, j, ballgrid[i][j].color);*/
  				 /*The bit where I check if the balls have the same collor and kill them*/
                  if(ball.color == ballgrid[x][y].color){
                      checkAround(ball.color, &ballgrid[x][y]);
                      printf("Ball color: %d\nBall Index: %d\n", ballgrid[x][y].color, y);
                  }
                  return &ballgrid[x][y];
              }
          }
      }
      
    return NULL;
}

/*Create PLAYER*/
PLAYER createPLAYER( float posX, float posY, float stepX, float stepY, int color,
               SDL_Surface *image) 
{
    PLAYER p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.color = color;
    p.image = image;
    return p;
}

/*Create NPC*/
NPC createNPC(float posY, float posX, int indexY, int indexX, int color,
				SDL_Surface *image) 
{
	NPC n;

	n.posX = posX;
    n.posY = posY;
    n.indexY = indexY;
    n.indexX = indexX;
    n.color = color;
    n.image = image;
    n.centroX = posX+IMAGE_WIDTH/2;
    n.centroY = posX+IMAGE_HEIGHT/2;
    return n;

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
	int x, y;
    int ballcolor;
    SDL_Surface* BallSurface;

	/*>>> Mudar isso quando for pro jogo completo, pois so esta considerando uma linha*/
	for (x = 0; x < 1; x++)
	{
		for (y = 1; y < ballY - 1; y++)
		{
			ballcolor = rand() % 6 + 1;
			BallSurface = GetColor(ballcolor);
			ballgrid[x][y] = createNPC(
				x*IMAGE_HEIGHT + BALL_WALL_DIST,
				/*Se x nunca chega a 1, x = 0, estah multiplicando por zero logo aqui abaixo entao
				 * e qual a ordem dessa conta? % 2 eh a ultima coisa mesmo?*/
				y*IMAGE_WIDTH - (BALL_WALL_DIST/2) + (IMAGE_WIDTH/2 * x % 2),
				x,
				y,
				ballcolor,
				BallSurface
			);
			
			drawNPC(ballgrid[x][y]);
			
		}
	}
}

/*Displays player on screen*/
void drawPLAYER(PLAYER p)
{
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0; srcRect.y = 0;
    srcRect.w = IMAGE_WIDTH;
    srcRect.h = IMAGE_HEIGHT;
    dstRect.x = p.posX;
    dstRect.y = p.posY;
	SDL_BlitSurface( p.image, &srcRect, gScreenSurface, &dstRect );
}

void drawBACKGROUND()
{
	SDL_BlitSurface( PUI, NULL, gScreenSurface, NULL );
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
  int x, y;

  /*Fill the surface white*/

  SDL_FillRect( gScreenSurface, NULL,
  SDL_MapRGB( gScreenSurface->format, 0xFF, 0xFF, 0xFF ) );

  drawBACKGROUND();

  drawPLAYER(ball);

  for (x = 0; x < BALLX; x++)
    for (y = 0; y < BALLY; y++)
        drawNPC(ballgrid[x][y]);

  /*Update the surface*/
  SDL_UpdateWindowSurface( gWindow );

  /* Not so good solution, depends on your computer*/
  /*SDL_Delay();*/
}


void shoot()
{
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

    /*CRIANDO UMA EXCEÇÃO PARA ANGULOS ACIMA DE 172 E ABAIXO DE 8 GRAUS
     * sen 8 = 0.99
     * cos 8 = -0.139 */

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

void game()
{
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
                if (e.button.button == SDL_BUTTON_LEFT && !clicked)
                {
                    shoot();
                }
                break;
        }
        /*game(e);*/
    }

    /*Moves Player*/
    if(clicked) movePLAYER(&ball);

    RefreshScreen();
}

int init() 
{
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
        /*Create window*/
        gWindow = SDL_CreateWindow( "Bubble Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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

    PUI = loadSurface( "./Images/bg.png" );

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
  ballgrid[colNPC->indexX][colNPC->indexY].color = 0;
  ball.color = 0;

	if (colNPC->indexX < 0 && colNPC->indexY > BALLX)
	{
		return ; 
	}
	
	else
	{
		if (ballgrid[colNPC->indexX][(colNPC->indexY) - 1].color == color)
		{
            printf("Left is same\n");
			if ((colNPC->indexY) - 1 > 0)
			{
				checkAround(color, &ballgrid[colNPC->indexX][(colNPC->indexY) - 1]);
			}
            /*ballgrid[0][Xindex-1].color = 0;*/
	        SDL_FreeSurface( ballgrid[colNPC->indexX][(colNPC->indexY) - 1].image );
		}
		
		if (ballgrid[colNPC->indexY][(colNPC->indexX) + 1].color == color)
		{
            printf("Right is same\n");
			if ((colNPC->indexY) + 1 < BALLX)
			{
				checkAround(color, &ballgrid[colNPC->indexX][(colNPC->indexY) + 1]);
			}
			/*ballgrid[0][Xindex+1].color = 0;*/
            SDL_FreeSurface( ballgrid[colNPC->indexX][(colNPC->indexY)+1].image );
		}
	}
	return ;
}
