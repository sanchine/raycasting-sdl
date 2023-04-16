#include <SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <math.h>
#include <vector>

#include "Map.h"
//#include "Guns.h"
//#include "Player.h"

using namespace std;

// RenderWindow

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

bool q = false;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

// View config

float view_fov = 3.14159 / 3;
float view_depth = 30;

// Crosshair

int crosshair_gap = 10;
int crosshair_size = 7;

 // Player

float player_posX = 2;
float player_posY = 1;
float player_angle = 0;
float player_speed = 5;

bool player_isFiring = false;
bool player_isTargetingAtEnemy = false;

// Mobs

SDL_Rect mobs_dstRect = { 10, 10, 12, 12 };


 // Animation Frame

int animation_frame = 0;

  // Guns

SDL_Rect gun_dstRect = { 1920 / 2 - 80 , 720 + 40, 500, 400 };
SDL_Surface* gun_Image;
SDL_Texture* gun_Texture;

SDL_Surface* gunFiring_Image;
SDL_Texture* gunFiring_Texture;

// Set up SDL

bool init() {
    bool ok = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
    }

    win = SDL_CreateWindow("Howis?", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        cout << "Can't create window: " << SDL_GetError() << endl;
        ok = false;
    }

    ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == NULL) {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        ok = false;
    }

    return ok;
}

void quit();

// Procedures

void gun_draw(const char* filePath) {
    SDL_SetRenderDrawColor(ren, 0, 255, 255, 100);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - crosshair_gap - crosshair_size, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - crosshair_gap);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + crosshair_gap + crosshair_size, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + crosshair_gap);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2 - crosshair_gap - crosshair_size, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - crosshair_gap, SCREEN_HEIGHT / 2);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2 + crosshair_gap + crosshair_size, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 + crosshair_gap, SCREEN_HEIGHT / 2);

    if (animation_frame == 0) gun_dstRect = { 1920 / 2 - 260, 720 + 40, 800, 600 };
    else if (animation_frame == 5) gun_dstRect = { 1920 / 2 - 240 , 720 + 40, 800, 600 };
    else if (animation_frame == 10) gun_dstRect = { 1920 / 2 - 200 , 720 + 40, 800, 600 };
    else if (animation_frame == 15) {
        gun_dstRect = { 1920 / 2 - 220 , 720 + 40, 800, 600 };
        animation_frame = 0;
    }

    gun_Image = SDL_LoadBMP(filePath);
    SDL_SetColorKey(gun_Image, SDL_TRUE, SDL_MapRGB(gun_Image->format, 0xFF, 0xFF, 0xFF));
    gun_Texture = SDL_CreateTextureFromSurface(ren, gun_Image);

    SDL_RenderCopy(ren, gun_Texture, NULL, &gun_dstRect);
}

void gunFiring_draw(const char* filePath) {
    SDL_SetRenderDrawColor(ren, 0, 255, 255, 100);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - crosshair_gap - 6 - crosshair_size, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - crosshair_gap - 6);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + crosshair_gap + crosshair_size + 6, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + crosshair_gap + 6);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2 - crosshair_gap - crosshair_size - 6, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 - crosshair_gap - 6, SCREEN_HEIGHT / 2);
    SDL_RenderDrawLine(ren, SCREEN_WIDTH / 2 + crosshair_gap + crosshair_size + 6, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2 + crosshair_gap + 6, SCREEN_HEIGHT / 2);

    if (animation_frame == 0) gun_dstRect = { 1920 / 2 - 260, 720 + 40, 800, 600 };
    else if (animation_frame == 5) gun_dstRect = { 1920 / 2 - 240 , 720 + 40, 800, 600 };
    else if (animation_frame == 10) gun_dstRect = { 1920 / 2 - 200 , 720 + 40, 800, 600 };
    else if (animation_frame == 15) {
        gun_dstRect = { 1920 / 2 - 220 , 720 + 40, 800, 600 };
        animation_frame = 0;
    }

    gunFiring_Image = SDL_LoadBMP(filePath);
    SDL_SetColorKey(gunFiring_Image, SDL_TRUE, SDL_MapRGB(gunFiring_Image->format, 0xFF, 0xFF, 0xFF));
    gunFiring_Texture = SDL_CreateTextureFromSurface(ren, gunFiring_Image);

    SDL_RenderCopy(ren, gunFiring_Texture, NULL, &gun_dstRect);

    player_isFiring = false;
        
}

// Main Function

