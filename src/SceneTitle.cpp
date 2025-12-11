#include"SceneTitle.h"
#include"Game.h"
#include"Scene_Main.h"
#include<iostream>
#include<string>

void SceneTitle::init()
{
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if(bgm == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"Could not open title music: %s\n",SDL_GetError());
    }
    Mix_PlayMusic(bgm,-1);
}

void SceneTitle::handleEvents(SDL_Event *event)
{
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.scancode == SDL_SCANCODE_J)
        {
            auto sceneMain = new SceneMain;
            // change 函数负责调用初始化，这里不用初始化
            game.changeScene(sceneMain);

        }
    }
}

void SceneTitle::update(float deltaTime)
{
    timer += deltaTime;
    if(timer > 1.0f){
        timer -= 1.0f;
    }

}

void SceneTitle::render()
{
    // 渲染标题文字
    std::string titleText = "SDL太空战机";
    game.renderTextCentered(titleText,0.4,true);
    // 渲染普通文字
    // 通过计时器来使文字闪烁
    std::string instructions = "按 J 键开始游戏";
    if(timer < 0.5f) game.renderTextCentered(instructions,0.7,false);
}

void SceneTitle::clean()
{
}
