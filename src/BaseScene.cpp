#include <SDL_image.h>
#include <glew.h>
#include <iostream>
#include <SDL.h>
#include <vector>
#include <unordered_map>
#include "BaseScene.h"
#include "ECS.h"
#include "ECSComponents.h"
#include <map>
#include <simpleini.h>
#include <cmath>
#include <VMath.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_opengl3.h"
#include "stb_image.h"

using namespace std;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define GET_VARIABLE_NAME(Variable) (#Variable)

template <typename T>
using refPtr = std::shared_ptr<T>;
static Manager manager;




union SDL_Event;

int NUMOFJOYSTICKS = 0;
const int JOYSTICK_DEAD_ZONE = 8000;
SDL_GameController *controller;
int leftStickX = 0;
int leftStickY = 0;

int KeyboardX = 0;
int KeyboardY = 0;

bool isDebugging = true;

BaseScene::BaseScene() : drawInWireMode(false), IsPaused(false) {}

BaseScene::~BaseScene() {}

bool BaseScene::OnCreate()
{

    bridge.SetupBridge();

    if (SDL_NumJoysticks() < 1)
    {
        printf("No game controller detected! Continuing with keyboard input\n");
    }
    else
    {
        NUMOFJOYSTICKS = SDL_NumJoysticks();
        printf("Number of game controllers detected: %d\n", NUMOFJOYSTICKS);
        controller = SDL_GameControllerOpen(0);
        if (controller == NULL)
        {
            printf("Unable to open game controller: %s\n", SDL_GetError());
        }
        else
        {
            printf("Controller detected and opened!\n");
            bridge.bind_controller(controller);
        }
    }

   

    return true;

}

void BaseScene::OnDestroy()
{

    if (controller != nullptr)
    {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
}

void BaseScene::HandleEvents(const SDL_Event &sdlEvent)
{
    bridge.process_sdl_event(sdlEvent);
    switch (sdlEvent.type)
    {
    case SDL_KEYDOWN:
        switch (sdlEvent.key.keysym.scancode)
        {
        case SDL_SCANCODE_L:
            drawInWireMode = !drawInWireMode;
            break;
        case SDL_SCANCODE_P:
            IsPaused = !IsPaused;
            break;
        case SDL_SCANCODE_O:
            isDebugging = !isDebugging;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }
 
}

void BaseScene::Update(const float deltaTime)
{
    if (isDebugging)
        DubugGUI();

    if (IsPaused)
        return;

    bridge.Update(deltaTime);


}


void BaseScene::DubugGUI()
{
    bool open = true;
    ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowSize(ImVec2(200, 50));
    ImGui::SetWindowPos("Frame rate", ImVec2(0, 0));

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.1f ms", 1000.0f / ImGui::GetIO().Framerate);

    ImGui::End();
}

void BaseScene::Render() const
{

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bridge.Render();

    drawInWireMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(0);
}
