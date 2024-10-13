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

#include <SDL_mixer.h>

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
static Entity &TestEntity(manager.addEntity("TestEntity"));
static Entity &TestCollider(manager.addEntity("TestCollider"));
static Entity &Capsule(manager.addEntity("Capsule"));
static Entity &Square(manager.addEntity("Square"));
static Entity &Shader(manager.addEntity("Shader"));
static Entity &background(manager.addEntity("background"));

union SDL_Event;

int NUMOFJOYSTICKS = 0;
const int JOYSTICK_DEAD_ZONE = 8000;
SDL_GameController *controller;
int leftStickX = 0;
int leftStickY = 0;

int KeyboardX = 0;
int KeyboardY = 0;

bool isDebugging = true;

Mix_Music *backgroundMusic = nullptr;
Mix_Chunk *soundEffect = nullptr;



BaseScene::BaseScene() : drawInWireMode(false), IsPaused(false) {}

BaseScene::~BaseScene() {}

bool BaseScene::OnCreate()
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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    // Load music and sound effect
    backgroundMusic = Mix_LoadMUS("./src/audio/Mysterious Ambience.mp3");
    if (backgroundMusic == nullptr)
    {
        std::cout << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    soundEffect = Mix_LoadWAV("./src/audio/mixkit-retro-game-notification-212.wav");
    if (soundEffect == nullptr)
    {
        std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return -1;
    }
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2); 
    Mix_PlayMusic(backgroundMusic, -1);
    

    Shader.addComponent<ShaderComponent>(nullptr, "./src/shaders/DefaultTextureShader.vert", "./src/shaders/DefaultTextureShade.frag");
    if (Shader.getComponent<ShaderComponent>().OnCreate() == false)
    {
        cout << "Shader failed ... we have a problem\n";
    }


    if (!TestEntity.addComponent<SpriteComponent>().LoadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(40.0f, 100.0f, 1.0f), true, 32, 8, 100))
    {
        cout << "Sprite failed ... we have a problem\n";
        return false;
    }


    if (!background.addComponent<SpriteComponent>().LoadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350.0f, 200.0f, 0.0f)))
    {
        cout << "Sprite failed ... we have a problem\n";
        return false;
    }

    TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkBack", 0, 7, 100);
    TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkLeft", 8, 15, 100);
    TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkRight", 16, 23, 100);
    TestEntity.getComponent<SpriteComponent>().SetAnimation("WalkFront", 24, 31, 100);

    TestEntity.addComponent<ColliderComponent>().AddCircleCollider(TestEntity.getComponent<SpriteComponent>().X(), TestEntity.getComponent<SpriteComponent>().Y(), 60);
    Capsule.addComponent<ColliderComponent>().AddCapsuleCollider(TestEntity.getComponent<SpriteComponent>().X() + 300, TestEntity.getComponent<SpriteComponent>().Y(), 100, 200);
    Square.addComponent<ColliderComponent>().AddAABBCollider(700, 300, 100, 100);
    TestCollider.addComponent<ColliderComponent>().AddCircleCollider(500, 300, 100);

    return true;
}

void BaseScene::OnDestroy()
{

    if (controller != nullptr)
    {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }

    Shader.getComponent<ShaderComponent>().OnDestroy();
    Shader.OnDestroy();
    TestEntity.getComponent<SpriteComponent>().OnDestroy();
    TestEntity.OnDestroy();
    TestCollider.getComponent<ColliderComponent>().OnDestroy();
    TestCollider.OnDestroy();
    Capsule.getComponent<ColliderComponent>().OnDestroy();
    Capsule.OnDestroy();
    Square.getComponent<ColliderComponent>().OnDestroy();
    Square.OnDestroy();

    manager.clearEntities();

    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(soundEffect);
    Mix_CloseAudio();

    //	Shader.removeComponent<ShaderComponent>();
}

