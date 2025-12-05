#ifndef OBJECT_H
#define OBJECT_H
#include<SDL.h>

enum class ItemType{
    Health,
    Shield,
    Time
};

struct Player
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int highSpeed = 400;
    int normalSpeed = 200;
    int currentHealth = 3;
    int maxHealth = 5;
    // 子弹的发射时间冷却；子弹在进行发射时要注意两个子弹之间的间隔
    Uint32 coolDown = 200;
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
    // 伤害
    int damage = 1;
};
// 敌机对象
struct Enemy
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = 200;
    // 生命值
    int currentHealth = 2;

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
    int speed = 200;
    // 伤害
    int damage = 1;
};

// 爆炸动画序列帧
struct Explosion
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;
    int totalFrame = 0;
    Uint32 startTime = 0;
    Uint32 FPS = 10;
};
// 掉落物品
struct Item
{
    SDL_Texture* texture = nullptr;
    SDL_FPoint position = {0,0};
    SDL_FPoint direction = {0,0};
    int width = 0;
    int height = 0;
    int speed = 200;
    int bounce = 3;
    ItemType type = ItemType::Health;
};
#endif