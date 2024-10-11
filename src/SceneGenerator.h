// SceneGenerator.h
#ifndef SCENEGENERATOR_H
#define SCENEGENERATOR_H

#include <string>
#include <iostream>
#include "SceneTemplate.h"

#define GENERATE_SCENE_CLASS(sceneName) \
class sceneName : public SceneTemplate { \
public: \
    sceneName() {} \
    ~sceneName() {} \
    bool OnCreate() override { \
        std::cout << "OnCreate called for " << #sceneName << std::endl; \
        return true; \
    } \
    void OnDestroy() override { \
        std::cout << "OnDestroy called for " << #sceneName << std::endl; \
    } \
    void Update(const float deltaTime) override { \
        std::cout << "Update called for " << #sceneName << std::endl; \
    } \
    void Render() const override { \
        std::cout << "Render called for " << #sceneName << std::endl; \
    } \
    void HandleEvents(const SDL_Event &sdlEvent) override { \
        std::cout << "HandleEvents called for " << #sceneName << std::endl; \
    } \
};

#endif // SCENEGENERATOR_H