#include "Bridge.h"
#include "ECS.h"
#include "ECSComponents.h"
#include <SDL.h>
#include <cstring>
#include <set>
#include <algorithm>
#include "math.h"

static Manager manager;
std::set<std::string> initial_globals;



void remove_new_globals(sol::state& lua, const std::set<std::string>& initial_globals) {
    sol::table globals = lua["_G"];
    sol::table persistent_globals = globals["_PERSISTENT_GLOBALS"];

    for (const auto& kvp : globals) {
        sol::object key = kvp.first;

        if (key.get_type() == sol::type::string) {
            std::string global_name = key.as<std::string>();

            // Skip initial globals and persistent globals
            if (initial_globals.find(global_name) == initial_globals.end() &&
                !persistent_globals[global_name].valid()) {
                globals[global_name] = sol::lua_nil;
                std::cout << "Removed: " << global_name << "\n";
            }
        }
    }
}

std::set<std::string> get_global_keys(sol::state& lua) {
    std::set<std::string> keys;
    sol::table globals = lua["_G"];

    for (const auto& kvp : globals) {
        sol::object key = kvp.first;
        if (key.get_type() == sol::type::string) {
            keys.insert(key.as<std::string>());
        }
    }
    return keys;
}


void sortEntitiesByY(std::vector<std::shared_ptr<Entity>>& dynamicEntities) {
    std::sort(dynamicEntities.begin(), dynamicEntities.end(),
              [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
                  if (!a->hasComponent<SpriteComponent>() || !b->hasComponent<SpriteComponent>()) {
                      return false;
                  }

                  float a_bottom = a->getComponent<SpriteComponent>().getPos().y - 
                                   (a->getComponent<SpriteComponent>().GetHeight() / 2);
                  float b_bottom = b->getComponent<SpriteComponent>().getPos().y - 
                                   (b->getComponent<SpriteComponent>().GetHeight() / 2);

                  return a_bottom > b_bottom; // Larger y values go last
              });
}