void BaseScene::HandleEvents(const SDL_Event &sdlEvent)
{
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

    switch (NUMOFJOYSTICKS)
    {
    case 0:

        switch (sdlEvent.type)
        {
        case SDL_KEYDOWN:
            switch (sdlEvent.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
                KeyboardY = 1;
                TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkFront");
                break;
            case SDL_SCANCODE_S:
                KeyboardY = -1;
                TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkBack");
                break;
            case SDL_SCANCODE_A:
                KeyboardX = -1;
                TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkLeft");
                break;
            case SDL_SCANCODE_D:
                KeyboardX = 1;
                TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkRight");
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP: // Handle when keys are released
            switch (sdlEvent.key.keysym.scancode)
            {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                KeyboardY = 0;
                if(KeyboardX == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                KeyboardX = 0;
                if (KeyboardY == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }

        break;

    case 1:

        switch (sdlEvent.type)
        {
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            //  printf("Controller button event detected!\n");
            // printf("Button: %d, State: %s\n",
            //      sdlEvent.cbutton.button,
            //       sdlEvent.type == SDL_CONTROLLERBUTTONDOWN ? "Pressed" : "Released");
            SDL_GameControllerRumble(controller, 65535, 65535, 500);
            Mix_PlayChannel(-1, soundEffect, 0);
            break;
        case SDL_CONTROLLERAXISMOTION:
            // printf("Controller axis motion detected!\n");
            //  printf("Axis: %d, Value: %d\n", sdlEvent.caxis.axis, sdlEvent.caxis.value);

            switch (sdlEvent.caxis.axis)
            {
            case SDL_CONTROLLER_AXIS_LEFTX:
                if (abs(sdlEvent.caxis.value) > JOYSTICK_DEAD_ZONE)
                {
                    leftStickX = sdlEvent.caxis.value;
                    if(leftStickX > 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkRight");}
                    if(leftStickX < 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkLeft");}
                }
                else
                {
                    leftStickX = 0;
                    if(leftStickY == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                }
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                if (abs(sdlEvent.caxis.value) > JOYSTICK_DEAD_ZONE)
                {
                    leftStickY = sdlEvent.caxis.value;
                    if(leftStickY < 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkFront");}
                    if(leftStickY > 0) {TestEntity.getComponent<SpriteComponent>().ClearAnimation(); TestEntity.getComponent<SpriteComponent>().PlayAnimation("WalkBack");}
                }
                else
                {
                    leftStickY = 0;
                    if (leftStickX == 0) TestEntity.getComponent<SpriteComponent>().ClearAnimation();
                }
                break;
            }
        }

        break;

    default:
        if (NUMOFJOYSTICKS > 1)
        {
            printf("Sorry we don't support multiple joysticks yet\n");
        }
        break;
    }



}

void KeyboardMovement()
{
    TestEntity.getComponent<SpriteComponent>().setPosX(TestEntity.getComponent<SpriteComponent>().X() + (KeyboardX * 2.5f));
    TestEntity.getComponent<SpriteComponent>().setPosY(TestEntity.getComponent<SpriteComponent>().Y() + (KeyboardY * 2.5f));
}
void JoystickMovement()
{

    TestEntity.getComponent<SpriteComponent>().setPosX(
        TestEntity.getComponent<SpriteComponent>().X() + (leftStickX * 0.0001f));
    TestEntity.getComponent<SpriteComponent>().setPosY(
        (TestEntity.getComponent<SpriteComponent>().Y() + (-leftStickY * 0.0001f)));
}

void BaseScene::Update(const float deltaTime)
{
    if (isDebugging)
        DubugGUI();

    if (IsPaused)
        return;

    // std::cout << "FPS: " << 1.0f / deltaTime << std::endl;
    dt = deltaTime;

    TestEntity.getComponent<SpriteComponent>().Update(deltaTime);
    background.getComponent<SpriteComponent>().Update(deltaTime);
    switch (NUMOFJOYSTICKS)
    {
    case 0:
        KeyboardMovement();
        break;
    case 1:
        JoystickMovement();
        break;
    default:
        if (NUMOFJOYSTICKS > 1)
        {
            JoystickMovement();
        }
        break;
    }

    TestCollider.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    Capsule.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    Square.getComponent<ColliderComponent>().isColliding(&TestEntity.getComponent<ColliderComponent>());
    TestEntity.getComponent<ColliderComponent>().setPos(Vec3(TestEntity.getComponent<SpriteComponent>().X(), TestEntity.getComponent<SpriteComponent>().Y(), 0.0f));
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
    ImGui::Text("X: %f", TestEntity.getComponent<SpriteComponent>().X());
    ImGui::Text("Y: %f", TestEntity.getComponent<SpriteComponent>().Y());

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

    if (isDebugging)
    {

        TestEntity.getComponent<ColliderComponent>().Render();
        Capsule.getComponent<ColliderComponent>().Render();
        TestCollider.getComponent<ColliderComponent>().Render();
        Square.getComponent<ColliderComponent>().Render();
    }

    if (drawInWireMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUseProgram(Shader.getComponent<ShaderComponent>().GetProgram());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUniformMatrix4fv(Shader.getComponent<ShaderComponent>().GetUniformID("projection"), 1, GL_FALSE, cam.GetProjectionMatrix());
    glUniformMatrix4fv(Shader.getComponent<ShaderComponent>().GetUniformID("view"), 1, GL_FALSE, cam.GetVeiwMatrix());

    glUniformMatrix4fv(Shader.getComponent<ShaderComponent>().GetUniformID("model"), 1, GL_FALSE, background.getComponent<SpriteComponent>().GetModelMatrix());
    background.getComponent<SpriteComponent>().Render();
    glUniformMatrix4fv(Shader.getComponent<ShaderComponent>().GetUniformID("model"), 1, GL_FALSE, TestEntity.getComponent<SpriteComponent>().GetModelMatrix());

    TestEntity.getComponent<SpriteComponent>().Render();

    glUseProgram(0);
}
