#define SQUNICODE



#include "Bridge.h"
#include <filesystem>
#include "Scene.h"
#include "SceneTemplate.h"
#include <sol.hpp>
#include <cassert>
#include "SceneGenerator.h"
#include <SDL.h>

#include "ECS.h"
#include "ECSComponents.h"
using namespace std;

static Manager manager;

Bridge::Bridge() {}
Bridge::~Bridge() {}

void HelloWorld()
{
    printf("Hello from C++!\n");
}

int add(int a, int b)
{
    return a + b;
}

void print_message(const std::string &message)
{
    std::cout << message << std::endl;
}

void CreateGameObject(const std::string &name)
{

    static Entity *entity = new Entity();
    entity->setName(name);
    manager.addEntity(name);
    std::cout << "Created GameObject: " << name << std::endl;
}

// void

void Bridge::SetupBridge()
{

    std::string sceneFolder = "./src/scenes";

    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // Bind the Manager class and its methods
    lua.new_usertype<Manager>("Manager",
                              "addEntity", &Manager::addEntity,
                              "update", &Manager::Update,
                              "render", &Manager::Render,
                              "clearEntities", &Manager::clearEntities,
                              "refresh", &Manager::refresh);

    // Bind the Entity class and its methods
    lua.new_usertype<Entity>("Entity",
                             "getID", &Entity::getID,
                             "getName", &Entity::getName,
                             "setName", &Entity::setName,
                             "addComponent", [](Entity &entity, const std::string &componentType = "")
                             {
                             if (componentType.empty()) {
                                 // Handle adding components with no args
                             } else {
                                 // Handle adding components with args
                             } });

    // You can also bind specific components (like SpriteComponent)
    lua.new_usertype<SpriteComponent>("SpriteComponent",
                                      "loadSprite", &SpriteComponent::LoadSprite,
                                      "setPos", &SpriteComponent::setPos);

    // Create a global instance of Manager
    static Manager manager;
    lua["manager"] = &manager;

    for (const auto &entry : std::filesystem::directory_iterator(sceneFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lua")
        {
            std::string sceneName = entry.path().stem().string();
            std::string scenePath = entry.path().string();

            lua.set_function("add", &add);
            lua.set_function("print_message", &print_message);
            lua.set_function("CreateGameObject", &CreateGameObject);

            lua.script_file(scenePath.c_str());
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
}