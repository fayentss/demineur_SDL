#include <SDL.h>
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

} Tile;

typedef struct Grid
{
    Tile* t;
    int size;
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
                    SDL_ExitWithError("Error sur la case %d\n", i);
            }
            else if (i % 2 == 1)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case %d\n", i);
            }
        }
        else if (((i - (i % g->size)) / g->size) % 2 == 1)
        {
            if (i % 2 == 0)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 78, 78, 78, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case %d\n", i);
            }
            else if (i % 2 == 1)
            {
                if (SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE) != 0)
                    SDL_ExitWithError("Error sur la case %d\n", i);
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


int setDifficulty(SDL_Window* window, SDL_Renderer* renderer)
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

void gameLoop(Grid* g, int diffChoice, Difficulty* difficulty)
{
    g->size = difficulty[diffChoice].size;
    g->total_size = g->size * g->size;
    g->t = malloc(sizeof(Tile) * g->total_size);
    g->bombs = difficulty[diffChoice].bomb;
    g->window = g->window;
    g->renderer = g->renderer;

    createGrid(g);

    SDL_bool GameIsRunning = SDL_TRUE;

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

                    int TileCoord = getCoord1D(g, x, y);

                    printf("la case est la numero : %d\n", TileCoord);

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

    // Déclaration
    SDL_Window* difficulty_window = NULL;
    SDL_Renderer* difficulty_renderer = NULL;

    SDL_Window* game_window = NULL;
    SDL_Renderer* game_renderer = NULL;


    // initialisation de la SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        SDL_ExitWithError("ERROR Initialisation SDL");

    /*---------------------------------- - Programme------------------------------------*/

    int diffChoice = setDifficulty(difficulty_window, difficulty_renderer);

    if (diffChoice != 3)
    {
        Difficulty difficulties[3];

        difficulties[0].size = 10;
        difficulties[0].bomb = 10;

        difficulties[1].size = 18;
        difficulties[1].bomb = 40;

        difficulties[2].size = 24;
        difficulties[2].bomb = 99;

        Grid grid;

        gameLoop(&grid, diffChoice, &difficulties);

        SDL_Texture* texture = NULL;

        IMG_Init(IMG_INIT_PNG);
        texture = IMG_LoadTexture(game_renderer, "./image.png");

        SDL_RenderCopy(game_renderer, texture, NULL, NULL);
        SDL_RenderPresent(game_renderer);

    }

    /*---------------------------------- - Programme------------------------------------*/
    
    SDL_DestroyRenderer(game_renderer);
    SDL_DestroyWindow(game_window);
    SDL_Quit();

    return 0;
}
