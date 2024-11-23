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
    //
    // switch (NUMOFJOYSTICKS)
    // {
    // case 0:
    //
    //     switch (sdlEvent.type)
    //     {
    //     case SDL_KEYDOWN:
    //         switch (sdlEvent.key.keysym.scancode)
    //         {
    //         case SDL_SCANCODE_W:
    //             KeyboardY = 1;
    //             TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkFront");
    //             break;
    //         case SDL_SCANCODE_S:
    //             KeyboardY = -1;
    //             TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkBack");
    //             break;
    //         case SDL_SCANCODE_A:
    //             KeyboardX = -1;
    //             TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkLeft");
    //             break;
    //         case SDL_SCANCODE_D:
    //             KeyboardX = 1;
    //             TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkRight");
    //             break;
    //         default:
    //             break;
    //         }
    //         break;
    //     case SDL_KEYUP: // Handle when keys are released
    //         switch (sdlEvent.key.keysym.scancode)
    //         {
    //         case SDL_SCANCODE_W:
    //         case SDL_SCANCODE_S:
    //             KeyboardY = 0;
    //             if(KeyboardX == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             break;
    //         case SDL_SCANCODE_A:
    //         case SDL_SCANCODE_D:
    //             KeyboardX = 0;
    //             if (KeyboardY == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             break;
    //         default:
    //             break;
    //         }
    //         break;
    //
    //     default:
    //         break;
    //     }
    //
    //     break;
    //
    // case 1:
    //
    //     switch (sdlEvent.type)
    //     {
    //     case SDL_CONTROLLERBUTTONDOWN:
    //     case SDL_CONTROLLERBUTTONUP:
    //         //  printf("Controller button event detected!\n");
    //         // printf("Button: %d, State: %s\n",
    //         //      sdlEvent.cbutton.button,
    //         //       sdlEvent.type == SDL_CONTROLLERBUTTONDOWN ? "Pressed" : "Released");
    //
    //         //    Low frequency rumble  High frequency rumble   Duration
    //         //SDL_GameControllerRumble(controller, 65535, 65535, 500);
    //
    //         SDL_GameControllerRumble(controller, 30000, 0, 100);
    //
    //         break;
    //     case SDL_CONTROLLERAXISMOTION:
    //         // printf("Controller axis motion detected!\n");
    //         //  printf("Axis: %d, Value: %d\n", sdlEvent.caxis.axis, sdlEvent.caxis.value);
    //
    //         switch (sdlEvent.caxis.axis)
    //         {
    //         case SDL_CONTROLLER_AXIS_LEFTX:
    //             if (abs(sdlEvent.caxis.value) > JOYSTICK_DEAD_ZONE)
    //             {
    //                 leftStickX = sdlEvent.caxis.value;
    //                 if(leftStickX > 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkRight");}
    //                 if(leftStickX < 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkLeft");}
    //             }
    //             else
    //             {
    //                 leftStickX = 0;
    //                 if(leftStickY == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             }
    //             break;
    //         case SDL_CONTROLLER_AXIS_LEFTY:
    //             if (abs(sdlEvent.caxis.value) > JOYSTICK_DEAD_ZONE)
    //             {
    //                 leftStickY = sdlEvent.caxis.value;
    //                 if(leftStickY < 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkFront");}
    //                 if(leftStickY > 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkBack");}
    //             }
    //             else
    //             {
    //                 leftStickY = 0;
    //                 if (leftStickX == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
    //             }
    //             break;
    //         }
    //     }
    //
    //     break;
    //
    // default:
    //     if (NUMOFJOYSTICKS > 1)
    //     {
    //         printf("Sorry we don't support multiple joysticks yet\n");
    //     }
    //     break;
    // }
}

// void KeyboardMovement()
//{
//     TestEntity.getComponent<SpriteComponent>().setPosX(TestEntity.getComponent<SpriteComponent>().X() + (KeyboardX * 2.5f));
//     TestEntity.getComponent<SpriteComponent>().setPosY(TestEntity.getComponent<SpriteComponent>().Y() + (KeyboardY * 2.5f));
// }
// void JoystickMovement()
//{
//
//     TestEntity.getComponent<SpriteComponent>().setPosX(
//         TestEntity.getComponent<SpriteComponent>().X() + (leftStickX * 0.0001f));
//     TestEntity.getComponent<SpriteComponent>().setPosY(
//         (TestEntity.getComponent<SpriteComponent>().Y() + (-leftStickY * 0.0001f)));
// }

void BaseScene::Update(const float deltaTime)
{
    if (isDebugging)
        DubugGUI();

    if (IsPaused)
        return;

    // std::cout << "FPS: " << 1.0f / deltaTime << std::endl;

    // TestEntity.getComponent<SpriteComponent>().Update(deltaTime);
    // background.getComponent<SpriteComponent>().Update(deltaTime);
    bridge.Update(deltaTime);
    switch (NUMOFJOYSTICKS)
    {
    case 0:
        // KeyboardMovement();
        break;
    case 1:
        // JoystickMovement();
        break;
    default:
        if (NUMOFJOYSTICKS > 1)
        {
            // JoystickMovement();
        }
        break;
    }


}

void BaseScene::DubugGUI()
{
    bool open = true;
    ImGui::Begin("Frame rate", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowSize(ImVec2(200, 100));
    ImGui::SetWindowPos("Frame rate", ImVec2(0, 0));
    static float values[90] = {};
    static int values_offset = 0;
    static double refresh_time = 0.0;
    while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
    {
        static float phase = 0.0f;
        values[values_offset] = ImGui::GetIO().Framerate;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
        phase += 0.10f * values_offset;
        refresh_time += 1.0f / 60.0f;
    }

    ImGui::Text("%.1f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
    float average = 0.0f;
    for (int n = 0; n < IM_ARRAYSIZE(values); n++)
        average += values[n];
    average /= (float)IM_ARRAYSIZE(values);
    char overlay[32];
    sprintf_s(overlay, "avg fps %f", average);
    ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 80.0f, ImVec2(0, 80.0f));
    ImGui::End();

    ImGui::Begin("Input", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    ImGui::SetWindowSize(ImVec2(200, 150));
    ImGui::SetWindowPos("Input", ImVec2(200, 0));
    // ImGui::Text("X: %f", TestEntity.getComponent<SpriteComponent>().X());
    // ImGui::Text("Y: %f", TestEntity.getComponent<SpriteComponent>().Y());

    if (ImGui::Button("Controller", ImVec2(100, 0)))
    {
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
            }
        }
    }

    if (ImGui::Button("Keyboard", ImVec2(100, 0)))
    {
        NUMOFJOYSTICKS = 0;
        printf("Switched to keyboard input\n");
    }
    if (IsPaused)
    {
        ImGui::Text("Paused");
    }

    ImGui::End();
}

void BaseScene::Render() const
{

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bridge.Render();


    if (isDebugging)
    {
        // TestEntity.getComponent<ColliderComponent>().Render();
        // Capsule.getComponent<ColliderComponent>().Render();
        // TestCollider.getComponent<ColliderComponent>().Render();
        // Square.getComponent<ColliderComponent>().Render();
    }

    drawInWireMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    glUseProgram(0);
}
