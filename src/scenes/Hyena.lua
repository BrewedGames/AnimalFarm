local backgroundMusic = manager:addEntity("BackgroundMusic")
local audio = backgroundMusic:addAudioComponent()
audio:setAudio("./src/audio/Mercury.wav", true)
audio:setVolume(50)
audio:setLoop(true)
audio:Play()

local HyenaSpriteWidth = 145
local HyenaSpriteHeight = 157.38

local function sqrt(number)
    if number <= 0 then return 0 end
    local guess = number / 2
    for _ = 1, 10 do guess = (guess + number / guess) / 2 end
    return guess
end

local function calculateDistance(dx, dy) return sqrt(dx * dx + dy * dy) end

local function moveEntity(sprite, collider, dx, dy, speed, delta_time, dampening)
    local pos = sprite:getPos()
    local distance = calculateDistance(dx, dy)
    local directionX, directionY = dx / distance, dy / distance
    local newX = pos.x + directionX * speed * dampening * delta_time
    local newY = pos.y + directionY * speed * dampening * delta_time

    sprite:setPos(Vec3(newX, newY, pos.z))
    collider:setPos(Vec3(newX, newY, pos.z))
end

local function initEntity(name, spritePath, width, height, position,
                          colliderRad, tag)
    local entity = manager:addEntity(name)
    local sprite = entity:addSpriteComponent()
    local collider = entity:addColliderComponent()
    sprite:loadSprite(spritePath, width, height, position)

    if colliderRad then
        collider:addCircleCollider(position.x, position.y, colliderRad)
        if tag then collider:setTag(tag) end
    end

    return entity, sprite, collider
end

local function initRectCol(name, position, colliderWidth, colliderHeight, tag)
    local entity = manager:addEntity(name)
    local collider = entity:addColliderComponent()
    collider:addRectCollider(position.x, position.y, colliderWidth,
                             colliderHeight)

    if tag then collider:setTag(tag) end

    return entity, collider
end

local background, backgroundSprite = initEntity("Background",
                                                "./static/hyena_map.jpg", 1280,
                                                720, Vec3(640, 360, 0))
local hyena, hyenaSprite, hyenaCollider = initEntity("hyena",
                                                     "./static/hyena.png",
                                                     HyenaSpriteWidth,
                                                     HyenaSpriteHeight,
                                                     Vec3(500, 500, 0), 60,
                                                     "hyena")

req("player")
local playerData = initPlayer()
local playerSprite, playerCollider = playerData.sprite, playerData.collider

constGlobal("A_VERY_IMPORTANT_CONSTANT", 10)

local chaseDistance, chaseSpeed = 100, 100

local function hyenaChase(hyenaSprite, hyenaCollider, playerSprite, delta_time)
    local hyenaPos, playerPos = hyenaSprite:getPos(), playerSprite:getPos()
    local dx, dy = playerPos.x - hyenaPos.x, playerPos.y - hyenaPos.y
    local distance = calculateDistance(dx, dy)

    if distance <= chaseDistance then
        moveEntity(hyenaSprite, hyenaCollider, dx, dy, chaseSpeed, delta_time,
                   0.5)
    end
end

local hyenaHealth = 100
local function playerAttack(playerCollider, hyenaCollider, key_states)
    if key_states["e"] then
        if playerCollision(playerCollider, hyenaCollider) then
            print("Hyena was attacked!")
            hyenaHealth = hyenaHealth - 10
        end
    end
    if controller_state["button_a"] then
        if playerCollision(playerCollider, hyenaCollider) then
            print("Hyena was attacked!")
            hyenaHealth = hyenaHealth - 10
        end
    end
end

local hyenaDed = false
local function updateHyenaHealth()
    if hyenaHealth <= 0 then
        manager:removeEntity("hyena")
        hyenaDed = true
    end
end

