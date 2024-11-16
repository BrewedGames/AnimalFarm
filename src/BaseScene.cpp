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

static unordered_map<refPtr<Entity>, int> entityMap;

static Manager manager;
// static Entity &TestEntity(manager.addEntity("TestEntity"));
// static Entity &TestCollider(manager.addEntity("TestCollider"));
// static Entity &Capsule(manager.addEntity("Capsule"));
// static Entity &Square(manager.addEntity("Square"));
// static Entity &background(manager.addEntity("background"));

// static Entity &LuaEntity(manager.addEntity("LuaEntity"));

static Entity &Skull(manager.addEntity("skull"));
static Entity &shader(manager.addEntity("shader"));


Matrix4 skullMatrix;

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

    Skull.addComponent<MeshComponent>(nullptr, "./src/meshes/Skull.obj");
    Skull.addComponent<Transform3DComponent>(Vec3(600.0f, 200.0f, 0.f), Vec3(50.f, 50.f, 50.f), Quaternion(1.0f, Vec3(0.0f, 1.0f, 0.0f)));
    Skull.addComponent<TextureComponent>().LoadTexture("./src/textures/skull_texture.jpg");

    shader.addComponent<ShaderComponent>(nullptr, "./src/shaders/textureVert.glsl", "./src/shaders/textureFrag.glsl");
    if (shader.getComponent<ShaderComponent>().OnCreate() == false)
    {
        cout << "Shader failed ... we have a problem\n";
    }


    //  Skull.addComponent<MeshComponent>("./src/meshes/Skull.obj", "./src/textures/skull_texture.jpg", LIT); // we should have three options for shaders LIT, UNLIT and custom
    // if texture not defined or failed then use blank_texture.jpg
    // in theory we should only need a mesh and transform component 
    // also we should make it to that we can use a vec3 with angles instead of a quaternion
    // Skull.addComponent<Transform3DComponent>(Vec3(600.0f, 200.0f, 0.f), Vec3(50.f, 50.f, 50.f), Vec3(0.0f, 90.0f, 0.0f));




   


    // if (!TestEntity.addComponent<SpriteComponent>().LoadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(40.0f, 100.0f, 1.0f), true, 32, 8, 100))
    // {
    //     cout << "Sprite failed ... we have a problem\n";
    //     return false;
    // }

    // if (!background.addComponent<SpriteComponent>().LoadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350.0f, 200.0f, 0.0f)))
    // {
    //     cout << "Sprite failed ... we have a problem\n";
    //     return false;
    // }

    // TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkBack", 0, 7, 100);
    // TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkLeft", 8, 15, 100);
    // TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkRight", 16, 23, 100);
    // TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkFront", 24, 31, 100);

    // TestEntity.addComponent<ColliderComponent>().AddCircleCollider(TestEntity.getComponent<SpriteComponent>().X(), TestEntity.getComponent<SpriteComponent>().Y(), 60);
    // Capsule.addComponent<ColliderComponent>().AddCapsuleCollider(TestEntity.getComponent<SpriteComponent>().X() + 300, TestEntity.getComponent<SpriteComponent>().Y(), 100, 200);
    // Square.addComponent<ColliderComponent>().AddAABBCollider(700, 300, 100, 100);
    // TestCollider.addComponent<ColliderComponent>().AddCircleCollider(500, 300, 100);
    //  TestEntity.addComponent<ColliderComponent>().

    return true;
}

void BaseScene::OnDestroy()
{

    if (controller != nullptr)
    {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }

    // TestEntity.getComponent<SpriteComponent>().OnDestroy();
    // TestEntity.OnDestroy();
    // TestCollider.getComponent<ColliderComponent>().OnDestroy();
    // TestCollider.OnDestroy();
    // Capsule.getComponent<ColliderComponent>().OnDestroy();
    // Capsule.OnDestroy();
    // Square.getComponent<ColliderComponent>().OnDestroy();
    // Square.OnDestroy();

    // manager.clearEntities();

    // Mix_CloseAudio();

    //	Shader.removeComponent<ShaderComponent>();

    Skull.getComponent<MeshComponent>().OnDestroy();
    Skull.OnDestroy();
    delete &Skull;
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

    static float angle = 0.0f;
	angle += 30.0f * deltaTime; 
	skullMatrix =  Skull.getComponent<Transform3DComponent>().transform() * MMath::rotate(angle, Vec3(0.0f, 1.0f, 0.0f)) ;


    // TestCollider.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    // Capsule.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    // Square.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    // TestEntity.getComponent<ColliderComponent>().setPos(Vec3(TestEntity.getComponent<SpriteComponent>().X(), TestEntity.getComponent<SpriteComponent>().Y(), 0.0f));

    // std::cout << TestEntity.getComponent<SpriteComponent>().GetSpritePath() << std::endl;
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






    // background.getComponent<SpriteComponent>().Render();
    // TestEntity.getComponent<SpriteComponent>().Render();
    bridge.Render();

    if (isDebugging)
    {
        // TestEntity.getComponent<ColliderComponent>().Render();
        // Capsule.getComponent<ColliderComponent>().Render();
        // TestCollider.getComponent<ColliderComponent>().Render();
        // Square.getComponent<ColliderComponent>().Render();
    }

    drawInWireMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    glDisable(GL_CULL_FACE);
    glUseProgram(shader.getComponent<ShaderComponent>().GetProgram());
    glBindTexture(GL_TEXTURE_2D, Skull.getComponent<TextureComponent>().getTextureID());
    glUniformMatrix4fv(shader.getComponent<ShaderComponent>().GetUniformID("projectionMatrix"), 1, GL_FALSE, cam.GetProjectionMatrix());
    glUniformMatrix4fv(shader.getComponent<ShaderComponent>().GetUniformID("viewMatrix"), 1, GL_FALSE, cam.GetViewMatrix());
    glUniformMatrix4fv(shader.getComponent<ShaderComponent>().GetUniformID("modelMatrix"), 1, GL_FALSE, skullMatrix);
    Skull.getComponent<MeshComponent>().Render(GL_TRIANGLES);

    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);
}
