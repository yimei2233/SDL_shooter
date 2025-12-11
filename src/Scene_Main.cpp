#include "Game.h"
#include "Scene_Main.h"
#include "Scene.h"
#include "Object.h"
#include "SceneTitle.h"
#include <SDL_image.h>
#include <iostream>
// c++ 随机数头文件
#include <random>

SceneMain::~SceneMain()
{
    
}

void SceneMain::init()
{
    // 先载入bgm
    bgm = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    if(bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Fail to load music: %s",Mix_GetError());
    }
    // 播放音乐，-1 为一直循环
    Mix_PlayMusic(bgm,-1);

    // 读取音效资源
    sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["player_explode"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemy_explode"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");
    sounds["get_item"] = Mix_LoadWAV("assets/sound/eff5.wav");

    // 载入字体
    // 字体尽可能选择倍数
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf",24);

    // 1.玩家初始化
    player.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/SpaceShip.png");
    // 获取纹理的宽和高
    SDL_QueryTexture(player.texture,nullptr,nullptr,&player.width,&player.height);
    player.width /= 4;
    player.height /= 4;
    // 初始化位置
    player.position.x = (game.getWindowWidth() - player.width) / 2;
    player.position.y = game.getWindowHeight() -player.height;
    // 2.初始化子弹模板（材质）
    ProjectilePlayerTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/laser-1.png");
    SDL_QueryTexture(ProjectilePlayerTemplate.texture,nullptr,nullptr,&ProjectilePlayerTemplate.width,&ProjectilePlayerTemplate.height);
    ProjectilePlayerTemplate.width /= 4;
    ProjectilePlayerTemplate.height /= 4;
    // 3.初始化随机数引擎
    std::random_device rd;
    gen = std::mt19937(rd());
    // 4.均匀分布
    dis = std::uniform_real_distribution<float>(0.0f,1.0f);
    // 5.初始化敌机模板
    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/insect-1.png");
    SDL_QueryTexture(enemyTemplate.texture,nullptr,nullptr,&enemyTemplate.width,&enemyTemplate.height);
    enemyTemplate.width /= 4;
    enemyTemplate.height /= 4;
    // 6.初始化敌机子弹模板
    ProjectileEnemyTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/bullet-1.png");
    SDL_QueryTexture(ProjectileEnemyTemplate.texture,nullptr,nullptr,&ProjectileEnemyTemplate.width,&ProjectileEnemyTemplate.height);
    ProjectileEnemyTemplate.width   /= 4;
    ProjectileEnemyTemplate.height  /= 4;
    // 7.初始化序列帧动画
    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture,nullptr,nullptr,&explosionTemplate.width,&explosionTemplate.height);
    explosionTemplate.totalFrame =  explosionTemplate.width / explosionTemplate.height;
    // explosionTemplate.height /= 4;
    explosionTemplate.width = explosionTemplate.height;
    // 8.初始化道具模板
    itemLifeTemplate.texture = IMG_LoadTexture(game.getRenderer(),"assets/image/bonus_life.png");
    SDL_QueryTexture(itemLifeTemplate.texture,nullptr,nullptr,&itemLifeTemplate.width,&itemLifeTemplate.height);
    itemLifeTemplate.width   /= 4;
    itemLifeTemplate.height  /= 4;
    // 9.初始化uiHealth
    uiHealth = IMG_LoadTexture(game.getRenderer(),"assets/image/Health UI Black.png");
}

void SceneMain::render()
{
    // 先渲染子弹
    renderPlayerProjectiles();
    // 渲染玩家
    if(!is_dead)
    {
        SDL_Rect playerRect = { static_cast<int>(player.position.x),
                        static_cast<int>(player.position.y),
                        player.width,
                        player.height };
        SDL_RenderCopy(game.getRenderer(),player.texture,NULL,&playerRect);
    }

    // 渲染敌人
    renderEnemies();
    // 渲染敌人子弹
    renderEnemyProjectile();
    // 渲染掉落物品
    renderItems();
    // 渲染爆炸特效
    renderExplosions();
    // 渲染UI
    renderUI();
}

