#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>
#include"Game.h"
int main(int,char**){
    // 使用单例模式,注意获取实例的方式
    Game& game = Game::getInstance();
    game.init();
    game.run();
    return 0;
}