local doorCreated = false
local function nextBoss()
    if hyenaDed then
        local door, doorCollider = initRectCol("door", Vec3(640, 360, 0), 50,
                                               100, "door")

        if not doorCreated then
            local doorSprite = door:addSpriteComponent()
            doorSprite:loadSprite("./static/door.png", 50, 100,
                                  Vec3(665, 410, 0))
            doorCreated = true
        end

        if playerCollision(playerCollider, doorCollider) then
            GameScene:changeScene("Rhino") -- if i got the order right?
        end
    end
end

-- arrays wont work and my head done till 2025 so enjoy the following :)
local offScreen = Vec3(-1000, -1000, 0)

local hyenaNormalSpritePath = "./static/hyena.png"
local hyenaAttackSpritePath = "./static/hyena_attack.png"

local knife1, knifeSprite1, knifeCollider1 =
    initEntity("knife1", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0),
               10, "knife")
local knife2, knifeSprite2, knifeCollider2 =
    initEntity("knife2", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0),
               10, "knife")
local knife3, knifeSprite3, knifeCollider3 =
    initEntity("knife3", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0),
               10, "knife")
local knife4, knifeSprite4, knifeCollider4 =
    initEntity("knife4", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0),
               10, "knife")

local knifeSpeed = 300
local speedIncreaseInterval = 10
local knifeSpeedIncrease = 70
local elapsedTime = 0
local knifeTimer1, knifeTimer2, knifeTimer3, knifeTimer4 = 0, 0, 0, 0
local knifeCooldown1, knifeCooldown2, knifeCooldown3, knifeCooldown4 = 7, 7.5,
                                                                       8, 8.5
local knifeDirection1, knifeDirection2, knifeDirection3, knifeDirection4 =
    Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0)
local isKnifeActive1, isKnifeActive2, isKnifeActive3, isKnifeActive4 = false,
                                                                       false,
                                                                       false,
                                                                       false

-- wave logic every 10 seconds increase the speed the knives thrown
local function updateKnifeSpeed(delta_time)
    elapsedTime = elapsedTime + delta_time
    if elapsedTime >= speedIncreaseInterval then
        knifeSpeed = knifeSpeed + knifeSpeedIncrease
        elapsedTime = 0
    end
end

local function throwKnife1(delta_time)
    if isKnifeActive1 then
        local pos = knifeSprite1:getPos()
        pos.x = pos.x + knifeDirection1.x * knifeSpeed * delta_time
        pos.y = pos.y + knifeDirection1.y * knifeSpeed * delta_time
        knifeSprite1:setPos(pos)
        knifeCollider1:setPos(pos)

        if playerCollision(playerCollider, knifeCollider1) then
            playerData.decreaseHealth(5)
            isKnifeActive1 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end

        knifeTimer1 = knifeTimer1 - delta_time
        if knifeTimer1 <= 0 then
            isKnifeActive1 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end
    elseif knifeTimer1 <= 0 then
        local hyenaPos, playerPos = hyenaSprite:getPos(), playerSprite:getPos()
        local dx, dy = playerPos.x - hyenaPos.x, playerPos.y - hyenaPos.y
        local distance = calculateDistance(dx, dy)
        if distance > chaseDistance then
            knifeDirection1 = Vec3(dx / distance, dy / distance, 0)
            knifeSprite1:setPos(hyenaPos)
            knifeCollider1:setPos(hyenaPos)

            isKnifeActive1 = true
            knifeTimer1 = knifeCooldown1

            hyenaSprite:loadSprite(hyenaAttackSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaPos)
        end
    else
        knifeTimer1 = knifeTimer1 - delta_time
    end
end

