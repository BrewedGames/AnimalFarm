#ifndef SCENETEMPLATE_H
#define SCENETEMPLATE_H
#include <iostream>
#include "Scene.h"
union SDL_Event;

class SceneTemplate : public Scene {

public:
    virtual ~SceneTemplate() = default;
     virtual bool OnCreate() override {
        std::cout << "OnCreate Called " << std::endl;
        return true;
    }


    virtual void OnDestroy() override {
    }

    virtual void Update(const float deltaTime) override {
    }

    virtual void Render() const override {

    }

    virtual void HandleEvents(const SDL_Event &sdlEvent) override {
    }
};
#endif // SCENETEMPLATE_H