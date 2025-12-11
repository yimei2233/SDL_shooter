#include"Game.h"
#include "Scene_Main.h"

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>

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
    // 初始化SDL_image
    if(Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG) != (MIX_INIT_MP3 | MIX_INIT_OGG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not initialize SDL_mix: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 初始化SDL_ttf
    if(TTF_Init() == -1)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not initialize SDL_ttf: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 打开音频设备
    if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048) < 0){
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not open audio: %s\n",SDL_GetError());
        isRunning = false;
    }
    // 设置音效channel数量 可同时播放音效的个数
    Mix_AllocateChannels(32);
    // 设置音乐音量
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    // 设置音效音量
    Mix_Volume(-1,MIX_MAX_VOLUME);
    

    // 初始化背景
    nearStars.texture = IMG_LoadTexture(renderer , "assets/image/Stars-A.png");
    SDL_QueryTexture(nearStars.texture,NULL,NULL,&nearStars.width , &nearStars.height);
    farStars.texture = IMG_LoadTexture(renderer , "assets/image/Stars-B.png");
    SDL_QueryTexture(farStars.texture,NULL,NULL,&farStars.width , &farStars.height);
    farStars.speed = 20;

    // 载入字体
    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf",64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf",32);
    if(titleFont == nullptr || textFont == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not create ttf: %s\n",SDL_GetError());
        isRunning = false;
    }

    // 初始化当前场景，这里决定了第一个创建的场景
    currentScene = new SceneTitle;
    currentScene->init();

}
void Game::clean()
{
    if(currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    if(nearStars.texture != nullptr)
    {
        SDL_DestroyTexture(nearStars.texture);
    }
    if(farStars.texture != nullptr)
    {
        SDL_DestroyTexture(farStars.texture);
    }
    if(titleFont != nullptr)
    {
        TTF_CloseFont(titleFont);
    }
    if(textFont != nullptr)
    {
        TTF_CloseFont(textFont);
    }

    // 清理SDL_IMG
    IMG_Quit();
    // 清理SDL_MIXer
    Mix_CloseAudio();
    Mix_Quit();
    // 清理ttf
    TTF_Quit();
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
    backgroundUpdate(deltaTime);
    currentScene->update(deltaTime);
}

void Game::render()
{
    // 1. 清屏
    SDL_RenderClear(renderer);
    // 渲染背景
    renderBackground();
    // 2. 渲染当前场景
    currentScene->render();
    // 3. 显示出来
    SDL_RenderPresent(renderer);
}

void Game::backgroundUpdate(float deltaTime)
{
    nearStars.offset += nearStars.speed * deltaTime;
    if(nearStars.offset >= 0)
    {
        nearStars.offset -= nearStars.height;
    }
    farStars.offset += farStars.speed * deltaTime;
    if(farStars.offset >= 0)
    {
        farStars.offset -= nearStars.height;
    }

}

void Game::renderBackground()
{
    // 远处的star
    int posY = static_cast<int>(farStars.offset);
    for( ;posY < getWindowHeight();posY += farStars.height)
    {
        for(int posX = 0;posX < getWindowWidth(); posX += farStars.width)
        {
            SDL_Rect dstRect = {posX,posY,farStars.width,farStars.height};
            SDL_RenderCopy(renderer , farStars.texture , nullptr , &dstRect);
        }
    }
    // 近处的star
    posY = static_cast<int>(nearStars.offset);
    for( ;posY < getWindowHeight();posY += nearStars.height)
    {
        for(int posX = 0;posX < getWindowWidth(); posX += nearStars.width)
        {
            SDL_Rect dstRect = {posX,posY,nearStars.width,nearStars.height};
            SDL_RenderCopy(renderer , nearStars.texture , nullptr , &dstRect);
        }
    }
}

void Game::renderTextCentered(std::string text, float posY, bool isTitle)
{
    SDL_Color color= {255,255,255,255};
    SDL_Surface* surface;
    // 区分是否为标题字体
    if(isTitle)
    {
        surface = TTF_RenderUTF8_Solid(titleFont,text.c_str(),color);
    }else{
        surface = TTF_RenderUTF8_Solid(textFont,text.c_str(),color);
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(getRenderer(),surface);
    int y = (getWindowHeight() - surface->h) * posY;
    SDL_Rect rect = {getWindowWidth()/2 - surface->w/2,
                        y,
                        surface->w ,
                        surface->h};
    SDL_RenderCopy(getRenderer(),texture,NULL,&rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