void SceneMain::handleEvents(SDL_Event* event)
{
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)
        {
            auto sceneMain = new SceneTitle;
            // change 函数负责调用初始化，这里不用初始化
            game.changeScene(sceneMain);

        }
    }
}
// deltaTime 来自于 游戏主循环run
void SceneMain::update(float deltaTime)
{
    // 键盘检测，包含对玩家控制的逻辑
    keyboardControl(deltaTime);
    // 发射出去的子弹更新
    updatePlayerProjectile(deltaTime);
    // 生成敌人
    spawEnemy();
    // 更新敌人
    updateEnemies(deltaTime);
    // 更新敌人子弹状态
    updateEnemyProjectile(deltaTime);
    // 更新玩家状态
    updatePlayer(deltaTime);
    // 处理动画
    updateExplosion(deltaTime);
    // 更新掉落物品状态
    updateItem(deltaTime);
}

void SceneMain::clean()
{
    // 清理容器
    /*
        注意：这里的类型是指针引用，即&*
        因为是指针引用，所以在delete时，能够将list中原本的指针指向的空间释放，并让该指针为nullptr
        如果不是指针引用而是值引用，相当于将指针拷贝了一份，也可以将指针对应的空间释放，但是list中的指针仍指向已经释放后的内存(悬空指针，不安全)
    */
    for(auto &projectile : ProjectilePlayers){
        if(projectile != nullptr)
        {
            delete projectile;
        }
    }
    ProjectilePlayers.clear();
    for(auto &enemy : enemies){
        if(enemy != nullptr){
            delete enemy;
        }
    }
    enemies.clear();
    for(auto &ProjectileEnemy : ProjectileEnemys){
        if(ProjectileEnemy != nullptr){
            delete ProjectileEnemy;
        }
    }
    ProjectileEnemys.clear();
    for(auto explosion : explosions)
    {
        if(explosion != nullptr){
            delete explosion;
        }
    }
    explosions.clear();
    for(auto item : items)
    {
        if(item != nullptr){
            delete item;
        }
    }
    items.clear();
    for(auto sound : sounds)
    {
        if(sound.second != nullptr)
        {
            delete(sound.second);
        }
    }
    sounds.clear();
    // 清理模板
    if(player.texture != nullptr)
    {
        SDL_DestroyTexture(player.texture);
    }
    if(ProjectilePlayerTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(ProjectilePlayerTemplate.texture);
    }
    if(enemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(ProjectilePlayerTemplate.texture);
    }
    if(ProjectileEnemyTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(ProjectileEnemyTemplate.texture);
    }
    if(explosionTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(explosionTemplate.texture);
    }
    if(itemLifeTemplate.texture != nullptr)
    {
        SDL_DestroyTexture(itemLifeTemplate.texture);
    }
    // 清理音乐资源
    if(bgm != nullptr)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
    if(uiHealth != nullptr)
    {
        SDL_DestroyTexture(uiHealth);
    }
    // 清理字体
    if(scoreFont != nullptr)
    {
        TTF_CloseFont(scoreFont);
    }
}

void SceneMain::keyboardControl(float deltaTime)
{
    // (is_dead)若玩家死亡，则不需要进行控制
    if(is_dead) return ;
    // 通过SDL_GetKeyboardState 返回键盘数组指针来判断是否按下
    auto keyboardState = SDL_GetKeyboardState(NULL);
    if(keyboardState[SDL_SCANCODE_W]){
        player.position.y -= player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_S]){
        player.position.y += player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_A]){
        player.position.x -= player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_D]){
        player.position.x += player.speed * deltaTime;
    }
    if(keyboardState[SDL_SCANCODE_SPACE])
    {
        player.speed = player.highSpeed;
    }else{
        player.speed = player.normalSpeed;
    }
    // 限制飞机的移动范围
    if(player.position.x < 0){
        player.position.x = 0;
    }
    if(player.position.x > (game.getWindowWidth() - player.width)){
        player.position.x = game.getWindowWidth() - player.width;
    }
    if(player.position.y < 0){
        player.position.y = 0;
    }
    if(player.position.y > game.getWindowHeight() - player.height){
        player.position.y = game.getWindowHeight() - player.height;
    }

    // 控制子弹发射
    if(keyboardState[SDL_SCANCODE_J]){
        // SDL_GetTicks 函数会获取自init以后的时间
        auto currentTime = SDL_GetTicks();
        if((currentTime - player.lastShootTime) > player.coolDown){
            shootPlayer();
            player.lastShootTime = SDL_GetTicks();
        }
    }
}

