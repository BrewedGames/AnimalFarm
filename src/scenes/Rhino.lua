local backgroundMusic = manager:addEntity("BackgroundMusic")
local audio = backgroundMusic:addAudioComponent()
audio:setAudio("./src/audio/Venus.wav", true)
audio:setVolume(50)
audio:setLoop(true)
audio:Play()

local RhinoSpriteWidth = 190.89
local RhinoSpriteHeight = 165

-- searched up newton raphson sqrt bcs ye
local function sqrt(number)
    if number <= 0 then return 0 end -- make sure its positive
    local guess = number / 2
    for _ = 1, 10 do -- do 10 times
        guess = (guess + number / guess) / 2
    end
    return guess
end

-- pythagorean theorem
local function calculateDistance(dx, dy)
    return sqrt(dx * dx + dy * dy) -- the distance between two points
end

-- move entities towards a direction
local function moveEntity(sprite, collider, dx, dy, speed, delta_time, dampening)
    local pos = sprite:getPos() -- pos of sprite
    local distance = calculateDistance(dx, dy) -- calc distance
    local directionX, directionY = dx / distance, dy / distance -- normalize
    local newX = pos.x + directionX * speed * dampening * delta_time -- new x
    local newY = pos.y + directionY * speed * dampening * delta_time -- new y

    -- update sprite and collider positions
    sprite:setPos(Vec3(newX, newY, pos.z))
    collider:setPos(Vec3(newX, newY, pos.z))
end

-- initialize entities 
local function initEntity(name, spritePath, width, height, position,
                          colliderRad, tag)
    local entity = manager:addEntity(name) -- add new entity
    local sprite = entity:addSpriteComponent() -- add sprite to it
    local collider = entity:addColliderComponent() -- add collider to it
    sprite:loadSprite(spritePath, width, height, position) -- load the sprite

    -- if the radius of collider is provided
    if colliderRad then
        collider:addCircleCollider(position.x, position.y, colliderRad) -- add circle collider
        if tag then
            collider:setTag(tag) -- if tag is provided set tag
        end
    end

    return entity, sprite, collider -- return the entity+
end

-- entity with invisible rectangle collider (for bounds)
local function initRectCol(name, position, colliderWidth, colliderHeight, tag)
    local entity = manager:addEntity(name) -- add the entity
    local collider = entity:addColliderComponent() -- add collider

    -- add rectngle collider
    collider:addRectCollider(position.x, position.y, colliderWidth,
                             colliderHeight)

    if (tag) then -- if tag provided
        collider:setTag(tag) -- set a tag
    end

    return entity, collider -- return entity and collider
end

-- init background entity and its image
local background, backgroundSprite = initEntity("Background",
                                                "./static/rhena_map.jpg", 1280,
                                                720, Vec3(640, 360, 0))

-- init rhino entity w image and collider
local rhino, rhinoSprite, rhinoCollider = initEntity("rhino",
                                                     "./static/rhena.png",
                                                     RhinoSpriteWidth,
                                                     RhinoSpriteHeight,
                                                     Vec3(500, 500, 0), 60,
                                                     "rhino")

-- init sausage entity with image collider and where it is off screen
local sausage, sausageSprite, sausageCollider =
    initEntity("sausage", "./static/sausage.png", 20, 10, Vec3(-1000, -1000, 0),
               10, "sausage")

-- load and init the existing player file
req("player")
local playerData = initPlayer()
local playerSprite, playerCollider = playerData.sprite, playerData.collider

constGlobal("A_VERY_IMPORTANT_CONSTANT", 10) -- keeps a variable throughout scenes (like a inventory item or smth)

local chaseDistance, chaseSpeed = 300, 200 -- distance and speed to chase player
local dashDistance, dashSpeed, dashDamage, dashCooldown = 200, 900, 25, 7 -- stuff needed for rhino dash
local dashTimer = 0 -- timer for rhino dash cooldown
local sausageSpeed, sausageTimer, sausageCooldown = 300, 0, 7 -- stuff needed for sausage throw
local isSausageActive = false -- check if sausage is active
local sausageDirection = Vec3(0, 0, 0) -- direction sausage thrown

