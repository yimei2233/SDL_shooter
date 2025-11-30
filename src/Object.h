#ifndef OBJECT_H
#define OBJECT_H
#include<SDL.h>

struct Player
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = 200;
    // 子弹的发射时间冷却；子弹在进行发射时要注意两个子弹之间的间隔
    Uint32 coolDown = 500;
    Uint32 lastShootTime = 0;
};
// 子弹（投掷物）对象
struct ProjectilePlayer
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = 400;
};
// 敌机对象
struct Enemy
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = 400;
    // 子弹的发射时间冷却；子弹在进行发射时要注意两个子弹之间的间隔
    Uint32 coolDown = 1000;
    Uint32 lastShootTime = 0;
};
// 敌机的子弹
struct ProjectileEnemy
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    // 子弹的方向，这里想设计为追踪玩家
    SDL_FPoint direction = {0,0};
    int width = 0;
    int height = 0;
    int speed = 400;
};
#endif