void SceneMain::shootPlayer()
{
    // 创建子弹
    // 如果在创建子弹时进行材质的初始化会非常的耗时，因为每一个对象都要进行一次初始化
    // 因此这里可以创建一个子弹模板，在创建新的子弹时可以通过子弹模板进行拷贝构造
    auto projectile = new ProjectilePlayer(ProjectilePlayerTemplate);
    projectile->position.y = player.position.y;
    projectile->position.x = player.position.x + player.width / 2 - projectile-> width/2;
    // 注意这里是指针，因为 new 出来的已经是指针了
    ProjectilePlayers.push_back(projectile);
    Mix_PlayChannel(0,sounds["player_shoot"],0);

}
// 更新子弹的移动
void SceneMain::updatePlayerProjectile(float deltaTime)
{
    // 设定子弹超出边界以后消失的距离
    int margin = 32;
    for(auto it = ProjectilePlayers.begin();it != ProjectilePlayers.end(); ){
        // 检测子弹是否命中
        // 注意：子弹的检测要在循环内部！！！！
        // 是每一个子弹都需要进行检测
        bool hit = false;
        auto projectile = *it;
        projectile->position.y -= deltaTime * projectile->speed;
        // 判断子弹是否超出屏幕
        if(projectile->position.y + margin < 0)
        {
            delete projectile;
            // 通过 迭代器 来清理在list中超出屏幕边界的子弹
            // 注意这里it 返回的是删除后的下一个指针，因此就不需要++it
            it = ProjectilePlayers.erase(it);
            // 输出子弹
            // static int i = 0;
            // std::cout << "id:" << ++i << " 子弹被清除" << std::endl;
        }else{
            // 这里就要进行子弹的碰撞检测，检测子弹是否碰撞了敌人
            // 检测函数的原理是检测两个矩形区域是否发生了重叠，因此这里要将子弹所在的区域和所有敌人区域进行对比
            // 检测是否发生了碰撞
            for(auto enemy : enemies)
            {
                SDL_Rect projectileRect = {
                    static_cast<int>(projectile->position.x),
                    static_cast<int>(projectile->position.y),
                    projectile->width,
                    projectile->height
                };
                SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
                if(SDL_HasIntersection(&enemyRect,&projectileRect))
                {
                    // 子弹命中,减少敌人的生命值，并清除子弹
                    enemy->currentHealth -= projectile->damage;
                    hit = true;
                    delete projectile;
                    it = ProjectilePlayers.erase(it);
                    Mix_PlayChannel(0,sounds["hit"],0);
                    break;
                }
            }
            // 不碰撞再自增
            if(!hit) ++it;
        }
    }
}

void SceneMain::renderPlayerProjectiles()
{
    for(auto projectile : ProjectilePlayers){
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height
        };
        SDL_RenderCopy(game.getRenderer(),projectile->texture,NULL, &projectileRect);
    }
    
}