local function throwKnife2(delta_time)
    if isKnifeActive2 then
        local pos = knifeSprite2:getPos()
        pos.x = pos.x + knifeDirection2.x * knifeSpeed * delta_time
        pos.y = pos.y + knifeDirection2.y * knifeSpeed * delta_time
        knifeSprite2:setPos(pos)
        knifeCollider2:setPos(pos)

        if playerCollision(playerCollider, knifeCollider2) then
            playerData.decreaseHealth(5)
            isKnifeActive2 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end

        knifeTimer2 = knifeTimer2 - delta_time
        if knifeTimer2 <= 0 then
            isKnifeActive2 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end
    elseif knifeTimer2 <= 0 then
        local hyenaPos, playerPos = hyenaSprite:getPos(), playerSprite:getPos()
        local dx, dy = playerPos.x - hyenaPos.x, playerPos.y - hyenaPos.y
        local distance = calculateDistance(dx, dy)
        if distance > chaseDistance then
            knifeDirection2 = Vec3(dx / distance, dy / distance, 0)
            knifeSprite2:setPos(hyenaPos)
            knifeCollider2:setPos(hyenaPos)

            isKnifeActive2 = true
            knifeTimer2 = knifeCooldown2

            hyenaSprite:loadSprite(hyenaAttackSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaPos)
        end
    else
        knifeTimer2 = knifeTimer2 - delta_time
    end
end

local function throwKnife3(delta_time)
    if isKnifeActive3 then
        local pos = knifeSprite3:getPos()
        pos.x = pos.x + knifeDirection3.x * knifeSpeed * delta_time
        pos.y = pos.y + knifeDirection3.y * knifeSpeed * delta_time
        knifeSprite3:setPos(pos)
        knifeCollider3:setPos(pos)

        if playerCollision(playerCollider, knifeCollider3) then
            playerData.decreaseHealth(5)
            isKnifeActive3 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end

        knifeTimer3 = knifeTimer3 - delta_time
        if knifeTimer3 <= 0 then
            isKnifeActive3 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end
    elseif knifeTimer3 <= 0 then
        local hyenaPos, playerPos = hyenaSprite:getPos(), playerSprite:getPos()
        local dx, dy = playerPos.x - hyenaPos.x, playerPos.y - hyenaPos.y
        local distance = calculateDistance(dx, dy)
        if distance > chaseDistance then
            knifeDirection3 = Vec3(dx / distance, dy / distance, 0)
            knifeSprite3:setPos(hyenaPos)
            knifeCollider3:setPos(hyenaPos)

            isKnifeActive3 = true
            knifeTimer3 = knifeCooldown3

            hyenaSprite:loadSprite(hyenaAttackSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaPos)
        end
    else
        knifeTimer3 = knifeTimer3 - delta_time
    end
end

local function throwKnife4(delta_time)
    if isKnifeActive4 then
        local pos = knifeSprite4:getPos()
        pos.x = pos.x + knifeDirection4.x * knifeSpeed * delta_time
        pos.y = pos.y + knifeDirection4.y * knifeSpeed * delta_time
        knifeSprite4:setPos(pos)
        knifeCollider4:setPos(pos)

        if playerCollision(playerCollider, knifeCollider4) then
            playerData.decreaseHealth(5)
            isKnifeActive4 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end

        knifeTimer4 = knifeTimer4 - delta_time
        if knifeTimer4 <= 0 then
            isKnifeActive4 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaSprite:getPos())
        end
    elseif knifeTimer4 <= 0 then
        local hyenaPos, playerPos = hyenaSprite:getPos(), playerSprite:getPos()
        local dx, dy = playerPos.x - hyenaPos.x, playerPos.y - hyenaPos.y
        local distance = calculateDistance(dx, dy)
        if distance > chaseDistance then
            knifeDirection4 = Vec3(dx / distance, dy / distance, 0)
            knifeSprite4:setPos(hyenaPos)
            knifeCollider4:setPos(hyenaPos)

            isKnifeActive4 = true
            knifeTimer4 = knifeCooldown4

            hyenaSprite:loadSprite(hyenaAttackSpritePath, HyenaSpriteWidth,
                                   HyenaSpriteHeight, hyenaPos)
        end
    else
        knifeTimer4 = knifeTimer4 - delta_time
    end
end

