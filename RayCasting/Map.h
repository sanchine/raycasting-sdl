#pragma once
#include <fstream>
#include <SDL.h>

int map_width = 16;
int map_height = 16;
int map_block_size = 10;
int mobs_block_size = 6;
const char* filePath = "Maps/map1.txt";

char** logicMap = new char*[map_width];
SDL_Rect** map = new SDL_Rect * [map_width];

void map_init() {

    for (int i = 0; i < map_width; i++) {
        map[i] = new SDL_Rect[map_height];
        logicMap[i] = new char[map_height];
    }
}

void map_draw(SDL_Renderer* ren,  float playerX,  float playerY) {
    std::ifstream file(filePath);
    playerX += 0.1;
    playerY += 0.1;
    SDL_Rect mapBackground = { 0, 0, map_width * map_block_size, map_height * map_block_size };
    SDL_Rect playerAtMap = { playerX * map_block_size - 2, playerY * map_block_size - 2, 4, 4};
    SDL_Rect enemyAtMap = { 5 * map_width, 5 * map_height, mobs_block_size, mobs_block_size };

    SDL_SetRenderDrawColor(ren, 240, 130, 140, 150);
    SDL_RenderFillRect(ren, &mapBackground);

    for (int i = 0, offsetX = 0; i < map_width; i++, offsetX += map_block_size) {
        for (int j = 0, offsetY = 0; j < map_width; j++, offsetY += map_block_size) {

            map[i][j] = { offsetX, offsetY, map_block_size , map_block_size };
            file >> logicMap[i][j];

            if (logicMap[i][j] == '#') {
                SDL_SetRenderDrawColor(ren, 218, 165, 32, 150);

            }
            else if (logicMap[i][j] == '*') {
                SDL_SetRenderDrawColor(ren, 176, 196, 222, 150);
            }

            SDL_RenderFillRect(ren, &map[i][j]);

        }
    }

    SDL_SetRenderDrawColor(ren, 240, 0, 0, 150);
    SDL_RenderFillRect(ren, &playerAtMap);

    SDL_SetRenderDrawColor(ren, 240, 0, 255, 150);
    SDL_RenderFillRect(ren, &enemyAtMap);

}