// 敌人的生成是通过std::list链表联系起来的(子弹也是类似的)，其中，第一个函数创建敌人
// 另外两个函数负责检查list
// 1）如果list中有成员，就负责更新其状态
// 2）如果list中有成员，就负责根据其状态来进行渲染
void SceneMain::spawEnemy()
{
    // 随机概率来生成敌机
    // 一秒钟生成一个
    if(dis(gen) > 1 / 60.0f){
        return;
    }
    // std::cout << "生成敌机" << std::endl;
    Enemy* enemy = new Enemy(enemyTemplate);
    enemy->position.x = dis(gen) * (game.getWindowWidth() - enemy->width);
    enemy->position.y = (-1)*enemy->height;
    enemies.push_back(enemy);
 }

 void SceneMain::updateEnemies(float deltaTime)
 {
    auto currentTime = SDL_GetTicks();
    for(auto it = enemies.begin();it != enemies.end(); ){
        // 注意it是迭代器，解引用变为指针
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        if(enemy->position.y > game.getWindowHeight())
        {
            delete enemy;
            it = enemies.erase(it);
        }else{
            // 发射子弹
            if((currentTime - enemy->lastShootTime > enemy->coolDown) && (!is_dead)){
                shootEnemy(enemy);
                enemy->lastShootTime = currentTime;
            }
            // 检测敌机的生命值
            if(enemy->currentHealth <= 0)
            {
                // 删除敌机的操作在函数中实现
                // 这里要传enemy指针，因此不能提前删除指针
                enemyExploade(enemy);
                it = enemies.erase(it);
            }else{
                ++it;
            }
        }
    }
 }

 void SceneMain::renderEnemies()
 {
        for(auto enemy : enemies){
        SDL_Rect enemyRect = {
            static_cast<int>(enemy->position.x),
            static_cast<int>(enemy->position.y),
            enemy->width,
            enemy->height
        };
        SDL_RenderCopy(game.getRenderer(),enemy->texture,NULL, &enemyRect);
    }
 }

 void SceneMain::shootEnemy(Enemy *enemy)
 {
    auto projectile = new ProjectileEnemy(ProjectileEnemyTemplate);
    projectile->position.x = enemy->position.x + enemy->width / 2 - projectile->width/2;
    projectile->position.y = enemy->position.y + enemy->height / 2 - projectile->height/2;
    projectile->direction = getDirection(enemy);
    // std::cout << "子弹朝向" << projectile->direction.x << projectile->direction.y << std::endl;
    ProjectileEnemys.push_back(projectile);
    Mix_PlayChannel(-1,sounds["enemy_shoot"],0);
 }
SDL_FPoint SceneMain::getDirection(Enemy *enemy)
{
    auto x = (player.position.x + player.width / 2) - (enemy->position.x + enemy->width/2);
    auto y = (player.position.y + player.height / 2) - (enemy->position.y + enemy->height/2);
    auto length = sqrt(x*x + y*y);
    x /= length;
    y /= length;
    return SDL_FPoint{x,y};
}
// 敌人子弹更新时检测玩家血量
// 同理玩家子弹检测是否碰撞敌人
void SceneMain::updateEnemyProjectile(float deltaTime)
{
    auto margin = 32;
    for(auto it = ProjectileEnemys.begin();it != ProjectileEnemys.end();)
    {
        // projectile 也是指针
        auto projectile = *it;
        projectile->position.y += projectile->speed * deltaTime * projectile->direction.y;
        projectile->position.x += projectile->speed * deltaTime * projectile->direction.x;
        // 是否超出屏幕的判断
        if(projectile->position.x < -margin ||
            projectile->position.x > game.getWindowWidth() + margin ||
            projectile->position.y < -margin ||
            projectile->position.y > game.getWindowHeight() + margin
            )
            {
                delete projectile;
                it = ProjectileEnemys.erase(it);
            }else{
                SDL_Rect projectileRect = {
                    static_cast<int>(projectile->position.x),
                    static_cast<int>(projectile->position.y),
                    projectile->width,
                    projectile->height
                };
                SDL_Rect playerRect = {
                    static_cast<int>(player.position.x),
                    static_cast<int>(player.position.y),
                    player.width,
                    player.height
                };
                // 玩家存活时才进行碰撞检测
                if(SDL_HasIntersection(&playerRect,&projectileRect) && (!is_dead))
                {
                    // 子弹命中,减少敌人的生命值，并清除子弹
                    player.currentHealth -= projectile->damage;
                    delete projectile;
                    it = ProjectileEnemys.erase(it);
                    // break;
                    Mix_PlayChannel(0,sounds["hit"],0);
                }else{
                    ++it;
                }
            }

    }
}

