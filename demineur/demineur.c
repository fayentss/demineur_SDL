#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL_ttf.h>
#include <SDL_image.h>




/*
          RENDERER FLAGS

    --SDL_RENDERER_SOFTWARE
    --SDL_RENDERER_ACCELERATED
    --SDL_RENDERER_PRESENTVSYNC
    --SDL_RENDERER_TARGETTEXTURE

*/

typedef struct Tile
{
    SDL_Rect R;
    int isRevealed;
    int isBomb;
    int isBombAround;
    int isSafe;
    int isFlag;

} Tile;

typedef struct Grid
{
    Tile* t;
    int onoff;
    int size;
    int caseSize;
    int total_size;
    int bombs;
    SDL_Window* window;
    SDL_Renderer* renderer;

} Grid;

typedef struct Difficulty
{

    int size;
    int bomb;

} Difficulty;

void SDL_ExitWithError(const char* message)
{
    SDL_Log("ERROR : %s > %s\n", message, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}

void createGrid(Grid* g)
{
    for (int i = 0; i < g->total_size; i++)
    {
        g->t[i].isBomb = 0;
        g->t[i].isRevealed = 0;
        g->t[i].isSafe = 0;
        g->t[i].isFlag = 0;
        g->t[i].isBombAround = 0;
    }

    // Création de la fenêtre + rendu
    if (SDL_CreateWindowAndRenderer(40 * g->size, 40 * g->size, 0, &g->window, &g->renderer) != 0)
        SDL_ExitWithError("Creation fenetre et rendu echoue");


    for (int i = 0; i < g->total_size; i++)
    {
        if (((i - (i % g->size)) / g->size) % 2 == 0)
        {
            if (i % 2 == 0)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case \n");
            }
            else if (i % 2 == 1)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case \n");
            }
        }
        else if (((i - (i % g->size)) / g->size) % 2 == 1)
        {
            if (i % 2 == 0)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case \n");
            }
            else if (i % 2 == 1)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case \n");
            }
        }

        g->t[i].R.h = g->t[i].R.w = 40;
        g->t[i].R.x = 40 * (i % g->size);
        g->t[i].R.y = 40 * ((i - (i % g->size)) / g->size);

        if (SDL_RenderFillRect(g->renderer, &g->t[i].R) != 0)
            SDL_ExitWithError("Error sur la case\n");

    }
    SDL_RenderPresent(g->renderer);
}

int getCoord1D(Grid* g, int x, int y)
{
    return (y * g->size + x);
}


int setDifficulty(SDL_Window* window, SDL_Renderer* renderer, Grid * g)
{

    // Création de la fenêtre + rendu
    if (SDL_CreateWindowAndRenderer(400, 600, 0, &window, &renderer) != 0)
        SDL_ExitWithError("Creation fenetre et rendu echoue");

    SDL_Rect diffBtn_One;
    SDL_Rect diffBtn_Two;
    SDL_Rect diffBtn_Three;

    diffBtn_One.h = diffBtn_Two.h = diffBtn_Three.h = 140;
    diffBtn_One.w = diffBtn_Two.w = diffBtn_Three.w = 360;
    diffBtn_One.x = diffBtn_Two.x = diffBtn_Three.x = 20;

    diffBtn_One.y = 45;
    diffBtn_Two.y = 230;
    diffBtn_Three.y = 415;

    if (SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE) != 0)
        SDL_ExitWithError("ERROR set render color");

    if (SDL_RenderFillRect(renderer, &diffBtn_One) != 0)
        SDL_ExitWithError("Error sur la case\n");

    if (SDL_SetRenderDrawColor(renderer, 255, 148, 0, SDL_ALPHA_OPAQUE) != 0)
        SDL_ExitWithError("ERROR set render color");

    if (SDL_RenderFillRect(renderer, &diffBtn_Two) != 0)
        SDL_ExitWithError("Error sur la case\n");

    if (SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE) != 0)
        SDL_ExitWithError("ERROR set render color");

    if (SDL_RenderFillRect(renderer, &diffBtn_Three) != 0)
        SDL_ExitWithError("Error sur la case\n");

    SDL_RenderPresent(renderer);

    SDL_bool DiffChoiceIsRun = SDL_TRUE;

    int diffChoice = 3;

    while (DiffChoiceIsRun)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = event.button.x;
                    int y = event.button.y;

                    if (20 < x && x < 380 && 45 < y && y < 185)
                    {
                        diffChoice = 0;
                        DiffChoiceIsRun = SDL_FALSE;
                    }
                    if (20 < x && x < 380 && 230 < y && y < 370)
                    {
                        diffChoice = 1;
                        DiffChoiceIsRun = SDL_FALSE;
                    }
                    if (20 < x && x < 380 && 415 < y && y < 555)
                    {
                        diffChoice = 2;
                        DiffChoiceIsRun = SDL_FALSE;
                    }
                    break;
                }

            case(SDL_QUIT):
                DiffChoiceIsRun = SDL_FALSE;
                g->onoff = 0;
                break;

            default:
                continue;
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return diffChoice;
}