local topEnt, topCol = initRectCol("top", Vec3(310, 600, 0), 600, 150, "top") -- top desk
local leftEnt, leftCol = initRectCol("left", Vec3(10, 155, 0), 100, 320, "left") -- left desk
local rightEnt, rightCol = initRectCol("right", Vec3(1170, 90, 0), 100, 320,
                                       "right") -- right desk
local rightBEnt, rightBCol = initRectCol("rightbottom", Vec3(1050, 20, 0), 320,
                                         70, "rightbottom") -- right bottom L desk

-- handles rhino chasing player
local function rhinoChase(rhinoSprite, rhinoCollider, playerSprite, delta_time)
    local rhinoPos, playerPos = rhinoSprite:getPos(), playerSprite:getPos() -- get the positions
    local dx, dy = playerPos.x - rhinoPos.x, playerPos.y - rhinoPos.y -- calculate delta positions
    local distance = calculateDistance(dx, dy) -- calculate distance to player

    if distance > dashDistance and distance <= chaseDistance then -- if within range
        moveEntity(rhinoSprite, rhinoCollider, dx, dy, chaseSpeed, delta_time,
                   0.5) -- chase player
    end
end

-- handles rhino dash
local function rhinoDash(rhinoSprite, rhinoCollider, playerSprite, delta_time)
    if dashTimer > 0 then -- if dash on cooldown
        dashTimer = dashTimer - delta_time -- decrease cooldown timer
        return
    end

    local rhinoPos, playerPos = rhinoSprite:getPos(), playerSprite:getPos() -- get the positions
    local dx, dy = playerPos.x - rhinoPos.x, playerPos.y - rhinoPos.y -- calculate delta positions
    local distance = calculateDistance(dx, dy) -- calculate distance to player

    if distance <= dashDistance then -- if within range
        moveEntity(rhinoSprite, rhinoCollider, dx, dy, dashSpeed, delta_time, 1) -- dash

        -- if enemy player collision while dash
        if playerCollision(playerCollider, rhinoCollider) then
            playerData.decreaseHealth(25)
            dashTimer = dashCooldown -- reset cooldown
        end
    end
end

local rhinoNormalSpritePath = "./static/rhena.png" -- normal rhino sprite
local rhinoAttackSpritePath = "./static/rhena_attack.png" -- attack rhino sprite

-- function to throw sausage
local function throwSausage(delta_time)
    if isSausageActive then -- if the sausage being thrown
        local pos = sausageSprite:getPos() -- get pos of sausage
        pos.x = pos.x + sausageDirection.x * sausageSpeed * delta_time -- update x
        pos.y = pos.y + sausageDirection.y * sausageSpeed * delta_time -- update y
        sausageSprite:setPos(pos) -- set new pos of sausage sprite
        sausageCollider:setPos(pos) -- set new pos of sausage collider

        -- if sausage and player collide
        if playerCollision(playerCollider, sausageCollider) then
            playerData.decreaseHealth(5)
            isSausageActive = false -- deactivate sausage

            -- reset rhino sprite
            rhinoSprite:loadSprite(rhinoNormalSpritePath, RhinoSpriteWidth,
                                   RhinoSpriteHeight, rhinoSprite:getPos())
        end

        -- decrease sausage timer
        sausageTimer = sausageTimer - delta_time
        if sausageTimer <= 0 then -- if sausage timer runs out
            isSausageActive = false -- deactivate sausage

            -- reset rhino sprite
            rhinoSprite:loadSprite(rhinoNormalSpritePath, RhinoSpriteWidth,
                                   RhinoSpriteHeight, rhinoSprite:getPos())
        end
    elseif sausageTimer <= 0 then -- if sausage not active
        -- get rhino and player pos
        local rhinoPos, playerPos = rhinoSprite:getPos(), playerSprite:getPos()
        local dx, dy = playerPos.x - rhinoPos.x, playerPos.y - rhinoPos.y -- deltas
        local distance = calculateDistance(dx, dy) -- calc distance
        if distance > chaseDistance then -- if distance is further than the other ranges
            sausageDirection = Vec3(dx / distance, dy / distance, 0) -- normalize dir
            sausageSprite:setPos(rhinoPos) -- move sausage sprite to rhino
            sausageCollider:setPos(rhinoPos) -- move sausage collider to rhino

            isSausageActive = true -- activate sausage
            sausageTimer = sausageCooldown -- reset

            -- change to attack sprite
            rhinoSprite:loadSprite(rhinoAttackSpritePath, RhinoSpriteWidth,
                                   RhinoSpriteHeight, rhinoPos)
        end
    else
        sausageTimer = sausageTimer - delta_time -- decrease timer
    end