void Bridge::process_sdl_event(const SDL_Event &event)
{
    sol::table lua_event = lua.create_table();

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
    {
        lua_event["type"] = (event.type == SDL_KEYDOWN) ? "keydown" : "keyup";
        lua_event["key"] = get_key_name(event.key.keysym.sym);
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
    {
        lua_event["type"] = (event.type == SDL_MOUSEBUTTONDOWN) ? "mousedown" : "mouseup";
        lua_event["button"] = event.button.button;
        lua_event["x"] = event.button.x;
        lua_event["y"] = event.button.y;
    }

    if (lua_controller)
    {
        sol::table controller_state = lua["controller_state"];

        if (event.type == SDL_CONTROLLERAXISMOTION)
        {
            controller_state[event.caxis.axis] = event.caxis.value;
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
        {
            bool is_pressed = (event.type == SDL_CONTROLLERBUTTONDOWN);
            controller_state[event.cbutton.button] = is_pressed;
        }
    }

    if (lua["on_event"].valid())
    {
        lua["on_event"](lua_event);

    }
}

Bridge::Bridge() {}
Bridge::~Bridge() {}

void Bridge::SetupBridge()
{
    // sol::state lua;
    lua.open_libraries(sol::lib::base);
    register_key_constants();
    register_controller_constants();
    lua.set_function("req", [this](const char* script) {
        this->req(script);
    });
    lua["controller_state"] = lua.create_table();
    lua["key_states"] = lua.create_table();

    //ouioui
    lua["odd"] = lua.create_table_with(
        "sin", [](float x) { return std::sin(x); },
        "cos", [](float x) { return std::cos(x); },
        "tan", [](float x) { return std::tan(x); },
        "deg", [](float x) { return x * (180.0f / M_PI); },
        "rad", [](float x) { return x * (M_PI / 180.0f); },
        "pi", M_PI
    );


    lua["_PERSISTENT_GLOBALS"] = lua.create_table();


    lua.set_function("constGlobal", [this](const std::string& name, sol::object value) {
        sol::table globals = lua["_G"];
        sol::table persistent_globals = globals["_PERSISTENT_GLOBALS"];
        persistent_globals[name] = value;
        globals[name] = value; 
    });


    lua.new_usertype<MATH::Vec3>("Vec3", sol::constructors<MATH::Vec3(), MATH::Vec3(float, float, float)>(),

       
                                 "x", &MATH::Vec3::x, "y", &MATH::Vec3::y, "z", &MATH::Vec3::z, "r", &MATH::Vec3::x, "g", &MATH::Vec3::y, "b", &MATH::Vec3::z,
                                 sol::meta_function::to_string, [](const MATH::Vec3 &v)
                                 { return "Vec3(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"; }, sol::meta_function::index, [](MATH::Vec3 &v, int index)
                                 { 
            if (index < 0 || index > 2) return 0.0f; 
            return v[index]; }, sol::meta_function::new_index, [](MATH::Vec3 &v, int index, float value)
                                 { 
            if (index >= 0 && index <= 2) v[index] = value; },
                                 sol::meta_function::addition, [](const MATH::Vec3 &lhs, const MATH::Vec3 &rhs)
                                 { return lhs + rhs; }, sol::meta_function::subtraction, [](const MATH::Vec3 &lhs, const MATH::Vec3 &rhs)
                                 { return lhs - rhs; }, sol::meta_function::multiplication, [](const MATH::Vec3 &vec, float scalar)
                                 { return vec * scalar; }, sol::meta_function::division, [](const MATH::Vec3 &vec, float scalar)
                                 { return vec / scalar; });

    // Set Vec3 as a constructor function for Lua (Vec3(x, y, z))
    lua.set_function("Vec3", [](float x, float y, float z)
                     { return MATH::Vec3(x, y, z); });

    // Bind other classes (Manager, Entity, SpriteComponent)
    lua.new_usertype<Manager>("Manager",
                              //"addEntity", [this](Manager& manager, const std::string& name) -> Entity& {
                              // return *AddLuaEntity(name);  // Create and store entity
                              //},

                              "addEntity", [](Manager &manager, const std::string &name) -> Entity &
                              { return manager.addEntity(name); }, "update", &Manager::Update, "render", &Manager::Render, "clearEntities", &Manager::clearEntities, "refresh", &Manager::refresh, "removeEntity", &Manager::removeEntity);

    lua.new_usertype<Entity>("Entity",
    "isStatic", &Entity::isStatic,
    "getID", &Entity::getID,
    "getName", &Entity::getName,
    "setName", &Entity::setName,
    "addSpriteComponent", [](Entity &entity) -> SpriteComponent & { 
    return entity.addComponent<SpriteComponent>(); 
    }, "getSpriteComponent",
     &Entity::getComponent<SpriteComponent>, 
     "addAudioComponent", [](Entity &entity) -> AudioComponent &
    { return entity.addComponent<AudioComponent>(); 
    }, "getAudioComponent",
     &Entity::getComponent<AudioComponent>,
    "addColliderComponent", [](Entity& entity) -> ColliderComponent& {
        return entity.addComponent<ColliderComponent>();
    },
    "getColliderComponent", &Entity::getComponent<ColliderComponent>,
    "addMeshComponent", [](Entity& entity) -> MeshComponent& {
    return entity.addComponent<MeshComponent>();
    },
    "getMeshComponent", &Entity::getComponent<MeshComponent>

                             //"addShaderComponent", [](Entity& entity) -> ShaderComponent& {
                             //    return entity.addComponent<ShaderComponent>();
                             //},
                             //"getShaderComponent", &Entity::getComponent<ShaderComponent>

    );



    lua.new_usertype<MeshComponent>("MeshComponent",
     "loadMesh", sol::overload(

        [](MeshComponent &mesh, const char *mesh_filename, const char *texture_filename, MATH::Vec3 pos, MATH::Vec3 scale, MATH::Vec3 rotation)
        {
            mesh.loadMesh(mesh_filename, texture_filename, pos, scale, rotation);
        },

        [](MeshComponent &mesh, const char *mesh_filename, const char *texture_filename)
        {
            mesh.loadMesh(mesh_filename, texture_filename);
        }),
        "setPos", &MeshComponent::setPos,
        "getPos", &MeshComponent::pos,
        "setRotation", &MeshComponent::setRotation,
        "getRotation", &MeshComponent::rotation,
        "setScale", &MeshComponent::setScale,
        "getScale", &MeshComponent::scale,
        "getTextureID", &MeshComponent::getTextureID,
        "getImageWidth", &MeshComponent::getImageWidth,
        "getImageHeight", &MeshComponent::getImageHeight
    );

    lua.new_usertype<SpriteComponent>("SpriteComponent",
        "loadSprite", sol::overload(

        [](SpriteComponent &sprite, const char *filename, float width, float height, MATH::Vec3 pos)
        {
            sprite.LoadSprite(filename, width, height, pos);
        },

        [](SpriteComponent &sprite, const char *filename, float width, float height, MATH::Vec3 pos, bool isAnimated, int totalFrames, int framesPerRow, int speed)
        {
            sprite.LoadSprite(filename, width, height, pos, isAnimated, totalFrames, framesPerRow, speed, Camera());
        }),
        "setPos", &SpriteComponent::setPos,
        "getPos", &SpriteComponent::getPos,
        "clearAnimation", &SpriteComponent::ClearAnimation,
        "playAnimation", [](SpriteComponent &sprite, const char *animationName) {
        sprite.PlayAnimation(animationName);
        },
        "setAnimation", sol::overload( // test if this works
                            &SpriteComponent::SetAnimation, [this](SpriteComponent &sprite, const char * name, int startFrame, int endFrame, int speed)
                            { sprite.SetAnimation(name, startFrame, endFrame, speed); }));



    lua.new_usertype<AudioComponent>("AudioComponent",
        "setAudio", sol::overload([this](AudioComponent &audio, const char *filename, bool isMusic)
         { audio.setAudio(filename, isMusic); }, [this](AudioComponent &audio, const char *filename)
         { audio.setAudio(filename); }),
        "setVolume", sol::overload([this](AudioComponent &audio, float volume)
        { audio.SetVolume(volume); }),
        "Play", &AudioComponent::Play,
        "Stop", &AudioComponent::Stop,
        "Pause", &AudioComponent::Pause,
        "Resume", &AudioComponent::Resume
    );

    lua["ColliderType"] = lua.create_table_with(
        "None", 0,
        "Circle", 1,
        "AABB", 2,
        "Capsule", 3,
        "Polygon", 4
    );

    lua.new_usertype<ColliderComponent>("ColliderComponent",
        "getTag", &ColliderComponent::getTag,
        "setTag", &ColliderComponent::setTag,
        "getPos", &ColliderComponent::getPos,
        "setPos", &ColliderComponent::setPos,
        "addCircleCollider", &ColliderComponent::AddCircleCollider,
        "addRectCollider", &ColliderComponent::AddAABBCollider,
        "addCapsuleCollider", &ColliderComponent::AddCapsuleCollider,
        "addPolygonCollider", &ColliderComponent::AddPolygonCollider,
        "isColliding", &ColliderComponent::isColliding,
        "getColliderType", &ColliderComponent::getColliderType
       // "isCollidingWithTag", &ColliderComponent::isCollidingWithTag
    );

        lua.new_usertype<GameScene>("GameScene",
        sol::constructors<GameScene()>(),
        "getName", &GameScene::name,
        "getSceneNumber", &GameScene::scenenum,
        "changeScene", sol::overload(
            [this](GameScene &scn, const char *name) {
                std::cout << "C++: changeScene called with name: " << name << std::endl;


                manager.clearEntities();
                manager.refresh();
                if(lua["on_event"].valid()){
                    lua["on_event"] = sol::nil;
                }
                if(lua["update"].valid()){
                    lua["update"] = sol::nil;
                }

                lua["manager"] = &manager;
                lua["GameScene"] = &scn;

                remove_new_globals(lua, initial_globals);

                lua.script_file(("./src/scenes/" + std::string(name) + ".lua").c_str());
            }
           //[this](GameScene &scn, int num) {
           //    std::cout << "C++: changeScene called with scene number: " << num << std::endl;
           //     manager.clearEntities();
           //     manager.refresh();
           //     if(lua["on_event"].valid()){
           //         lua["on_event"] = sol::nil;
           //     }
           //     if(lua["update"].valid()){
           //         lua["update"] = sol::nil;
           //     }
           //     std::cout << "Current scene: " << scenes[num].name << std::endl;
           //     lua["manager"] = &manager;
           //     lua["GameScene"] = &scn;
           //     lua.script_file(("./src/scenes/" + std::string(scenes[num].name) + ".lua").c_str());
           //}
        ));


    lua["manager"] = &manager;
    lua["GameScene"] = &current_scene;



    
    initial_globals = get_global_keys(lua);
    // Load Lua scenes
    ini.SetUnicode();
	ini.LoadFile("config.ini");
	current_scene.name = ini.GetValue("DefaultScene", "scene");
    current_scene.scenenum = 0;
    scenes.push_back(current_scene);

    const char * sceneFolder = "./src/scenes";
    lua.script_file((std::string(sceneFolder) + "/" + current_scene.name + ".lua").c_str());

    int i = 0;

    for (const auto &entry : std::filesystem::directory_iterator(sceneFolder))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".lua")
        {
            if(entry.path().filename().string() == (std::string(current_scene.name) + ".lua")){
                continue;
            } else {
                i++;
                int num = i; 
                scenes.push_back(GameScene(entry.path().filename().string().c_str(), num));
                std::cout << "Loaded scene: " << entry.path().filename().string() << " (" << num << ")" << std::endl;
            }


        }
    }



}


