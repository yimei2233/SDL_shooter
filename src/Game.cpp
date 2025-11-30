#include"Game.h"
#include "Scene_Main.h"
#include<SDL.h>
#include<SDL_image.h>

Game::Game()
{

}
Game::~Game()
{
    clean();
}
// 只初始化一次
void Game::init()
{
    // 初始化帧率
    frameTime = 1000 / FPS;

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        // 初始化SDL失败
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Unable to initialize SDL: %s \n", SDL_GetError());
        isRunning = false;
    }
    // 创建窗口
    window = SDL_CreateWindow("SDL Shooter",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,windowWidth,windowHeight,SDL_WINDOW_SHOWN);
    if(window == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not create window: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 创建渲染器
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not create renderer: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 初始化SDL_image
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not initialize SDL_image: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 初始化当前场景
    currentScene = new SceneMain();
    currentScene->init();
}
void Game::clean()
{
    if(currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::changeScene(Scene *scene)
{
    if(currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    currentScene = scene;
    currentScene->init();
}

void Game::run()
{
    while(isRunning)
    {
        // 记录每一帧的时间
        auto frameStart = SDL_GetTicks();

        // 将游戏逻辑分开
        SDL_Event event;
        // 1.事件处理
        handleEvent(&event);

        // 2.更新
        update(deltaTime);
        
        // 3.渲染
        render();
        
        // 4.控制帧率
        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if(diff < frameTime){
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        }else{
            deltaTime = diff / 1000.0f;
        }
    }
    
}

void Game::handleEvent(SDL_Event* event)
{
    while(SDL_PollEvent(event))
        {
            if(event->type == SDL_QUIT)
            {
                isRunning = false;
            }
            currentScene->handleEvents(event);
        }
}

void Game::update(float deltaTime)
{
    currentScene->update(deltaTime);
}

void Game::render()
{
    // 1. 清屏
    SDL_RenderClear(renderer);
    // 2. 渲染当前场景
    currentScene->render();
    // 3. 显示出来
    SDL_RenderPresent(renderer);
}