local function updateKnives()
    if not isKnifeActive1 then
        knifeSprite1:setPos(offScreen)
        knifeCollider1:setPos(offScreen)
    end
    if not isKnifeActive2 then
        knifeSprite2:setPos(offScreen)
        knifeCollider2:setPos(offScreen)
    end
    if not isKnifeActive3 then
        knifeSprite3:setPos(offScreen)
        knifeCollider3:setPos(offScreen)
    end
    if not isKnifeActive4 then
        knifeSprite4:setPos(offScreen)
        knifeCollider4:setPos(offScreen)
    end
end

local rotatingRectEntity = manager:addEntity("rotatingRect")
local rectSprite = rotatingRectEntity:addSpriteComponent()
rectSprite:loadSprite("./static/door.png", 500, 20, offScreen)

local rectActive = false
local rectTimer = 0
local rectCooldown = 10
local rectActiveTime = 7
local rectAngle = 0

local function initRect()
    local hyenaPos = hyenaSprite:getPos()
    rectAngle = 0
    rectSprite:setPos(Vec3(hyenaPos.x + 250, hyenaPos.y, 0))
end

local function rotatePointAroundOrigin(x, y, originX, originY, angle)
    local angleRad = math.rad(angle)
    local cosA, sinA = math.cos(angleRad), math.sin(angleRad)

    local translatedX, translatedY = x - originX, y - originY

    local rotatedX = translatedX * cosA - translatedY * sinA
    local rotatedY = translatedX * sinA + translatedY * cosA

    return rotatedX + originX, rotatedY + originY
end

local function spawnRect(delta_time)
    rectTimer = rectTimer + delta_time
    if rectTimer >= rectCooldown then
        rectActive = true
        rectTimer = 0
        initRect()
    end
end

local rectActiveTimeLimit = 5
local rectActiveElapsedTime = 0
local function updateRect(delta_time)
    if rectActive then
        rectActiveElapsedTime = rectActiveElapsedTime + delta_time

        rectAngle = rectAngle + 90 * delta_time
        if rectAngle >= 360 then rectAngle = rectAngle - 360 end

        local hyenaPos = hyenaSprite:getPos()
        local rectLength = 250
        local freeEndX, freeEndY = rotatePointAroundOrigin(hyenaPos.x +
                                                               rectLength,
                                                           hyenaPos.y,
                                                           hyenaPos.x,
                                                           hyenaPos.y, rectAngle)

        local midPointX = (hyenaPos.x + freeEndX) / 2
        local midPointY = (hyenaPos.y + freeEndY) / 2

        rectSprite:setPos(Vec3(midPointX, midPointY, 0))

        if rectActiveElapsedTime >= rectActiveTimeLimit then
            rectActive = false
            rectSprite:setPos(offScreen)
            rectActiveElapsedTime = 0
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
            GameScene:changeScene("Rhino") -- while debugging
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
    updateHyenaHealth()

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
    playerCollider:setPos(playerSprite:getPos())

    if not hyenaDed then
        playerAttack(playerCollider, hyenaCollider, key_states)
        hyenaChase(hyenaSprite, hyenaCollider, playerSprite, delta_time)

        throwKnife1(delta_time)
        throwKnife2(delta_time)
        throwKnife3(delta_time)
        throwKnife4(delta_time)
        updateKnives()
        updateKnifeSpeed(delta_time)

        spawnRect(delta_time)
        updateRect(delta_time)

        if rectActive then
            local hyenaPos = hyenaSprite:getPos()
            rectAngle = rectAngle + 90 * delta_time

            if rectAngle >= 360 then rectAngle = rectAngle - 360 end

            local radius = 100
            local angleRad = math.rad(rectAngle)
            local offsetX = radius * math.cos(angleRad)
            local offsetY = radius * math.sin(angleRad)

            rectSprite:setPos(
                Vec3(hyenaPos.x + offsetX, hyenaPos.y + offsetY, 0))
        end
    end

    handleCollision(playerCollider, hyenaCollider, playerSprite, 5, 5)
    nextBoss()
end
