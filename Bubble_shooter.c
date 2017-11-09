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
const int BALLX = 9;
const int BALLY = 19;

const int false = 0;
const int true = 1;

const int IMAGE_WIDTH = 32;
const int IMAGE_HEIGHT = 32;

typedef struct _NPC 
{
    float posX;
    float posY;
    float stepX;
    float stepY;
    SDL_Surface* image;
    int imgW;
    int imgH;
} NPC;

typedef struct _POS
{
	int indexX;
	int indexY;
} POS;

/*
 * Global Variables
 */
 
int clicked = 0;

/*The window we'll be rendering to*/
SDL_Window* gWindow = NULL;

/*The image character*/
NPC ball, ballz[15][20];

/*The surface contained by the window*/
SDL_Surface* gScreenSurface = NULL;

/*Current displayed PNG image*/
SDL_Surface* gJPGSurface = NULL;

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

/*Create NPC*/
NPC createNPC( float posX, float posY, float stepX, float stepY, SDL_Surface *image);

/*Move NPC*/
void moveNPC(NPC *p);

/*Checks Collision*/
POS checkCollision();

/*Preenche primeira fileira com as bolinhas*/
void draw(NPC p){
	SDL_Rect srcRect, dstRect;
	srcRect.x = 0; srcRect.y = 0;
    srcRect.w = IMAGE_WIDTH;
    srcRect.h = IMAGE_HEIGHT;
    dstRect.x = p.posX;
    dstRect.y = p.posY;
	SDL_BlitSurface( p.image, &srcRect, gScreenSurface, &dstRect );
}

int main( int argc, char* args[] ) {
    SDL_Event e;/*Event handler*/
    SDL_Surface* balls;/*Ball surface*/
    int quit = false;/*Main loop flag*/
    int Mx, My;
    int i, j,noBalls = 1;

    /*Start up SDL and create window*/
    if( !init() ) {
        printf( "Failed to initialize!\n" );
        return 1;
    }

    /*Load media*/
    if( !loadMedia() ) {
        printf( "Failed to load media!\n" );
        return 2;
    }
    balls = loadSurface("./Images/blue.tga");

    /*Create NPC*/
    ball = createNPC((SCREEN_WIDTH/2 - IMAGE_WIDTH/2),
                     (SCREEN_HEIGHT - IMAGE_HEIGHT),
                      0,
                      0,
                      balls);

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
						 * ball.stepX = 0;
						 * ball.stepY = -8
						 */
						}
				}
					break;

            }
        }

        /*Fill the surface white*/
        SDL_FillRect( gScreenSurface, NULL, 
					SDL_MapRGB( gScreenSurface->format, 0xFF, 0xFF, 0xFF ) );

		if(clicked == 1) moveNPC(&ball);
        draw(ball);
        
        if(noBalls)
        {
			for (i = 1; i <= BALLY; i++)
			{
				for (j = 1; j <= 15; j++)
				{
					if (j % 2 == 0) 
					{
						ballz[j][i] = createNPC(
								(i-1)*IMAGE_WIDTH + (IMAGE_WIDTH/2) - (IMAGE_WIDTH/4),
								(j-1) * IMAGE_HEIGHT + (IMAGE_WIDTH/4),
								0,
								0,
								balls);
					}
					else 
					{
						ballz[j][i] = createNPC(
								(i-1)*IMAGE_WIDTH + (IMAGE_WIDTH) - (IMAGE_WIDTH/4),
								(j-1) * IMAGE_HEIGHT + (IMAGE_WIDTH/4),
								0,
								0,
								balls);
					}
					
					
				 }
			}
			noBalls = 0;
		} 
		
		for (i = 1; i <= BALLY; i++)
        {
			for (j = 1; j <= BALLX; j++)
			{
				draw (ballz[j][i]);
			}
		} 

        /*Update the surface*/
        SDL_UpdateWindowSurface( gWindow );

        /* Not so good solution, depends on your computer*/
        SDL_Delay(5);
    }

    /*Free resources and closing SDL*/
    closing();

    return 0;
}


