#ifndef SCENE_H
#define SCENE_H

#include<SDL.h>

// 基类
class Scene
{
public:
    // 默认构造函数和析构函数，子类可以根据需求重写
    Scene() = default;
    ~Scene() = default;
    // 纯虚函数，强制子类实现这些方法
    virtual void init() = 0;
    virtual void handleEvents(SDL_Event* event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void clean() = 0;
};


#endif