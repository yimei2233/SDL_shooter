#ifndef GAME_H
#define GAME_H

#include "Scene.h"
#include"Object.h"
#include"SceneTitle.h"

#include<SDL.h>
#include<SDL_ttf.h>
#include<string>
class Game{

public:
    // 单例模式：静态成员函数，获取类的唯一接口
    static Game& getInstance()
    {
        static Game instance;
        return instance;
    }
    ~Game();

    void init();
    void clean();
    void run();
    void changeScene(Scene *scene);
    // getters 函数
    // 访问成员变量的接口
    SDL_Window* getWindow(){ return window; }
    SDL_Renderer* getRenderer(){ return renderer; }
    int getWindowWidth(){ return windowWidth; }
    int getWindowHeight(){ return windowHeight; }
    // setters

    // 渲染文字函数
    // 因为渲染文字很复杂，这里单独写一个函数来对对应的文字进行绘制
    void renderTextCentered(std::string text,float posY,bool isTitle);

private:
    Game();
    // 删除拷贝与赋值构造函数
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // run() 的逻辑: 事件处理 -> 更新 -> 渲染
    void handleEvent(SDL_Event* event);
    void update(float deltaTime);
    void render();
    // 背景更新，渲染
    void backgroundUpdate(float deltaTime);
    void renderBackground();
    

private:
    // 经常用的变量
    bool isRunning = true;
    Scene* currentScene = nullptr;
    SDL_Window* window = nullptr;
    SDL_Renderer * renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;
    // 控制游戏帧率，防止游戏速度与cpu有关
    int FPS = 60;
    Uint32 frameTime;
    float deltaTime;
    // 背景图片
    Background nearStars;
    Background farStars;
    // 字体资源
    TTF_Font* titleFont;
    TTF_Font* textFont;
};



#endif