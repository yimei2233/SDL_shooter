#ifndef  SCENE_MAIN_H
#define  SCENE_MAIN_H

// 这里是基类的派生类，所以需要包含基类的头文件
#include "Scene.h"
#include "Object.h"
#include<SDL.h>
#include<list>
#include<random>

class SceneMain : public Scene
{
public:
    SceneMain();
    ~SceneMain();

    // 重写父类的纯虚函数
    // override 关键字用于明确表示这是对基类方法的重写
    void init() override;
    void render() override;
    void handleEvents(SDL_Event* event) override;
    void update(float deltaTime) override;
    void clean() override;
    // 键盘控制
    void keyboardControl(float deltaTime);
    void shootPlayer();
    // 玩家子弹更新,注意涉及到帧的更新时都要使用 deltaTime 来控制速度
    void updatePlayerProjectile(float deltaTime);
    // 渲染玩家子弹函数
    void renderPlayerProjectiles();
    // 敌人生成
    void spawEnemy();
    // 敌人位置更新
    void updateEnemies(float deltaTime);
    // 渲染敌人
    void renderEnemies();
    // 敌人射击（这里需要传入敌人对象）
    void shootEnemy(Enemy *enemy);
    // 敌人子弹方向
    SDL_FPoint getDirection(Enemy *enemy);
    // 更新子弹状态
    void updateEnemyProjectile(float deltaTime);
    // 渲染敌人子弹函数
    void renderEnemyProjectile();
private:
    Game &game;
    Player player;
    // 1.使用list来管理玩家子弹对象
    // 这里使用指针为了减少栈上的开销
    std::list<ProjectilePlayer*>ProjectilePlayers;
    // 初始化子弹模板
    ProjectilePlayer ProjectilePlayerTemplate;
    // 2.随机数
    std::mt19937 gen;
    std::uniform_real_distribution<float>dis;
    // 3.敌机模板
    Enemy enemyTemplate;
    std::list<Enemy*>enemies;
    // 4.敌机子弹
    ProjectileEnemy ProjectileEnemyTemplate;
    std::list<ProjectileEnemy*>ProjectileEnemys;
};


#endif