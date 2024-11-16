local background = manager:addEntity("Background")
local squirrel = manager:addEntity("Squirrel")
local player = manager:addEntity("Player")


local squirrelSprite = squirrel:addSpriteComponent()
local sprite = player:addSpriteComponent()
local audio  = player:addAudioComponent()
local playerCollider = player:addColliderComponent()
local squirrelCollider = squirrel:addColliderComponent()

local backgroundImage = background:addSpriteComponent()


local testVec = Vec3(1, 2, 3)
print("Created Vec3:", testVec.x, testVec.y, testVec.z)

squirrelSprite:loadSprite("./static/starstruck_color.png", 100, 100, Vec3(500, 500, 0))
sprite:loadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(40, 100, 1), true, 32, 8, 100)
backgroundImage:loadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350, 200, 0))

audio:setAudio("./src/audio/Celeste_Original_Soundtrack_First_Steps.mp3", true)
audio:setVolume(50) 
audio:Play()

playerCollider:setTag("Player")
playerCollider:addCapsuleCollider(sprite:getPos().x, sprite:getPos().y, 50, 100)

squirrelCollider:setTag("Squirrel")
squirrelCollider:addCircleCollider(squirrelSprite:getPos().x, squirrelSprite:getPos().y, 60)


sprite:setAnimation("WalkBack", 0, 7, 100)
sprite:setAnimation("WalkLeft", 8, 15, 100)
sprite:setAnimation("WalkRight", 16, 23, 100)
sprite:setAnimation("WalkFront", 24, 31, 100)

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
    elseif event.type == "keyup" then
        sprite:clearAnimation()
        key_states[event.key] = false
    end
end


function update(delta_time)

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