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
const int BALLY = 2;

const int false = 0;
const int true = 1;

const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;

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
} NPC;

/*
 * Global Variables
 */

int clicked = 0;

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/*The image character*/
PLAYER ball;

/*Ball Grid [Y][X]*/
NPC ballgrid[2][20];

SDL_Surface* BallSurface;/*Ball surface*/

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

int checkCollision();

int checkAround(int color,int Xindex);

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

SDL_Surface* GetColor(int color);

/*Displays player on screen*/
void drawPLAYER(PLAYER p){
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0; srcRect.y = 0;
    srcRect.w = IMAGE_WIDTH;
    srcRect.h = IMAGE_HEIGHT;
    dstRect.x = p.posX;
    dstRect.y = p.posY;
	SDL_BlitSurface( p.image, &srcRect, gScreenSurface, &dstRect );
}

/*Displays NPC on screen*/
void drawNPC(NPC n){
    if(n.color){
    	SDL_Rect srcRect, dstRect;
    	srcRect.x = 0; srcRect.y = 0;
        srcRect.w = IMAGE_WIDTH;
        srcRect.h = IMAGE_HEIGHT;
        dstRect.x = n.posX;
        dstRect.y = n.posY;
    	SDL_BlitSurface( n.image, &srcRect, gScreenSurface, &dstRect );
    }
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

int main( int argc, char* args[] ) {
    SDL_Event e;/*Event handler*/

    int quit = false;/*Main loop flag*/
    int Mx, My;
    int i, j;
    int ballcolor;

    /*Start up SDL and create window*/
    if( !init() ) {
        printf( "Failed to initialize!\n" );
        return 1;
    }


    ballcolor = rand()%6+1;
    BallSurface = GetColor(ballcolor);

    /*Load media*/
    if( !loadMedia() ) {
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

    /*While application is running*/
    while( !quit ) {
        while( SDL_PollEvent( &e ) != 0 ) {
            switch (e.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (e.key.keysym.sym == SDLK_ESCAPE) quit = true;
                    break;

			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT && !clicked){
					SDL_GetMouseState( &Mx, &My );
					/*REINTERPRETANDO Mx E My COM ORIGEM NO CENTRO DE BALL*/
					Mx = Mx - SCREEN_WIDTH/2;
					My = SCREEN_HEIGHT - My - IMAGE_HEIGHT/2;
                    /*CLICK NAO FUNCIONA SE MY < CENTRO DA BOLINHA*/
					if(My > 0){
						/*REINTERPRETANDO X E Y PARA QUE A HIPOTENUSA SEJA 1 (CALCULANDO O STEP)*/
						ball.stepX = Mx / sqrt((Mx*Mx) + (My*My));
						ball.stepY = -My / sqrt((Mx*Mx) + (My*My));

						/* CASO NECESSARIO, SABER QUAL STEPX E STEPY
						printf("step X = %f\nstep Y = %f\n", ball.stepX, ball.stepY);
						printf("%d, %d\n", Mx, My);
						*/

						/*CRIANDO UMA EXCEÇÃO PARA ANGULOS ACIMA DE 172 E ABAIXO DE 8 GRAUS
                         * sen 8 = 0.99
                         * cos 8 = -0.139
                         */

						if (ball.stepX > 0.99) { ball.stepX = 0.99; ball.stepY = -0.139; }
						if (ball.stepX < -0.99) { ball.stepX = -0.99; ball.stepY = -0.139; }
						ball.stepY*= MSPEED;
						ball.stepX*= MSPEED;
						clicked = 1;
						/* P/ TESTAR A BOLINHA IR RETO
						  ball.stepX = 0;
						  ball.stepY = -8; */

						}
				}
					break;

            }
        }

        /*Fill the surface white*/
        SDL_FillRect( gScreenSurface, NULL,
					SDL_MapRGB( gScreenSurface->format, 0xFF, 0xFF, 0xFF ) );

		/*Moves Player*/
		if(clicked == 1) movePLAYER(&ball);

		drawPLAYER(ball);

		for (i = 0; i < BALLY; i++)
			for (j = 0; j < BALLX; j++)
				drawNPC(ballgrid[i][j]);

        /*Update the surface*/
        SDL_UpdateWindowSurface( gWindow );

        /* Not so good solution, depends on your computer*/
        SDL_Delay(5);
    }

    /*Free resources and closing SDL*/
    closing();

    return 0;
}


void movePLAYER(PLAYER *p)
{
    int ballcolor;
    int col;

    if (clicked)
    {
        p->posX += p->stepX;
        p->posY += p->stepY;

        if ( (p->posX + IMAGE_WIDTH > SCREEN_WIDTH) ||
             (p->posX < 0) )
        {
            p->stepX = -p->stepX;
            p->posX += p->stepX;
        }
        if ( (p->posY + IMAGE_HEIGHT > SCREEN_HEIGHT) ||
             (p->posY < 0) )
        {
            p->stepY = 0;
            p->posY = 0;
            p->stepX = 0;
        }
    }

    col = checkCollision();

    if (col && clicked)
    {
        col--;
        if (p->posX > ballgrid[0][col].posX)
        {
            p->posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
            p->posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
            p->stepX = 0;
            p->stepY = 0;
            clicked = 0;

            checkAround(ball.color,col+1);

            ballgrid[1][col] = createNPC(
				                IMAGE_WIDTH - 5,
                    		col*IMAGE_HEIGHT + IMAGE_WIDTH/2,
				                1,
				                col,
                        p->color,
				                p->image);
			      drawNPC(ballgrid[1][col]);

            ballcolor = rand()%6+1;
            ball.color = ballcolor;
            p->image = GetColor(ballcolor);
        }
        else
        {
            p->posX = (SCREEN_WIDTH/2 - IMAGE_WIDTH/2);
            p->posY = (SCREEN_HEIGHT - IMAGE_HEIGHT);
            p->stepX = 0;
            p->stepY = 0;
            clicked = 0;

            checkAround(ball.color,col-1);

            ballgrid[1][col-1] = createNPC(
				                IMAGE_WIDTH - 5,
                        (col-1)*IMAGE_HEIGHT + IMAGE_WIDTH/2,
				                1,
				                col-1,
                        p->color,
				                p->image);
			      drawNPC(ballgrid[1][col-1]);

            ballcolor = rand()%6+1;
            ball.color = ballcolor;
            p->image = GetColor(ballcolor);
        }
    }
}

int checkCollision()
{
    int i;
    int dist;

    for (i = 0; i < BALLX; i++)
    {
        if(ballgrid[0][i].color){
            dist = sqrt(((ballgrid[0][i].posX -ball.posX) * (ballgrid[0][i].posX -ball.posX)) +
                        ((ballgrid[0][i].posY -ball.posY) * (ballgrid[0][i].posY -ball.posY)));
            if (dist < IMAGE_WIDTH)
            {
                /*printf("ballcolor = %d\nballgrid %d color = %d\n", ball.color, i, ballgrid[0][i].color);*/
				/*The bit where I check if the balls have the same collor and kill them*/
                if(ball.color == ballgrid[0][i].color){
					          checkAround(ball.color,i);
					          printf("Ball color: %d\nBall Index: %d\n",ballgrid[0][i].color,i);
                }
                return i+1;
            }
        }
    }
    return 0;
}

/*Create PLAYER*/
PLAYER createPLAYER( float posX, float posY, float stepX, float stepY, int color,
               SDL_Surface *image) {
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
				SDL_Surface *image) {
	NPC n;

	n.posX = posX;
    n.posY = posY;
    n.indexY = indexY;
    n.indexX = indexX;
    n.color = color;
    n.image = image;
    return n;

}

SDL_Surface* GetColor(int color){
    SDL_Surface* ColorSurface;

    switch(color){
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
void createGrid(int ballY, int ballX){
	int i, j;
    int ballcolor;
    SDL_Surface* BallSurface;

    /*LEMBRAR DE TROCAR ISTO QUANDO FOR PARA MATRIZ*/
	for (i=0; i<1; i++){
		for (j=0; j<ballX; j++){
        ballcolor = rand()%6+1;
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

int init() {
    /*Initialization flag*/
    int success = true;

    srand(time(NULL));

    /*Initialize SDL*/
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else {
        /*Create window*/
        gWindow = SDL_CreateWindow( "Bubble Shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL ) {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else {
            /*Initialize JPG and PNG loading */
            int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) ) {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            else {
                /*Get window surface*/
                gScreenSurface = SDL_GetWindowSurface( gWindow );
            }
        }
    }



    return success;
}

int loadMedia() {
    /*Loading success flag*/
    int success = true;
    /*uint32_t colorKey;*/

    if( BallSurface == NULL) {
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

void closing() {
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

SDL_Surface* loadSurface( char *path ) {
    /*The final optimized image*/
    SDL_Surface* optimizedSurface = NULL;

    /*Load image at specified path*/
    SDL_Surface* loadedSurface = IMG_Load( path );
    if( loadedSurface == NULL ) {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
    }
    else {
        /*Convert surface to screen format*/
        optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
        if( optimizedSurface == NULL ) {
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

int checkAround(int color,int Xindex)
{
  ballgrid[0][Xindex].color = 0;
  ball.color = 0;

	if (Xindex < 0 && Xindex > BALLX)
	{
		return 0;
	}
	else
	{
		if (ballgrid[0][Xindex-1].color == color)
		{
      printf("Left is same\n");
			if (Xindex - 1 > 0)
			{
				checkAround(color, Xindex-1);
			}
			ballgrid[0][Xindex-1].color = 0;
		}
		if (ballgrid[0][Xindex+1].color == color)
		{
      printf("Right is same\n");
			if (Xindex + 1 < BALLX)
			{
				checkAround(color, Xindex+1);
			}
			ballgrid[0][Xindex+1].color = 0;
		}
	}
	return 0;
}