void SceneMain::renderEnemyProjectile()
{
    for(auto projectile : ProjectileEnemys){
        SDL_Rect projectileRect = {
            static_cast<int>(projectile->position.x),
            static_cast<int>(projectile->position.y),
            projectile->width,
            projectile->height
        };
        // 这里为了保证子弹旋转，要使用新的渲染函数ex
        // SDL_RenderCopy(game.getRenderer(),projectile->texture,NULL, &projectileRect);
        // 先计算角度 , 这里使用了atan2函数，计算反正切，结果为弧度制下的角度
        float angle = atan2(projectile->direction.y,projectile->direction.x) * 180 / M_PI - 90;
        SDL_RenderCopyEx(game.getRenderer(),projectile->texture,NULL, &projectileRect,angle,NULL,SDL_FLIP_NONE);
    }
}

void SceneMain::enemyExploade(Enemy *enemy)
{
    auto currentTime = SDL_GetTicks();
    // 1.添加爆炸特效
    auto explosion = new Explosion(explosionTemplate);
    explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
    explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
    explosion->startTime = currentTime;
    explosions.push_back(explosion);
    Mix_PlayChannel(0,sounds["enemy_shoot"],0);
    // 2.掉落物品,在一定概率下
    if(dis(gen) < 0.5){
        dropItem(enemy);
    }
    delete enemy;
    // 3.分数计算
    score += 10;
}
void SceneMain::updatePlayer(float)
{
    if(is_dead) return;
    // 玩家死亡逻辑
    // 检测是否与敌人碰撞
    for(auto enemy : enemies)
    {
        SDL_Rect enemyRect = {
                    static_cast<int>(enemy->position.x),
                    static_cast<int>(enemy->position.y),
                    enemy->width,
                    enemy->height
                };
        SDL_Rect playerRect = {
            static_cast<int>(player.position.x),
            static_cast<int>(player.position.y),
            player.width,
            player.height
        };
        if(SDL_HasIntersection(&playerRect,&enemyRect))
        {
            enemy->currentHealth = 0;
            player.currentHealth -= 1;
        }
    }
    if(player.currentHealth <= 0)
    {
        // 游戏结束
        // 使用变量来标识玩家是否死亡,如果玩家死亡
        // 1. 禁止玩家控制
        is_dead = true;

        auto currentTime = SDL_GetTicks();
        // 添加爆炸特效
        auto explosion = new Explosion(explosionTemplate);
        explosion->position.x = player.position.x + player.width / 2 - explosion->width / 2;
        explosion->position.y = player.position.y + player.height / 2 - explosion->height / 2;
        explosion->startTime = currentTime;
        explosions.push_back(explosion);
        Mix_PlayChannel(0,sounds["player_explode"],0);
    }
}
// 记住：update所要处理的只有逻辑与信息，render才负责渲染
// render渲染的信息来自于update函数
void SceneMain::updateExplosion(float)
{
    auto currentTime = SDL_GetTicks();
    for(auto it = explosions.begin() ; it != explosions.end(); )
    {
        
        Explosion* explosion = *it;
        // 计算当前帧
        // 时间间隔*FPS/1000  因为这里的时间单位为毫秒
        explosion->currentFrame = (currentTime - explosion->startTime)*explosion->FPS / 1000;
        if(explosion->currentFrame >= explosion->totalFrame)
        {
            delete explosion;
            it = explosions.erase(it);
        }else{
            ++it;
        }
    }
}

void SceneMain::renderExplosions()
{
    for(auto explosion : explosions)
    {
        // 注意这里我们通过当前帧的编号来进行图片区域的选择
        // 图片分为原图片和目标图片
        SDL_Rect src = {explosion->currentFrame * explosion->width,0,explosion->width,explosion->height};
        SDL_Rect dsc = {static_cast<int>(explosion->position.x),
                        static_cast<int>(explosion->position.y),
                        explosion->width,
                        explosion->height};
        SDL_RenderCopy(game.getRenderer(),explosion->texture,&src,&dsc);
    }
}