void moveNPC(NPC *p) {
    POS hitPos;
    float distX;
    
    if (clicked == 1){
        p->posX += p->stepX;
        p->posY += p->stepY;

        hitPos = checkCollision();
        /*Corrige o fato da bola jogada nao estar precisamente entre outras duas paradas*/

        if (hitPos.indexX)
        { 
			distX = (ball.posX + IMAGE_WIDTH/2) - (ballz[hitPos.indexX][hitPos.indexY].posX);
			printf("%.2f distance on X axis\n",distX);
			ball.stepX = 0;
			ball.stepY = 0;
            /*A primeira linha p->posX está com (p->poX *>* 0)
             *e não com *>=*
             *pois a bola parada está eternamente em colisão, 
             *o que faz com que o ternário abaixo
             *entrasse em suas duas condições
             * 
             * Recalcular collision de acordo com distX
             * 
            p->posX = (p->posX > 0)? (IMAGE_WIDTH * col) - (IMAGE_WIDTH/2) : (IMAGE_WIDTH * col) + (IMAGE_WIDTH/2) ;
            p->stepY = 0;
            p->stepX = 0;
            p->posY = IMAGE_HEIGHT - 5;
            clicked = 0 permite ao usuario jogar mais uma bolinha
            clicked = 0; 
        }

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
        */
		}
    }
}

POS checkCollision(){ /*REFAZER TESTE DE COLISAO*/
	int i,j;
	POS colPosition;
	float dist,distX,distY; 
	
	
		
    /*Dist = distancia do centro da bolinha i ate o centro da bolinha que esta se movendo*/
	/*float distX, distY, dist;*/
	/*for(i = 0; i < BALLX; i++)
	{
		distX = (ball.posX + IMAGE_WIDTH/2) - (ballz[0][i].posX + IMAGE_WIDTH/2);
		distY = (ball.posY + IMAGE_WIDTH/2) - (ballz[0][i].posY + IMAGE_HEIGHT/2);
		dist = sqrt((distX*distX) + (distY*distY));
        * 
         * em dist *<=* IMAGE_WIDTH,
         * o *<=* esta presente pois do contrario
         * quando lançada em um angulo de 90 graus,
         * a bolinha nao colidia
         */
         /*
		if (dist <= IMAGE_WIDTH)
		{
			printf("bolinha da colisão = %d\n", i);
            printf("distX = %f\n", distX);
			return i+1;
		}
		* */
	
	for (i = 1; i <= BALLY; i++)
	{
		for (j = 1; j <= BALLX; j++)
		{
			if ((!(ballz[j][i-1]).image || !(ballz[j][i+1]).image ||
				!(ballz[j+1][i]).image || !(ballz[j-1][i]).image ||
				!(ballz[j+1][i+1]).image || !(ballz[j+1][i+1]).image)
				&&(ballz[j][i+1]).image)
			{
					distX = (ball.posX + IMAGE_WIDTH/2) - (ballz[j][i].posX + IMAGE_WIDTH/2);
					distY = (ball.posY + IMAGE_WIDTH/2) - (ballz[j][i].posY + IMAGE_HEIGHT/2);
					dist = sqrt((distX*distX) + (distY*distY));
					
					if (dist < IMAGE_WIDTH)
					{
						colPosition.indexX = j;
						colPosition.indexY = i;
						return colPosition;
					}
					
			}					
		 }
	}
	/*}*/
	colPosition.indexX = 0;
	colPosition.indexY = 0;
		
	return colPosition;
}




/*Create NPC*/
NPC createNPC( float posX, float posY, float stepX, float stepY,
               SDL_Surface *image) {
    NPC p;

    p.posX = posX;
    p.posY = posY;
    p.stepX = stepX;
    p.stepY = stepY;
    p.image = image;
    return p;
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

    /*Load PNG surface*/
    /*NAO ESTA IMPORTANDO QUAL COR E COLOCADA AQUI*/
    gJPGSurface = loadSurface( "./Images/red.tga" );
    if( gJPGSurface == NULL) {
        printf( "Failed to load image! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        Uint32 colorkey = SDL_MapRGB( gJPGSurface->format, 0, 0xFF, 0 );
        SDL_SetColorKey( gJPGSurface,1, colorkey );
    }
    return success;
}                                                                                         

void closing() {
    /*Free loaded image*/
    SDL_FreeSurface( gJPGSurface );
    gJPGSurface = NULL;

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
            Uint32 colorkey = SDL_MapRGB( optimizedSurface->format, 0, 0xFF, 0 );
            SDL_SetColorKey( optimizedSurface,1, colorkey );
        }

        /*Get rid of old loaded surface*/
        SDL_FreeSurface( loadedSurface );
    }

    return optimizedSurface;
}
