#ifndef  SCENE_MAIN_H
#define  SCENE_MAIN_H

// 这里是基类的派生类，所以需要包含基类的头文件
#include "Scene.h"
#include "Object.h"
#include<SDL.h>
#include<list>
#include<map>
#include<random>
#include<SDL_mixer.h>

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
    // 敌人死亡
    void enemyExploade(Enemy* enemy);
    // 玩家状态更新
    void updatePlayer(float deltaTime);
    // 处理动画队列
    void updateExplosion(float deltaTime);
    // 渲染动画序列
    void renderExplosions();
    // 创建掉落物品，在敌人死后调用
    void dropItem(Enemy* enemy);
    // 更新物品状态
    void updateItem(float deltaTime);
    // 玩家获取物品
    void playerGetItem(Item* item);
    // 渲染物品
    void renderItems();
private:
    Game &game;
    Player player;
    // 1.使用list来管理玩家子弹对象
    // 这里使用指针为了减少栈上的开销
    std::list<ProjectilePlayer*>ProjectilePlayers;
    // 初始化子弹模板
    ProjectilePlayer ProjectilePlayerTemplate;
    // 2.随机数  通过 dis(gen) 来生成 
    std::mt19937 gen;
    std::uniform_real_distribution<float>dis;
    // 3.敌机模板
    Enemy enemyTemplate;
    std::list<Enemy*>enemies;
    // 4.敌机子弹
    ProjectileEnemy ProjectileEnemyTemplate;
    std::list<ProjectileEnemy*>ProjectileEnemys;
    // 5.玩家状态
    bool is_dead = false;
    // 6.序列帧动画 在死亡的物体上创建动画，待创建的动画都保存在explosions中
    Explosion explosionTemplate;
    std::list<Explosion*>explosions;
    // 7.道具模板
    Item itemLifeTemplate;
    std::list<Item*>items;
    // 8.音乐资源
    Mix_Music* bgm;
    // 9.音效库
    std::map<std::string,Mix_Chunk*>sounds;
};


#endif