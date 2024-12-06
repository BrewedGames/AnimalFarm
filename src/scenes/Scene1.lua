local background = manager:addEntity("Background")
local squirrel = manager:addEntity("Squirrel")
local skull = manager:addEntity("Skull")
local startButton = manager:addEntity("Startbutton")
local exitButton = manager:addEntity("Exitbutton")


-- load another lua script
req("player")


local squirrelSprite = squirrel:addSpriteComponent()
local audio  = player:addAudioComponent()
local squirrelCollider = squirrel:addColliderComponent()

local backgroundImage = background:addSpriteComponent()

local SkullModel = skull:addMeshComponent()
local angle = 0

local startbutton = startButton:addButtonComponent()
local exitbutton = exitButton:addButtonComponent()
startbutton:loadButton("./static/SampleButton.png", "./static/SampleButtonHovered.png", "./static/SampleButtonPressed.png", Vec2(600, 300), 0.3)
exitbutton:loadButton("./static/SampleButton.png", "./static/SampleButtonHovered.png", "./static/SampleButtonPressed.png", Vec2(600, 250), 0.3)

squirrel.isStatic = false

-- after switching scenes anything that was set in the previous scene will be deleted 
-- this includes globals and scripts that were set in the previous scene
-- however if you want to keep something between scenes you can use the constGlobal function
constGlobal("a_very_important_constant", 10)


-- if you want to load a Mesh with defualt values 
-- SkullModel:loadMesh("./src/meshes/Skull.obj", "./src/textures/skull_texture.jpg")
-- or you can specify the values
SkullModel:loadMesh("./src/meshes/Skull.obj", "./src/textures/skull_texture.jpg", Vec3(600, 200, 0), Vec3(50, 50, 50), Vec3(90, 0, 0))


local testVec = Vec3(1, 2, 3)
print("Created Vec3:", testVec.x, testVec.y, testVec.z)

squirrelSprite:loadSprite("./static/starstruck_color.png", 100, 100, Vec3(500, 500, 0))
backgroundImage:loadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350, 200, 0))

audio:setAudio("./src/audio/Celeste_Original_Soundtrack_First_Steps.mp3", true)
audio:setVolume(50) 
--audio:Play()


squirrelCollider:setTag("Squirrel")
squirrelCollider:addCircleCollider(squirrelSprite:getPos().x, squirrelSprite:getPos().y, 60)


local pos = squirrelSprite:getPos()
print("Player sprite position:", pos.x, pos.y, pos.z)


function on_event(event)
   -- print("Event received in Lua with type: " .. event.type .. ", key: " .. tostring(event.key)) -- Debugging
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["s"] then
            sprite:clearAnimation()
            sprite:playAnimation("WalkBack")
        end
        if key_states["d"] then
            sprite:clearAnimation()
            sprite:playAnimation("WalkRight")
        end
        if key_states["w"] then
            sprite:clearAnimation()
            sprite:playAnimation("WalkFront")
        end
        if key_states["a"] then
            sprite:clearAnimation()
            sprite:playAnimation("WalkLeft")
        end
        if key_states["x"] then
            startbutton:setButtonHovered(true)
        end
        if key_states["space"] then
            print(GameScene) -- Verify it prints a userdata reference, not nil or a function

           GameScene:changeScene("DemoScene")

        end
    elseif event.type == "keyup" then
        sprite:clearAnimation()
        startbutton:setButtonHovered(false)
        key_states[event.key] = false
    end
end



function update(delta_time)

    print("Start button State: ", startbutton:getButtonState())
    angle = angle + 30.0 * delta_time
    SkullModel:setRotation(Vec3(angle, angle, 0))

    playerCollider:setPos(Vec3(sprite:getPos().x, sprite:getPos().y, sprite:getPos().z))

    if playerCollider:isColliding(squirrelCollider) then
       -- print("Collision detected")
    end


    if key_states["s"] then
        sprite:setPos(Vec3(sprite:getPos().x, sprite:getPos().y - 1, sprite:getPos().z, 0))
    end
    if key_states["d"] then
        sprite:setPos(Vec3(sprite:getPos().x + 1, sprite:getPos().y, sprite:getPos().z, 0))
    end
    if key_states["w"] then
        sprite:setPos(Vec3(sprite:getPos().x, sprite:getPos().y + 1, sprite:getPos().z, 0))
    end
    if key_states["a"] then
        sprite:setPos(Vec3(sprite:getPos().x - 1, sprite:getPos().y, sprite:getPos().z, 0))
    end

        if controller_state[SDL_CONTROLLER_AXIS_LEFTX] > 8000 then
            print("Moving right with left stick")
        elseif controller_state[SDL_CONTROLLER_AXIS_LEFTX] < -8000 then
            print("Moving left with left stick")
        end
    
        if controller_state[SDL_CONTROLLER_AXIS_LEFTY] > 8000 then
            print("Moving down with left stick")
        elseif controller_state[SDL_CONTROLLER_AXIS_LEFTY] < -8000 then
            print("Moving up with left stick")
        end
    
        if controller_state[SDL_CONTROLLER_AXIS_RIGHTX] > 8000 then
            print("Looking right with right stick")
        elseif controller_state[SDL_CONTROLLER_AXIS_RIGHTX] < -8000 then
            print("Looking left with right stick")
        end
    
        if controller_state[SDL_CONTROLLER_AXIS_RIGHTY] > 8000 then
            print("Looking down with right stick")
        elseif controller_state[SDL_CONTROLLER_AXIS_RIGHTY] < -8000 then
            print("Looking up with right stick")
        end
    

        if controller_state[SDL_CONTROLLER_AXIS_TRIGGERLEFT] > 8000 then
            print("Left trigger pressed")
        end
        if controller_state[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] > 8000 then
            print("Right trigger pressed")
        end
end
---------------------------WARNING-------------------------------
-- Update Loops anything called after Update will not be called--
-----------------------------------------------------------------