end

local offScreen = Vec3(-1000, -1000, 0) -- where sausage is hidden when inactive

-- to hide sausage when inactive
local function updateSausages()
    if not isSausageActive then -- if not active
        sausageSprite:setPos(offScreen) -- move sausage sprite
        sausageCollider:setPos(offScreen) -- move sausage collider
    end
end

local rhinoHealth = 100
local function playerAttack(playerCollider, rhinoCollider, key_states)
    if key_states["e"] then
        if playerCollision(playerCollider, rhinoCollider) then
            print("Rhino was attacked!")
            rhinoHealth = rhinoHealth - 100
        end
    end
    if controller_state["button_a"] then
        if playerCollision(playerCollider, rhinoCollider) then
            print("Rhino was attacked!")
            rhinoHealth = rhinoHealth - 100
        end
    end
end

local rhinoDed = false;
local function updateRhinoHealth()
    if rhinoHealth <= 0 then
        manager:removeEntity("rhino")
        rhinoDed = true
    end
end

local doorCreated = false
local function nextBoss()
    if rhinoDed then
        local door, doorCollider = initRectCol("door", Vec3(640, 360, 0), 50,
                                               100, "door")

        if not doorCreated then
            local doorSprite = door:addSpriteComponent()
            doorSprite:loadSprite("./static/door.png", 50, 100,
                                  Vec3(665, 410, 0))
            doorCreated = true
        end

        if playerCollision(playerCollider, doorCollider) then
            GameScene:changeScene("DemoScene")
        end
    end
end

function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["down"] then setPlayerDown(true) end
        if key_states["right"] then setPlayerRight(true) end
        if key_states["up"] then setPlayerUp(true) end
        if key_states["left"] then setPlayerLeft(true) end
        if event.key == "z" then triggerDash() end
        if key_states["space"] then
            GameScene:changeScene("Chicken") -- while debugging
        end
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

function update(delta_time)
    updateRhinoHealth()
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
    playerCollider:setPos(playerSprite:getPos()) -- update player collider pos

    if not rhinoDed then
        playerAttack(playerCollider, rhinoCollider, key_states)

        rhinoChase(rhinoSprite, rhinoCollider, playerSprite, delta_time) -- call rhino chase
        rhinoDash(rhinoSprite, rhinoCollider, playerSprite, delta_time) -- call rhino dash

        throwSausage(delta_time) -- throw sausage
        updateSausages() -- update when inactive
    end

    -- handles walking into tables
    handleCollision(playerCollider, leftCol, playerSprite, 90, 10)
    handleCollision(playerCollider, rightCol, playerSprite, -40, 10)
    handleCollision(playerCollider, rightBCol, playerSprite, -40, 40)
    handleCollision(playerCollider, topCol, playerSprite, 1, -70)
    handleCollision(playerCollider, rhinoCollider, playerSprite, 5, 5)

    nextBoss()
end
