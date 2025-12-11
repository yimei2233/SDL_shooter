#ifndef SCENETITLE_H
#define SCENETITLE_H

#include"Scene.h"
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h>

// 这里要添加 public
class SceneTitle : public Scene
{

public:
    void init() override;
    void handleEvents(SDL_Event* event)override;
    void update(float deltaTime)override;
    void render()override;
    void clean()override;

private:
    Mix_Music* bgm;
    // 计时器i设计
    float timer = 0;

};




#endif