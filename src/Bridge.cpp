#define SQUNICODE


/*

    hello future zoe 
    what we need to do is for scenefiles in scenes folder get the names of each file without the extension and out them in a array
    make sure that the DefualtScene as specified in config.ini is at the front of the array then we use that array to load them in
    a scene at the start of the game 

    technically we only have on Scene and it chnages based on what lua script is currently loaded 

    so the scenes oncreate for example would run the oncreate for the currently active lua file 
    that way we have dynamic scenes without this mess 


*/



#include "Bridge.h"
#include <filesystem>
#include "Scene.h"
#include "SceneTemplate.h"
#include <sol.hpp>
#include <cassert>
#include "SceneGenerator.h"
#include <SDL.h>
using namespace std;



Bridge::Bridge() {}
Bridge::~Bridge() {}

void HelloWorld()
{
    printf("Hello from C++!\n");
}

void Bridge::SetupBridge()
{

    std::string sceneFolder = "./src/scenes";

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    for (const auto &entry : std::filesystem::directory_iterator(sceneFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lua")
        {
            std::string sceneName = entry.path().stem().string();
            std::string scenePath = entry.path().string();

            // Define the class
           //class DynamicScene : public Scene
           //{
           //public:
           //    bool OnCreate() override { return true; }
           //    void OnDestroy() override {}
           //    void Update(const float deltaTime) override {}
           //    void Render() const override {}
           //    void HandleEvents(const SDL_Event &sdlEvent) override {}
           //};

           //// Register the class with Lua
           //lua.new_usertype<DynamicScene>(sceneName,
           //                               "OnCreate", &DynamicScene::OnCreate,
           //                               "Update", &DynamicScene::Update,
           //                               "Render", &DynamicScene::Render,
           //                               "OnDestroy", &DynamicScene::OnDestroy,
           //                               "HandleEvents", &DynamicScene::HandleEvents);

           //// Load and execute the Lua script
            lua.script_file(scenePath.c_str());

            // Create an instance and set it in Lua
           //auto sceneInstance = std::make_shared<DynamicScene>();
           //lua[sceneName] = sceneInstance;
           //sceneInstance->OnCreate();
        }
    }

    std::string scriptFolder = "./src/scripts";
    for (const auto &entry : std::filesystem::directory_iterator(scriptFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lua")
        {
            sol::state lua;
            lua.open_libraries(sol::lib::base);
            std::cout << "Found .lua file: " << entry.path().filename() << std::endl;
            std::string scriptPath = "./src/scripts/" + entry.path().filename().string();
            lua.script_file(scriptPath.c_str());
        }
    }

    // Available flags:
    // NONE
    // IO
    // BLOB
    // MATH
    // SYSTEM
    // STRING
    // ALL

    //TestEntity.addComponent<TestComponent>();
    //TestEntity.getComponent<TestComponent>().TestFunction();
}