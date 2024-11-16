#ifndef BRIDGE_H
#define BRIDGE_H

#define SQUNICODE

#include <sol.hpp>
#include <string>
#include <iostream>
#include <filesystem>
#include "Scene.h"
#include "SceneTemplate.h"
#include "SceneGenerator.h"
#include "Vector.h"
#include <SDL.h>

#include "Camera.h"

using namespace MATH;

class Bridge {
public:
    Bridge();
    ~Bridge();



    void SetupBridge();
    void register_key_constants();
    void register_controller_constants();
    void poll_controller_state();
    std::string get_key_name(SDL_Keycode keycode);
    void bind_controller(SDL_GameController* controller);
    void process_sdl_event(const SDL_Event &event);
    static void CreateGameObject(const std::string &name);

    void OnCreate(); 
    void Update(float deltaTime);  
    void Render() const;  


    Camera cam;



private:

    sol::state lua;
    SDL_GameController* lua_controller = nullptr;

 
};

#endif // BRIDGE_H
