-- Newton-Raphson square root function
local function sqrt(number)
    if number <= 0 then return 0 end -- Ensure the number is positive
    local guess = number / 2
    for _ = 1, 10 do -- Perform 10 iterations
        guess = (guess + number / guess) / 2
    end
    return guess
end

-- Pythagorean theorem to calculate distance
local function calculateDistance(dx, dy)
    return sqrt(dx * dx + dy * dy) -- Distance between two points
end

-- Move entities toward a direction
local function moveEntity(sprite, collider, dx, dy, speed, delta_time)
    local pos = sprite:getPos() -- Position of the sprite
    local distance = calculateDistance(dx, dy) -- Calculate distance
    local directionX, directionY = dx / distance, dy / distance -- Normalize direction
    local newX = pos.x + directionX * speed * delta_time -- New x position
    local newY = pos.y + directionY * speed * delta_time -- New y position

    -- Update sprite and collider positions
    sprite:setPos(Vec3(newX, newY, pos.z))
    collider:setPos(Vec3(newX, newY, pos.z))
end

-- Initialize entities with sprite and collider
local function initEntity(name, spritePath, width, height, position, colliderRad, tag)
    local entity = manager:addEntity(name) -- Add new entity
    local sprite = entity:addSpriteComponent() -- Add sprite component
    local collider = entity:addColliderComponent() -- Add collider component
    sprite:loadSprite(spritePath, width, height, position) -- Load the sprite
    
    if colliderRad then -- If collider radius is provided
        collider:addCircleCollider(position.x, position.y, colliderRad) -- Add circle collider
        if tag then 
            collider:setTag(tag) -- Set collider tag if provided
        end 
    end

    return entity, sprite, collider -- Return the entity components
end

-- Initialize background entity and its image
local background, backgroundSprite = initEntity("Background", "./static/chicken_map.jpg", 1280, 720, Vec3(640, 360, 0))

-- Initialize chicken entity with image and collider
local chicken, chickenSprite, chickenCollider = initEntity("chicken", "./static/chicken.png", 500, 500, Vec3(640, 360, 0), 60, "chicken")

-- Initialize slipper entity with image, collider, and off-screen position
local slipper, slipperSprite, slipperCollider = initEntity("slipper", "./static/chicken_slipper_attack.png", 400, 400, Vec3(-1000, -1000, 0), 10, "slipper")


-- Load and initialize the existing player file
req("player")
local playerData = initPlayer()
local playerSprite, playerCollider = playerData.sprite, playerData.collider

-- Variables for slipper behavior
local slipperSpeed, slipperTimer, slipperCooldown = 300, 0, 5 -- Speed, timer, and cooldown for slipper
local isSlipperActive = false -- Check if slipper is active
local slipperDirection = Vec3(0, 0, 0) -- Direction slipper is thrown
local offScreen = Vec3(-1000, -1000, 0) -- Off-screen position to hide slipper when inactive

-- Function to handle slipper throwing
local function throwSlipper(delta_time)
    if isSlipperActive then -- If slipper is active
        local pos = slipperSprite:getPos() -- Get current position
        pos.x = pos.x + slipperDirection.x * slipperSpeed * delta_time -- Update x position
        pos.y = pos.y + slipperDirection.y * slipperSpeed * delta_time -- Update y position
        slipperSprite:setPos(pos) -- Update slipper sprite position
        slipperCollider:setPos(pos) -- Update slipper collider position

        -- Check collision with player
        if playerCollision(playerCollider, slipperCollider) then
            print("Player lost 10 health")
            isSlipperActive = false -- Deactivate slipper
            slipperSprite:setPos(offScreen) -- Move slipper off-screen
            slipperCollider:setPos(offScreen) -- Move collider off-screen
        end

        -- Decrease slipper timer
        slipperTimer = slipperTimer - delta_time
        if slipperTimer <= 0 then -- If timer runs out
            isSlipperActive = false -- Deactivate slipper
            slipperSprite:setPos(offScreen) -- Move slipper off-screen
            slipperCollider:setPos(offScreen) -- Move collider off-screen
        end
    elseif slipperTimer <= 0 then -- If slipper is not active and cooldown is over
        local chickenPos, playerPos = chickenSprite:getPos(), playerSprite:getPos() -- Get positions
        local dx, dy = playerPos.x - chickenPos.x, playerPos.y - chickenPos.y -- Calculate direction
        local distance = calculateDistance(dx, dy) -- Calculate distance
        slipperDirection = Vec3(dx / distance, dy / distance, 0) -- Normalize direction
        slipperSprite:setPos(chickenPos) -- Move slipper to chicken's position
        slipperCollider:setPos(chickenPos) -- Update collider position

        isSlipperActive = true -- Activate slipper
        slipperTimer = slipperCooldown -- Reset slipper timer

        -- Optional: Change chicken sprite to attack sprite
        -- chickenSprite:loadSprite("./static/chicken_attack.png", 100, 120, chickenPos)
    else
        slipperTimer = slipperTimer - delta_time -- Decrease cooldown timer
    end
end

-- Update slipper when inactive
local function updateSlipper()
    if not isSlipperActive then -- If slipper is not active
        slipperSprite:setPos(offScreen) -- Move slipper off-screen
        slipperCollider:setPos(offScreen) -- Move collider off-screen
    end
end

-- Collision detection functions
function rectCollision(otherCollider, playerCollider)
    if otherCollider:isColliding(playerCollider) then
        return true
    end
    return false
end

function playerCollision(playerCollider, otherCollider)
    if playerCollider:isColliding(otherCollider) then
        return true
    end
    return false
end

-- Event handling function
function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        
        if key_states["down"] then setPlayerDown(true) end
        if key_states["right"] then setPlayerRight(true) end
        if key_states["up"] then setPlayerUp(true) end
        if key_states["left"] then setPlayerLeft(true) end
        if event.key == "z" then triggerDash() end

    elseif event.type == "keyup" then
        key_states[event.key] = false

        if event.key == "down" then setPlayerDown(false) end
        if event.key == "right" then setPlayerRight(false) end
        if event.key == "up" then setPlayerUp(false) end
        if event.key == "left" then setPlayerLeft(false) end
    end
    if event.type == "controllerbuttondown" then
        controller_state[event.button] = true
        if controller_state["button_a"] then triggerDash() end
        
    elseif event.type == "controllerbuttonup" then
        controller_state[event.button] = false
    end
end

-- Update function called every frame
function update(delta_time)

    
    if math.abs(controller_state["right_stick_x"]) > CONTROLLERTHRESHOLD or
        math.abs(controller_state["right_stick_y"]) > CONTROLLERTHRESHOLD then
        updateArrowPosition()
    end
    if controller_state["left_stick_x"] > CONTROLLERTHRESHOLD then
        setPlayerRight(true)
    elseif controller_state["left_stick_x"] < -CONTROLLERTHRESHOLD then
        setPlayerLeft(true)
    else
        setPlayerRight(false)
        setPlayerLeft(false)
    end
    if controller_state["left_stick_y"] > CONTROLLERTHRESHOLD then
        setPlayerDown(true)
    elseif controller_state["left_stick_y"] < -CONTROLLERTHRESHOLD then
        setPlayerUp(true)
    else
        setPlayerDown(false)
        setPlayerUp(false)
    end
    updatePlayer(delta_time)
    playerCollider:setPos(playerSprite:getPos()) -- Update player collider position

    throwSlipper(delta_time) -- Handle slipper throwing
    updateSlipper() -- Update slipper when inactive
end