int main(int arhc, char** argv) {

    // Initialize

    if (!init()) {
        quit();
        system("pause");
        return 1;
    }

    // Helpers

    SDL_Event e;
    auto tp1 = chrono::system_clock::now();
    auto tp2 = chrono::system_clock::now();

    // Creating a map

    map_init();

    // Game Loop

    while (!q) {

        // ElaspedTime

        tp2 = chrono::system_clock::now();
        chrono::duration <float> elapsedTime = tp2 - tp1;
        tp1 = tp2;
        float elapsedTime_value = elapsedTime.count();

        // Mouse State First Job

        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        // Controller

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    q = true;
                }
                if (e.key.keysym.sym == SDLK_a) {
                    player_angle -= (player_speed * 0.75) * elapsedTime_value;
                }
                if (e.key.keysym.sym == SDLK_d) {
                    player_angle += (player_speed * 0.75) * elapsedTime_value;
                }
                if (e.key.keysym.sym == SDLK_w) {
                    player_posX += sinf(player_angle) * player_speed * elapsedTime_value;
                    player_posY += cosf(player_angle) * player_speed * elapsedTime_value;

                    if (logicMap[(int)player_posX][(int)player_posY] == '#') {
                        player_posX -= sinf(player_angle) * player_speed * elapsedTime_value;
                        player_posY -= cosf(player_angle) * player_speed * elapsedTime_value;
                    }

                    animation_frame++;

                }
                if (e.key.keysym.sym == SDLK_s) {
                    player_posX -= sinf(player_angle) * player_speed * elapsedTime_value;
                    player_posY -= cosf(player_angle) * player_speed * elapsedTime_value;

                    if (logicMap[(int)player_posX][(int)player_posY] == '#') {
                        player_posX += sinf(player_angle) * player_speed * elapsedTime_value;
                        player_posY += cosf(player_angle) * player_speed * elapsedTime_value;
                    }

                    animation_frame++;
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                animation_frame++;
                player_isFiring = true;
            }
            if (e.type == SDL_MOUSEBUTTONUP) {
                animation_frame++;
            }
        }

        // Mouse State Second Job

        int mouse_x2, mouse_y2;
        float mouse_speed = 5;
        SDL_GetMouseState(&mouse_x2, &mouse_y2);

        if (mouse_x2 - mouse_x > 0 || mouse_x2 >= SCREEN_WIDTH - 1) {
            player_angle += (mouse_speed * 0.75) * elapsedTime_value;
        }
        else if (mouse_x2 - mouse_x < 0 || mouse_x2 <= 0) {
            player_angle -= (mouse_speed * 0.75) * elapsedTime_value;
        }

        // drawing lines like 3d ****

        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderClear(ren);

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            float rayAngle = (player_angle - view_fov / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * view_fov;

            float distanceToObject = 0;
            bool wall_wasTouched = false;
            bool mobs_wasTouched = false;

            float unitVector_x = sinf(rayAngle);
            float unitVector_y = cosf(rayAngle);

            while (!wall_wasTouched && distanceToObject < view_depth) {
                distanceToObject += 0.1;

                int targetedX = (int)(player_posX + unitVector_x * distanceToObject);
                int targetedY = (int)(player_posY + unitVector_y * distanceToObject);

                if (targetedX < 0 || targetedX >= map_width || targetedY < 0 || targetedY >= map_height) {
                    wall_wasTouched = true;
                    distanceToObject = view_depth;
                }
                else if (logicMap[targetedX][targetedY] == '#') {
                    wall_wasTouched = true;
                }
                else if (logicMap[targetedX][targetedY] == 'e') {
                    wall_wasTouched = true;
                    mobs_wasTouched = true;
                }
            }

            int ceilY = (float)(SCREEN_HEIGHT / 2) - SCREEN_HEIGHT / ((float)distanceToObject);
            int floorY = SCREEN_HEIGHT - ceilY;

            // drawing a sky

            SDL_SetRenderDrawColor(ren, 0, 170, 170, 100);
            SDL_RenderDrawLine(ren, x, 0, x, ceilY);

            // drawing objects

            if (distanceToObject <= view_depth / 3) SDL_SetRenderDrawColor(ren, 0, 0, 140, 255);
            else if (distanceToObject <= view_depth / 2) SDL_SetRenderDrawColor(ren, 0, 0, 50, 140);
            else if (distanceToObject <= view_depth / 1.5) SDL_SetRenderDrawColor(ren, 0, 0, 150, 200);
            else if (distanceToObject <= view_depth) SDL_SetRenderDrawColor(ren, 0, 0, 100, 100);

            SDL_RenderDrawLine(ren, x, ceilY, x, floorY);
            

            // drawing a floor

            SDL_SetRenderDrawColor(ren, 153, 102, 0, 100);
            SDL_RenderDrawLine(ren, x, floorY - 50, x, SCREEN_HEIGHT);
        }
        

        // drawing gun

        if (player_isFiring) gunFiring_draw("Images/gun_firing.bmp");
        else gun_draw("Images/gun.bmp");

        // drawing map

        map_draw(ren, player_posX, player_posY);

        // drawing enemies


        SDL_RenderPresent(ren);

        SDL_Delay(15);
    }

    quit();
    return 0;
}

// SDL

void quit() {
    SDL_DestroyWindow(win);
    win = NULL;

    SDL_DestroyRenderer(ren);
    ren = NULL;

    SDL_DestroyTexture(gun_Texture);

    delete[] map;
    map = nullptr;

    SDL_Quit();
}