void placeBomb(Grid* g)
{
    srand(time(NULL));

    int bomb = 0;
    while (bomb < g->bombs)
    {
        int val = rand() % (g->size * g->size);
        if (g->t[val].isSafe != 1 && g->t[val].isBomb != 1)
        {
            g->t[val].isBomb = 1;
            bomb += 1;
        }
    }
}
int isInGrid(Grid* g, int x, int y)
{
    if (x >= 0 && x < g->size && y >= 0 && y < g->size)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int isBombAtTile(Grid* g, int x, int y)
{
    if (isInGrid(g, x, y) == 1)
    {
        return g->t[getCoord1D(g, x, y)].isBomb;
    }
    else
    {
        return 2;
    }
}
void placeBombAround(Grid* g)
{
    for (int y = 0; y < g->size; y++)
    {
        for (int x = 0; x < g->size; x++)
        {
            if (isBombAtTile(g, x, y) == 0)
            {
                int bombs = 0;
                if (isBombAtTile(g, x - 1, y - 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x, y - 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x + 1, y - 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x + 1, y) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x + 1, y + 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x, y + 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x - 1, y + 1) == 1)
                {
                    bombs += 1;
                }

                if (isBombAtTile(g, x - 1, y) == 1)
                {
                    bombs += 1;
                }

                g->t[getCoord1D(g, x, y)].isBombAround = bombs;

            }
        }
    }
}
void refreshScreen(Grid* g, SDL_Surface* image, SDL_Texture* texture)
{
    for (int i = 0; i < g->total_size; i++)
    {
        if (g->t[i].isRevealed == 1)
        {
            if (SDL_SetRenderDrawColor(g->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0)
                SDL_ExitWithError("Error sur la case \n");

            if (SDL_RenderFillRect(g->renderer, &g->t[i].R) != 0)
                SDL_ExitWithError("Error sur la case\n");

            if (g->t[i].isBomb == 1)
            {
                image = IMG_Load("src/bomb.png");

                texture = SDL_CreateTextureFromSurface(g->renderer, image);

                SDL_QueryTexture(texture, NULL, NULL, &g->caseSize, &g->caseSize);


                SDL_RenderCopy(g->renderer, texture, NULL, &g->t[i].R);
            }
            else if (g->t[i].isBombAround != 0)
            {
                switch (g->t[i].isBombAround)
                {

                case 1:
                    image = IMG_Load("src/chiffre_1.png");
                    break;
                case 2:
                    image = IMG_Load("src/chiffre_2.png");
                    break;
                case 3:
                    image = IMG_Load("src/chiffre_3.png");
                    break;
                case 4:
                    image = IMG_Load("src/chiffre_4.png");
                    break;
                case 5:
                    image = IMG_Load("src/chiffre_5.png");
                    break;
                case 6:
                    image = IMG_Load("src/chiffre_6.png");
                    break;
                case 7:
                    image = IMG_Load("src/chiffre_7.png");
                    break;
                case 8:
                    image = IMG_Load("src/chiffre_8.png");
                    break;



                default:
                    break;
                }

                texture = SDL_CreateTextureFromSurface(g->renderer, image);

                SDL_QueryTexture(texture, NULL, NULL, &g->caseSize, &g->caseSize);

                SDL_RenderCopy(g->renderer, texture, NULL, &g->t[i].R);
            }
        }
        else if (g->t[i].isRevealed == 0)
        {
            if (((i - (i % g->size)) / g->size) % 2 == 0)
            {
                if (i % 2 == 0)
                {
                    if (SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
                        SDL_ExitWithError("Error sur la case \n");
                }
                else if (i % 2 == 1)
                {
                    if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                        SDL_ExitWithError("Error sur la case \n");
                }
            }
            else if (((i - (i % g->size)) / g->size) % 2 == 1)
            {
                if (i % 2 == 0)
                {
                    if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                        SDL_ExitWithError("Error sur la case \n");
                }
                else if (i % 2 == 1)
                {
                    if (SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
                        SDL_ExitWithError("Error sur la case \n");
                }
            }

            g->t[i].R.h = g->t[i].R.w = 40;
            g->t[i].R.x = 40 * (i % g->size);
            g->t[i].R.y = 40 * ((i - (i % g->size)) / g->size);

            if (SDL_RenderFillRect(g->renderer, &g->t[i].R) != 0)
                SDL_ExitWithError("Error sur la case\n");

            if (g->t[i].isFlag == 1)
            {
                image = IMG_Load("src/flag.png");

                texture = SDL_CreateTextureFromSurface(g->renderer, image);

                SDL_QueryTexture(texture, NULL, NULL, &g->caseSize, &g->caseSize);

                SDL_RenderCopy(g->renderer, texture, NULL, &g->t[i].R);
            }
        }
        
    }
    SDL_RenderPresent(g->renderer);
}
void reveleEmptyCase(Grid* g, int x, int y, int* reveledCase)
{
    if (isBombAtTile(g, x, y) == 0 && g->t[getCoord1D(g, x, y)].isBombAround == 0 && g->t[getCoord1D(g, x, y)].isRevealed == 0)
    {
        g->t[getCoord1D(g, x, y)].isRevealed = 1;
        *reveledCase -= 1;


        discoverCase(g, x - 1, y - 1, reveledCase);

        discoverCase(g, x, y - 1, reveledCase);

        discoverCase(g, x + 1, y - 1, reveledCase);

        discoverCase(g, x + 1, y, reveledCase);

        discoverCase(g, x + 1, y + 1, reveledCase);

        discoverCase(g, x, y + 1, reveledCase);

        discoverCase(g, x - 1, y + 1, reveledCase);

        discoverCase(g, x - 1, y, reveledCase);
    }
}

int discoverCase(Grid* g, int x, int y, int* reveledCase)
{
    if (isInGrid(g, x, y) == 0)
    {
        return 0;
    }
    else if (isBombAtTile(g, x, y, g->size) == 1)
    {
        g->t[getCoord1D(g, x, y)].isRevealed = 1;
        return 1;
    }
    else if (g->t[getCoord1D(g, x, y)].isRevealed == 1)
    {
        return 0;
    }

    else if (g->t[getCoord1D(g, x, y)].isBombAround > 0 && g->t[getCoord1D(g, x, y)].isRevealed == 0)
    {
        g->t[getCoord1D(g, x, y)].isRevealed = 1;
        *reveledCase -= 1;

        reveleEmptyCase(g, x - 1, y - 1, reveledCase);

        reveleEmptyCase(g, x, y - 1, reveledCase);

        reveleEmptyCase(g, x + 1, y - 1, reveledCase);

        reveleEmptyCase(g, x + 1, y, reveledCase);

        reveleEmptyCase(g, x + 1, y + 1, reveledCase);

        reveleEmptyCase(g, x, y + 1, reveledCase);

        reveleEmptyCase(g, x - 1, y + 1, reveledCase);

        reveleEmptyCase(g, x - 1, y, reveledCase);
    }
    else if (g->t[getCoord1D(g, x, y)].isBombAround == 0 && isBombAtTile(g, x, y, g->size) == 0)
    {
        reveleEmptyCase(g, x, y, reveledCase, g->size);
    }

    return 2;
}

void placeSafeTile(Grid* g, int playerChoiceX, int playerChoiceY)
{
    if (isInGrid(g, playerChoiceX, playerChoiceY) == 0)
        return;

    g->t[getCoord1D(g, playerChoiceX, playerChoiceY)].isSafe = 1;
}

void placeSafeZone(Grid* g, int playerChoiceX, int playerChoiceY)
{
    placeSafeTile(g, playerChoiceX, playerChoiceY);
    placeSafeTile(g, playerChoiceX - 1, playerChoiceY - 1);
    placeSafeTile(g, playerChoiceX, playerChoiceY - 1);
    placeSafeTile(g, playerChoiceX + 1, playerChoiceY - 1);
    placeSafeTile(g, playerChoiceX + 1, playerChoiceY);
    placeSafeTile(g, playerChoiceX + 1, playerChoiceY + 1);
    placeSafeTile(g, playerChoiceX, playerChoiceY + 1);
    placeSafeTile(g, playerChoiceX - 1, playerChoiceY + 1);
    placeSafeTile(g, playerChoiceX - 1, playerChoiceY);
}

int replay()
{
    char answer;
    printf("Veux tu rejouer ( O / N ) : ");
    while (scanf_s(" %c", &answer) != 1)
    {
        char c = getchar();
        while (c != '\n') {
            c = getchar();
        };
        printf("Erreur.\nVeux tu rejouer ( O / N ) : ");
    }

    if (answer == 'O')
    {
        return 1;
    }
    else if (answer == 'N')
    {
        return 0;
    }
    else
    {
        printf("Ta réponse n'est pas correct recommence\n");
        replay();
    }
}

void gameLoop(Grid* g, int diffChoice, Difficulty* difficulty, SDL_Surface* image, SDL_Texture* texture)
{
    g->size = difficulty[diffChoice].size;
    g->total_size = g->size * g->size;
    g->t = malloc(sizeof(Tile) * g->total_size);
    g->bombs = difficulty[diffChoice].bomb;
    g->window = g->window;
    g->renderer = g->renderer;

    createGrid(g);

    SDL_bool GameIsRunning = SDL_TRUE;

    int reveledCase = g->total_size;

    if (diffChoice == 3)
    {
        GameIsRunning = SDL_FALSE;
    }

    while (GameIsRunning)
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {

            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int x = ((event.button.x - (event.button.x % 40)) / 40);
                    int y = ((event.button.y - (event.button.y % 40)) / 40);

                    if (reveledCase == g->total_size)
                    {
                        placeSafeZone(g,x,y);

                        placeBomb(g);

                        placeBombAround(g);

                        discoverCase(g, x, y, &reveledCase);

                        refreshScreen(g, image, texture);

                    }
                    else if (g->t[getCoord1D(g,x,y)].isRevealed == 0 && g->t[getCoord1D(g, x, y)].isFlag == 0)
                    {
                        discoverCase(g, x, y, &reveledCase);

                        refreshScreen(g, image, texture);

                        if (g->t[getCoord1D(g, x, y)].isBomb == 1)
                        {
                            printf("Tu as perdu\n");
                            SDL_Delay(2000);
                            GameIsRunning = SDL_FALSE;
                        }
                        if (reveledCase == g->bombs)
                        {
                            printf("Tu as gagne\n");
                            SDL_Delay(2000);
                            GameIsRunning = SDL_FALSE;
                        }
                    }


                    continue;
                }

                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    int x = ((event.button.x - (event.button.x % 40)) / 40);
                    int y = ((event.button.y - (event.button.y % 40)) / 40);

                    int TileCoord = getCoord1D(g, x, y);

                    if (reveledCase != 0)
                    {
                        if (g->t[TileCoord].isRevealed == 0 && g->t[TileCoord].isFlag == 0)
                        {
                            g->t[TileCoord].isFlag = 1;
                        }
                        else if (g->t[TileCoord].isFlag == 1)
                        {
                            g->t[TileCoord].isFlag = 0;

                        }
                    }

                    refreshScreen(g, image, texture);

                    continue;
                }

            case(SDL_QUIT):
                GameIsRunning = SDL_FALSE;
                break;

            default:
                continue;
            }
        }
    }
}