void SceneMain::dropItem(Enemy* enemy)
{
    auto item = new Item(itemLifeTemplate);
    item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
    item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
    float  angle = dis(gen)* 2 * M_PI;
    item->direction.x = cos(angle);
    item->direction.y = sin(angle);
    items.push_back(item);
}

void SceneMain::updateItem(float deltaTime)
{
    for(auto it = items.begin();it != items.end(); )
    {
        auto item = *it;
        // 更新位置
        item->position.x += item->direction.x * item->speed * deltaTime;
        item->position.y += item->direction.y * item->speed * deltaTime;
        // 判断如果超出屏幕范围则删除
        // 判断反弹
        if(item->position.x < 0 && item->bounce > 0 )
        {
            item->direction.x *= -1;
            --item->bounce;
        }
        if(item->position.x + item->width > game.getWindowWidth() && item->bounce > 0)
        {
            item->direction.x *= -1;
            --item->bounce;
        }
        if(item->position.y < 0&& item->bounce > 0)
        {
            item->direction.y *= -1;
            --item->bounce;
        }
        if(item->position.y + item->height > game.getWindowHeight()&& item->bounce > 0)
        {
            item->direction.y *= -1;
            --item->bounce;
        }

        if( item->position.x + item->width < 0 ||
            item->position.x > game.getWindowWidth() ||    
            item->position.y > game.getWindowHeight() ||
            item->position.y + item->height < 0 )
            {
                delete item;
                it = items.erase(it);
            }
        else {
            SDL_Rect itemRect = {
            static_cast<int>(item->position.x),
            static_cast<int>(item->position.y),
            item->width,
            item->height
            };
            SDL_Rect playerRect = {
                static_cast<int>(player.position.x),
                static_cast<int>(player.position.y),
                player.width,
                player.height
            };
            if(SDL_HasIntersection(&itemRect,&playerRect) && (!is_dead))
            {
                // 负责获取道具并清除对应的图标
                playerGetItem(item);
                delete item;
                it = items.erase(it);
            }else
            {
                ++it;
            }

        }
    }

}

void SceneMain::playerGetItem(Item* item)
{
    // 添加分数
    score += 5;
    if(item->type == ItemType::Health)
    {
        if(player.currentHealth < player.maxHealth)
        {
            ++player.currentHealth;
            // std:: cout << "当前生命值："<< player.currentHealth << std::endl;
        }
    }
    Mix_PlayChannel(0,sounds["get_item"],0);
}

void SceneMain::renderItems()
{
    for(auto& item : items)
    {
        SDL_Rect itemRect = {
                    static_cast<int>(item->position.x),
                    static_cast<int>(item->position.y),
                    item->width,
                    item->height
                };
        SDL_RenderCopy(game.getRenderer(),item->texture,NULL,&itemRect);
    }
}

void SceneMain::renderUI()
{
    int x = 10;
    int y = 10;
    int size = 32;
    int offset = 40;
    // 1.渲染血量
    // 修改颜色，后三个参数代表RGB 
    SDL_SetTextureColorMod(uiHealth,100,100,100);
    for(int i = 0;i < player.maxHealth;++i)
    {
        SDL_Rect rect = {x + i*offset , y ,size,size};
        SDL_RenderCopy(game.getRenderer(),uiHealth,NULL,&rect);
    }
    // 恢复颜色
    SDL_SetTextureColorMod(uiHealth,255,255,255);
    for(int i = 0;i < player.currentHealth;++i)
    {
        SDL_Rect rect = {x + i*offset , y ,size,size};
        SDL_RenderCopy(game.getRenderer(),uiHealth,NULL,&rect);
    }
    // 2.渲染得分
    // 每次渲染后都要进行清理
    std::string text = "SCORE:" + std::to_string(score);
    SDL_Color color= {255,255,255,255};
    SDL_Surface* surface = TTF_RenderUTF8_Solid(scoreFont,text.c_str(),color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(game.getRenderer(),surface);
    SDL_Rect rect = {game.getWindowWidth() - 150 , 10 , surface->w , surface->h};
    SDL_RenderCopy(game.getRenderer(),texture,NULL,&rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