void Bridge::Update(float deltaTime)
{
    poll_controller_state();
    if (lua["update"].valid())
    {
        lua["update"](deltaTime);
    }

    for (auto &entity : manager.getEntities())
    {
        entity->hasComponent<SpriteComponent>() ? entity->getComponent<SpriteComponent>().Update(deltaTime) : 0;
        entity->hasComponent<ColliderComponent>() ? entity->getComponent<ColliderComponent>().Update(deltaTime) : 0;
        entity->hasComponent<MeshComponent>() ? entity->getComponent<MeshComponent>().Update(deltaTime) : 0;
    }

}

void Bridge::Render() const {
    std::vector<std::shared_ptr<Entity>> dynamicEntities;
    std::vector<std::shared_ptr<Entity>> staticEntities;

    for (const auto& entity : manager.getEntities()) {
        if (entity->isStatic) {
            staticEntities.push_back(entity);
        } else {
            dynamicEntities.push_back(entity);
        }
    }


    if (!dynamicEntities.empty()) {
        sortEntitiesByY(dynamicEntities);
    }


    for (const auto& entity : staticEntities) {
        entity->hasComponent<SpriteComponent>() ? entity->getComponent<SpriteComponent>().Render() : void();
        entity->hasComponent<ColliderComponent>() ? entity->getComponent<ColliderComponent>().Render() : void();
        entity->hasComponent<MeshComponent>() ? entity->getComponent<MeshComponent>().Render() : void();
    }


    for (const auto& entity : dynamicEntities) {
        entity->hasComponent<SpriteComponent>() ? entity->getComponent<SpriteComponent>().Render() : void();
        entity->hasComponent<ColliderComponent>() ? entity->getComponent<ColliderComponent>().Render() : void();
        entity->hasComponent<MeshComponent>() ? entity->getComponent<MeshComponent>().Render() : void();
    }
}