int main(int argc, char* argv[])
{
    // initialisation de la SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_ExitWithError("ERROR Initialisation SDL");

    /*---------------------------------- - Programme------------------------------------*/
    SDL_Surface* image = NULL;
    Grid grid;
    grid.onoff = 1;

    while (grid.onoff == 1)
    {
        SDL_Event event;

        SDL_Window* difficulty_window = NULL;
        SDL_Renderer* difficulty_renderer = NULL;

        int diffChoice = setDifficulty(difficulty_window, difficulty_renderer, &grid);

        SDL_Texture* texture = NULL;

        if (diffChoice != 3)
        {
            Difficulty difficulties[3];

            difficulties[0].size = 10;
            difficulties[0].bomb = 10;

            difficulties[1].size = 18;
            difficulties[1].bomb = 40;

            difficulties[2].size = 24;
            difficulties[2].bomb = 99;

            

            grid.caseSize = 40;

            gameLoop(&grid, diffChoice, difficulties, image, texture);

            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(grid.renderer);
            SDL_DestroyWindow(grid.window);
            if (replay() == 0)
            {
                printf("A bientot\n");
                grid.onoff = 0;
            }
            else
            {
                printf("C'est reparti\n");
            }
            free(grid.t);

        }

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_Quit)
            {
                grid.onoff = 0;
            }
        }
    }

    /*---------------------------------- - Programme------------------------------------*/
   
    
    SDL_Quit();

    return 0;
}