void Bridge::req(const char* script) {
    std::string scriptPath = script;

    if (scriptPath.find(".lua") == std::string::npos) {
        scriptPath += ".lua";
    }
    if (scriptPath.find('/') == std::string::npos) {
        scriptPath = "./src/scripts/" + scriptPath;
    }
    lua.script_file(scriptPath.c_str());
}


void Bridge::CreateGameObject(const std::string &name)
{
    static Entity *entity = new Entity();
    entity->setName(name);
    manager.addEntity(name);
    std::cout << "Created GameObject: " << name << std::endl;
}

void Bridge::poll_controller_state()
{
    if (!lua_controller)
        return;

    sol::table controller_state = lua["controller_state"];

    // Poll each axis directly
    controller_state["left_stick_x"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_LEFTX);
    controller_state["left_stick_y"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_LEFTY);
    controller_state["right_stick_x"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_RIGHTX);
    controller_state["right_stick_y"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_RIGHTY);
    controller_state["left_trigger"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    controller_state["right_trigger"] = SDL_GameControllerGetAxis(lua_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

    // Poll each button directly
    controller_state["button_a"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_A);
    controller_state["button_b"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_B);
    controller_state["button_x"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_X);
    controller_state["button_y"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_Y);
    controller_state["button_back"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_BACK);
    controller_state["button_guide"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_GUIDE);
    controller_state["button_start"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_START);
    controller_state["button_leftstick"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_LEFTSTICK);
    controller_state["button_rightstick"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK);
    controller_state["button_leftshoulder"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
    controller_state["button_rightshoulder"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    controller_state["dpad_up"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_DPAD_UP);
    controller_state["dpad_down"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
    controller_state["dpad_left"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    controller_state["dpad_right"] = SDL_GameControllerGetButton(lua_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
}

void Bridge::register_controller_constants()
{
    // axis constants
    lua["SDL_CONTROLLER_AXIS_LEFTX"] = SDL_CONTROLLER_AXIS_LEFTX;
    lua["SDL_CONTROLLER_AXIS_LEFTY"] = SDL_CONTROLLER_AXIS_LEFTY;
    lua["SDL_CONTROLLER_AXIS_RIGHTX"] = SDL_CONTROLLER_AXIS_RIGHTX;
    lua["SDL_CONTROLLER_AXIS_RIGHTY"] = SDL_CONTROLLER_AXIS_RIGHTY;
    lua["SDL_CONTROLLER_AXIS_TRIGGERLEFT"] = SDL_CONTROLLER_AXIS_TRIGGERLEFT;
    lua["SDL_CONTROLLER_AXIS_TRIGGERRIGHT"] = SDL_CONTROLLER_AXIS_TRIGGERRIGHT;

    // button constants
    lua["SDL_CONTROLLER_BUTTON_A"] = SDL_CONTROLLER_BUTTON_A;
    lua["SDL_CONTROLLER_BUTTON_B"] = SDL_CONTROLLER_BUTTON_B;
    lua["SDL_CONTROLLER_BUTTON_X"] = SDL_CONTROLLER_BUTTON_X;
    lua["SDL_CONTROLLER_BUTTON_Y"] = SDL_CONTROLLER_BUTTON_Y;
    lua["SDL_CONTROLLER_BUTTON_BACK"] = SDL_CONTROLLER_BUTTON_BACK;
    lua["SDL_CONTROLLER_BUTTON_GUIDE"] = SDL_CONTROLLER_BUTTON_GUIDE;
    lua["SDL_CONTROLLER_BUTTON_START"] = SDL_CONTROLLER_BUTTON_START;
    lua["SDL_CONTROLLER_BUTTON_LEFTSTICK"] = SDL_CONTROLLER_BUTTON_LEFTSTICK;
    lua["SDL_CONTROLLER_BUTTON_RIGHTSTICK"] = SDL_CONTROLLER_BUTTON_RIGHTSTICK;
    lua["SDL_CONTROLLER_BUTTON_LEFTSHOULDER"] = SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
    lua["SDL_CONTROLLER_BUTTON_RIGHTSHOULDER"] = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
    lua["SDL_CONTROLLER_BUTTON_DPAD_UP"] = SDL_CONTROLLER_BUTTON_DPAD_UP;
    lua["SDL_CONTROLLER_BUTTON_DPAD_DOWN"] = SDL_CONTROLLER_BUTTON_DPAD_DOWN;
    lua["SDL_CONTROLLER_BUTTON_DPAD_LEFT"] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
    lua["SDL_CONTROLLER_BUTTON_DPAD_RIGHT"] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
}

void Bridge::bind_controller(SDL_GameController *controller)
{

    lua_controller = controller;

    sol::table controller_state = lua.create_table();
    lua["controller_state"] = controller_state;

    controller_state["left_stick_x"] = 0;
    controller_state["left_stick_y"] = 0;
    controller_state["right_stick_x"] = 0;
    controller_state["right_stick_y"] = 0;
    controller_state["left_trigger"] = 0;
    controller_state["right_trigger"] = 0;

    controller_state["button_a"] = false;
    controller_state["button_b"] = false;
    controller_state["button_x"] = false;
    controller_state["button_y"] = false;
    controller_state["button_back"] = false;
    controller_state["button_guide"] = false;
    controller_state["button_start"] = false;
    controller_state["button_leftstick"] = false;
    controller_state["button_rightstick"] = false;
    controller_state["button_leftshoulder"] = false;
    controller_state["button_rightshoulder"] = false;
    controller_state["dpad_up"] = false;
    controller_state["dpad_down"] = false;
    controller_state["dpad_left"] = false;
    controller_state["dpad_right"] = false;
}

std::string Bridge::get_key_name(SDL_Keycode keycode)
{
    switch (keycode)
    {
    case SDLK_a:
        return "a";
    case SDLK_b:
        return "b";
    case SDLK_c:
        return "c";
    case SDLK_d:
        return "d";
    case SDLK_e:
        return "e";
    case SDLK_f:
        return "f";
    case SDLK_g:
        return "g";
    case SDLK_h:
        return "h";
    case SDLK_i:
        return "i";
    case SDLK_j:
        return "j";
    case SDLK_k:
        return "k";
    case SDLK_l:
        return "l";
    case SDLK_m:
        return "m";
    case SDLK_n:
        return "n";
    case SDLK_o:
        return "o";
    case SDLK_p:
        return "p";
    case SDLK_q:
        return "q";
    case SDLK_r:
        return "r";
    case SDLK_s:
        return "s";
    case SDLK_t:
        return "t";
    case SDLK_u:
        return "u";
    case SDLK_v:
        return "v";
    case SDLK_w:
        return "w";
    case SDLK_x:
        return "x";
    case SDLK_y:
        return "y";
    case SDLK_z:
        return "z";
    case SDLK_1:
        return "1";
    case SDLK_2:
        return "2";
    case SDLK_3:
        return "3";
    case SDLK_4:
        return "4";
    case SDLK_5:
        return "5";
    case SDLK_6:
        return "6";
    case SDLK_7:
        return "7";
    case SDLK_8:
        return "8";
    case SDLK_9:
        return "9";
    case SDLK_0:
        return "0";
    case SDLK_RETURN:
        return "return";
    case SDLK_ESCAPE:
        return "escape";
    case SDLK_BACKSPACE:
        return "backspace";
    case SDLK_TAB:
        return "tab";
    case SDLK_SPACE:
        return "space";
    case SDLK_MINUS:
        return "minus";
    case SDLK_EQUALS:
        return "equals";
    case SDLK_LEFTBRACKET:
        return "leftbracket";
    case SDLK_RIGHTBRACKET:
        return "rightbracket";
    case SDLK_BACKSLASH:
        return "backslash";
    case SDLK_SEMICOLON:
        return "semicolon";
    case SDLK_COMMA:
        return "comma";
    case SDLK_PERIOD:
        return "period";
    case SDLK_SLASH:
        return "slash";
    case SDLK_CAPSLOCK:
        return "capslock";
    case SDLK_F1:
        return "f1";
    case SDLK_F2:
        return "f2";
    case SDLK_F3:
        return "f3";
    case SDLK_F4:
        return "f4";
    case SDLK_F5:
        return "f5";
    case SDLK_F6:
        return "f6";
    case SDLK_F7:
        return "f7";
    case SDLK_F8:
        return "f8";
    case SDLK_F9:
        return "f9";
    case SDLK_F10:
        return "f10";
    case SDLK_F11:
        return "f11";
    case SDLK_F12:
        return "f12";
    case SDLK_PRINTSCREEN:
        return "printscreen";
    case SDLK_SCROLLLOCK:
        return "scrolllock";
    case SDLK_PAUSE:
        return "pause";
    case SDLK_INSERT:
        return "insert";
    case SDLK_HOME:
        return "home";
    case SDLK_PAGEUP:
        return "pageup";
    case SDLK_DELETE:
        return "delete";
    case SDLK_END:
        return "end";
    case SDLK_PAGEDOWN:
        return "pagedown";
    case SDLK_RIGHT:
        return "right";
    case SDLK_LEFT:
        return "left";
    case SDLK_DOWN:
        return "down";
    case SDLK_UP:
        return "up";
    case SDLK_LCTRL:
        return "lctrl";
    case SDLK_LSHIFT:
        return "lshift";
    case SDLK_LALT:
        return "lalt";
    case SDLK_LGUI:
        return "lgui";
    case SDLK_RCTRL:
        return "rctrl";
    case SDLK_RSHIFT:
        return "rshift";
    case SDLK_RALT:
        return "ralt";
    case SDLK_RGUI:
        return "rgui";
    default:
        return "unknown";
    }
}

void Bridge::register_key_constants()
{
    // Alphabet keys
    lua["SDLK_a"] = SDLK_a;
    lua["SDLK_b"] = SDLK_b;
    lua["SDLK_c"] = SDLK_c;
    lua["SDLK_d"] = SDLK_d;
    lua["SDLK_e"] = SDLK_e;
    lua["SDLK_f"] = SDLK_f;
    lua["SDLK_g"] = SDLK_g;
    lua["SDLK_h"] = SDLK_h;
    lua["SDLK_i"] = SDLK_i;
    lua["SDLK_j"] = SDLK_j;
    lua["SDLK_k"] = SDLK_k;
    lua["SDLK_l"] = SDLK_l;
    lua["SDLK_m"] = SDLK_m;
    lua["SDLK_n"] = SDLK_n;
    lua["SDLK_o"] = SDLK_o;
    lua["SDLK_p"] = SDLK_p;
    lua["SDLK_q"] = SDLK_q;
    lua["SDLK_r"] = SDLK_r;
    lua["SDLK_s"] = SDLK_s;
    lua["SDLK_t"] = SDLK_t;
    lua["SDLK_u"] = SDLK_u;
    lua["SDLK_v"] = SDLK_v;
    lua["SDLK_w"] = SDLK_w;
    lua["SDLK_x"] = SDLK_x;
    lua["SDLK_y"] = SDLK_y;
    lua["SDLK_z"] = SDLK_z;

    // Number keys
    lua["SDLK_1"] = SDLK_1;
    lua["SDLK_2"] = SDLK_2;
    lua["SDLK_3"] = SDLK_3;
    lua["SDLK_4"] = SDLK_4;
    lua["SDLK_5"] = SDLK_5;
    lua["SDLK_6"] = SDLK_6;
    lua["SDLK_7"] = SDLK_7;
    lua["SDLK_8"] = SDLK_8;
    lua["SDLK_9"] = SDLK_9;
    lua["SDLK_0"] = SDLK_0;

    // Function keys
    lua["SDLK_F1"] = SDLK_F1;
    lua["SDLK_F2"] = SDLK_F2;
    lua["SDLK_F3"] = SDLK_F3;
    lua["SDLK_F4"] = SDLK_F4;
    lua["SDLK_F5"] = SDLK_F5;
    lua["SDLK_F6"] = SDLK_F6;
    lua["SDLK_F7"] = SDLK_F7;
    lua["SDLK_F8"] = SDLK_F8;
    lua["SDLK_F9"] = SDLK_F9;
    lua["SDLK_F10"] = SDLK_F10;
    lua["SDLK_F11"] = SDLK_F11;
    lua["SDLK_F12"] = SDLK_F12;

    // Modifier keys
    lua["SDLK_LCTRL"] = SDLK_LCTRL;
    lua["SDLK_RCTRL"] = SDLK_RCTRL;
    lua["SDLK_LSHIFT"] = SDLK_LSHIFT;
    lua["SDLK_RSHIFT"] = SDLK_RSHIFT;
    lua["SDLK_LALT"] = SDLK_LALT;
    lua["SDLK_RALT"] = SDLK_RALT;
    lua["SDLK_LGUI"] = SDLK_LGUI;
    lua["SDLK_RGUI"] = SDLK_RGUI;

    // Arrow keys
    lua["SDLK_UP"] = SDLK_UP;
    lua["SDLK_DOWN"] = SDLK_DOWN;
    lua["SDLK_LEFT"] = SDLK_LEFT;
    lua["SDLK_RIGHT"] = SDLK_RIGHT;

    // Special keys
    lua["SDLK_RETURN"] = SDLK_RETURN;
    lua["SDLK_ESCAPE"] = SDLK_ESCAPE;
    lua["SDLK_BACKSPACE"] = SDLK_BACKSPACE;
    lua["SDLK_TAB"] = SDLK_TAB;
    lua["SDLK_SPACE"] = SDLK_SPACE;

    // Symbols and punctuation
    lua["SDLK_MINUS"] = SDLK_MINUS;
    lua["SDLK_EQUALS"] = SDLK_EQUALS;
    lua["SDLK_LEFTBRACKET"] = SDLK_LEFTBRACKET;
    lua["SDLK_RIGHTBRACKET"] = SDLK_RIGHTBRACKET;
    lua["SDLK_BACKSLASH"] = SDLK_BACKSLASH;
    lua["SDLK_SEMICOLON"] = SDLK_SEMICOLON;
    lua["SDLK_COMMA"] = SDLK_COMMA;
    lua["SDLK_PERIOD"] = SDLK_PERIOD;
    lua["SDLK_SLASH"] = SDLK_SLASH;

    // Lock keys
    lua["SDLK_CAPSLOCK"] = SDLK_CAPSLOCK;
    lua["SDLK_SCROLLLOCK"] = SDLK_SCROLLLOCK;
    lua["SDLK_NUMLOCKCLEAR"] = SDLK_NUMLOCKCLEAR;

    // Editing keys
    lua["SDLK_PRINTSCREEN"] = SDLK_PRINTSCREEN;
    lua["SDLK_PAUSE"] = SDLK_PAUSE;
    lua["SDLK_INSERT"] = SDLK_INSERT;
    lua["SDLK_HOME"] = SDLK_HOME;
    lua["SDLK_PAGEUP"] = SDLK_PAGEUP;
    lua["SDLK_DELETE"] = SDLK_DELETE;
    lua["SDLK_END"] = SDLK_END;
    lua["SDLK_PAGEDOWN"] = SDLK_PAGEDOWN;

    // Keypad keys
    lua["SDLK_KP_DIVIDE"] = SDLK_KP_DIVIDE;
    lua["SDLK_KP_MULTIPLY"] = SDLK_KP_MULTIPLY;
    lua["SDLK_KP_MINUS"] = SDLK_KP_MINUS;
    lua["SDLK_KP_PLUS"] = SDLK_KP_PLUS;
    lua["SDLK_KP_ENTER"] = SDLK_KP_ENTER;
    lua["SDLK_KP_1"] = SDLK_KP_1;
    lua["SDLK_KP_2"] = SDLK_KP_2;
    lua["SDLK_KP_3"] = SDLK_KP_3;
    lua["SDLK_KP_4"] = SDLK_KP_4;
    lua["SDLK_KP_5"] = SDLK_KP_5;
    lua["SDLK_KP_6"] = SDLK_KP_6;
    lua["SDLK_KP_7"] = SDLK_KP_7;
    lua["SDLK_KP_8"] = SDLK_KP_8;
    lua["SDLK_KP_9"] = SDLK_KP_9;
    lua["SDLK_KP_0"] = SDLK_KP_0;
    lua["SDLK_KP_PERIOD"] = SDLK_KP_PERIOD;

    // Additional media and utility keys
    lua["SDLK_AUDIONEXT"] = SDLK_AUDIONEXT;
    lua["SDLK_AUDIOPREV"] = SDLK_AUDIOPREV;
    lua["SDLK_AUDIOSTOP"] = SDLK_AUDIOSTOP;
    lua["SDLK_AUDIOPLAY"] = SDLK_AUDIOPLAY;
    lua["SDLK_AUDIOMUTE"] = SDLK_AUDIOMUTE;
    lua["SDLK_VOLUMEUP"] = SDLK_VOLUMEUP;
    lua["SDLK_VOLUMEDOWN"] = SDLK_VOLUMEDOWN;
    lua["SDLK_BRIGHTNESSDOWN"] = SDLK_BRIGHTNESSDOWN;
    lua["SDLK_BRIGHTNESSUP"] = SDLK_BRIGHTNESSUP;
    lua["SDLK_EJECT"] = SDLK_EJECT;
    lua["SDLK_SLEEP"] = SDLK_SLEEP;
}
