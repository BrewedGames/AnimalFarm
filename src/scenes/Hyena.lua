local function sqrt(number)
    if number <= 0 then return 0 end
    local guess = number / 2
    for _ = 1, 10 do
        guess = (guess + number / guess) / 2
    end
    return guess
end

local function calculateDistance(dx, dy)
    return sqrt(dx * dx + dy * dy)
end

local function moveEntity(sprite, collider, dx, dy, speed, delta_time, dampening)
    local pos = sprite:getPos()
    local distance = calculateDistance(dx, dy)
    local directionX, directionY = dx / distance, dy / distance
    local newX = pos.x + directionX * speed * dampening * delta_time
    local newY = pos.y + directionY * speed * dampening * delta_time

    sprite:setPos(Vec3(newX, newY, pos.z))
    collider:setPos(Vec3(newX, newY, pos.z))
end

local function initEntity(name, spritePath, width, height, position, colliderRad, tag)
    local entity = manager:addEntity(name)
    local sprite = entity:addSpriteComponent()
    local collider = entity:addColliderComponent()
    sprite:loadSprite(spritePath, width, height, position)

    if colliderRad then
        collider:addCircleCollider(position.x, position.y, colliderRad)
        if tag then 
            collider:setTag(tag)
        end 
    end

    return entity, sprite, collider
end

local function initRectCol(name, position, colliderWidth, colliderHeight, tag)
    local entity = manager:addEntity(name)
    local collider = entity:addColliderComponent()
    collider:addRectCollider(position.x, position.y, colliderWidth, colliderHeight)

    if tag then
        collider:setTag(tag)
    end

    return entity, collider
end

local background, backgroundSprite = initEntity("Background", "./static/hyena_map.jpg", 1280, 720, Vec3(640, 360, 0))
local hyena, hyenaSprite, hyenaCollider = initEntity("hyena", "./static/hyena.png", 100, 120, Vec3(500, 500, 0), 60, "hyena")

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
        moveEntity(hyenaSprite, hyenaCollider, dx, dy, chaseSpeed, delta_time, 0.5)
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
        local door, doorCollider = initRectCol("door", Vec3(640, 360, 0), 50, 100, "door")

        if not doorCreated then 
            local doorSprite = door:addSpriteComponent()
            doorSprite:loadSprite("./static/door.png", 50, 100, Vec3(665, 410, 0))
            doorCreated = true
        end

        if playerCollision(playerCollider, doorCollider) then
            GameScene:changeScene("WinScene")--if i got the order right?
        end
    end
end

--arrays wont work and my head done till 2025 so enjoy the following :)
local offScreen = Vec3(-1000, -1000, 0)

local hyenaNormalSpritePath = "./static/hyena.png"
local hyenaAttackSpritePath = "./static/hyena_attack.png"

local knife1, knifeSprite1, knifeCollider1 = initEntity("knife1", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife2, knifeSprite2, knifeCollider2 = initEntity("knife2", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife3, knifeSprite3, knifeCollider3 = initEntity("knife3", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife4, knifeSprite4, knifeCollider4 = initEntity("knife4", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")

local knifeSpeed = 300
local speedIncreaseInterval = 10
local knifeSpeedIncrease = 70
local elapsedTime = 0
local knifeTimer1, knifeTimer2, knifeTimer3, knifeTimer4 = 0, 0, 0, 0
local knifeCooldown1, knifeCooldown2, knifeCooldown3, knifeCooldown4 = 7, 7.5, 8, 8.5
local knifeDirection1, knifeDirection2, knifeDirection3, knifeDirection4 = Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0)
local isKnifeActive1, isKnifeActive2, isKnifeActive3, isKnifeActive4 = false, false, false, false

--wave logic every 10 seconds increase the speed the knives thrown
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer1 = knifeTimer1 - delta_time
        if knifeTimer1 <= 0 then
            isKnifeActive1 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer2 = knifeTimer2 - delta_time
        if knifeTimer2 <= 0 then
            isKnifeActive2 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer3 = knifeTimer3 - delta_time
        if knifeTimer3 <= 0 then
            isKnifeActive3 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer4 = knifeTimer4 - delta_time
        if knifeTimer4 <= 0 then
            isKnifeActive4 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

local function sqrt(number)
    if number <= 0 then return 0 end
    local guess = number / 2
    for _ = 1, 10 do
        guess = (guess + number / guess) / 2
    end
    return guess
end

local function calculateDistance(dx, dy)
    return sqrt(dx * dx + dy * dy)
end

local function moveEntity(sprite, collider, dx, dy, speed, delta_time, dampening)
    local pos = sprite:getPos()
    local distance = calculateDistance(dx, dy)
    local directionX, directionY = dx / distance, dy / distance
    local newX = pos.x + directionX * speed * dampening * delta_time
    local newY = pos.y + directionY * speed * dampening * delta_time

    sprite:setPos(Vec3(newX, newY, pos.z))
    collider:setPos(Vec3(newX, newY, pos.z))
end

local function initEntity(name, spritePath, width, height, position, colliderRad, tag)
    local entity = manager:addEntity(name)
    local sprite = entity:addSpriteComponent()
    local collider = entity:addColliderComponent()
    sprite:loadSprite(spritePath, width, height, position)

    if colliderRad then
        collider:addCircleCollider(position.x, position.y, colliderRad)
        if tag then 
            collider:setTag(tag)
        end 
    end

    return entity, sprite, collider
end

local function initRectCol(name, position, colliderWidth, colliderHeight, tag)
    local entity = manager:addEntity(name)
    local collider = entity:addColliderComponent()
    collider:addRectCollider(position.x, position.y, colliderWidth, colliderHeight)

    if tag then
        collider:setTag(tag)
    end

    return entity, collider
end

local background, backgroundSprite = initEntity("Background", "./static/hyena_map.jpg", 1280, 720, Vec3(640, 360, 0))
local hyena, hyenaSprite, hyenaCollider = initEntity("hyena", "./static/hyena.png", 100, 120, Vec3(500, 500, 0), 60, "hyena")

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
        moveEntity(hyenaSprite, hyenaCollider, dx, dy, chaseSpeed, delta_time, 0.5)
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
        local door, doorCollider = initRectCol("door", Vec3(640, 360, 0), 50, 100, "door")

        if not doorCreated then 
            local doorSprite = door:addSpriteComponent()
            doorSprite:loadSprite("./static/door.png", 50, 100, Vec3(665, 410, 0))
            doorCreated = true
        end

        if playerCollision(playerCollider, doorCollider) then
            GameScene:changeScene("WinScene")--if i got the order right?
        end
    end
end

--arrays wont work and my head done till 2025 so enjoy the following :)
local offScreen = Vec3(-1000, -1000, 0)

local hyenaNormalSpritePath = "./static/hyena.png"
local hyenaAttackSpritePath = "./static/hyena_attack.png"

local knife1, knifeSprite1, knifeCollider1 = initEntity("knife1", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife2, knifeSprite2, knifeCollider2 = initEntity("knife2", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife3, knifeSprite3, knifeCollider3 = initEntity("knife3", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")
local knife4, knifeSprite4, knifeCollider4 = initEntity("knife4", "./static/knife.png", 20, 10, Vec3(-1000, -1000, 0), 10, "knife")

local knifeSpeed = 300
local speedIncreaseInterval = 10
local knifeSpeedIncrease = 70
local elapsedTime = 0
local knifeTimer1, knifeTimer2, knifeTimer3, knifeTimer4 = 0, 0, 0, 0
local knifeCooldown1, knifeCooldown2, knifeCooldown3, knifeCooldown4 = 7, 7.5, 8, 8.5
local knifeDirection1, knifeDirection2, knifeDirection3, knifeDirection4 = Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0), Vec3(0, 0, 0)
local isKnifeActive1, isKnifeActive2, isKnifeActive3, isKnifeActive4 = false, false, false, false

--wave logic every 10 seconds increase the speed the knives thrown
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer1 = knifeTimer1 - delta_time
        if knifeTimer1 <= 0 then
            isKnifeActive1 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer2 = knifeTimer2 - delta_time
        if knifeTimer2 <= 0 then
            isKnifeActive2 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer3 = knifeTimer3 - delta_time
        if knifeTimer3 <= 0 then
            isKnifeActive3 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
        end

        knifeTimer4 = knifeTimer4 - delta_time
        if knifeTimer4 <= 0 then
            isKnifeActive4 = false

            hyenaSprite:loadSprite(hyenaNormalSpritePath, 100, 120, hyenaSprite:getPos())
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

            hyenaSprite:loadSprite(hyenaAttackSpritePath, 140, 120, hyenaPos)
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

local rectEntity = manager:addEntity("rotatingRect")
local rectCollider = rectEntity:addColliderComponent()
rectCollider:addRectCollider(-1000,-1000, 500, 20)
local rectSprite = rectEntity:addSpriteComponent()
rectSprite:loadSprite("./static/door.png", 500, 20, Vec3(-1000, -1000, 0))
rectCollider:setTag("rect")

local rotateTimer = 0
local spinDuration = 20
local spawnInterval = 5
local rotating = false
local angle = 0
local radius = 20
local rectPos = Vec3(0, 0, 0)

local function startRotation(hyenaPos)
    rectPos = Vec3(hyenaPos.x + radius, hyenaPos.y, hyenaPos.z)
    rectSprite:setPos(rectPos)
    rectCollider:setPos(rectPos)
    rotating = true
    angle = 0
end

local function updateRotation(delta_time)
    if rotating then
        local steps = 360
        local stepSize = (360 / steps) * 200

        angle = angle + stepSize * delta_time
        if angle >= 360 then
            angle = angle - 360
        end

        local xOffset = radius * (angle % steps) / steps
        local yOffset = radius * (angle % steps) / steps
        
        rectPos.x = hyenaSprite:getPos().x + xOffset
        rectPos.y = hyenaSprite:getPos().y + yOffset

        local forFixingCol = Vec3(250, 8, 0)

        rectSprite:setPos(rectPos)
        rectCollider:setPos(rectPos - forFixingCol)

        rotateTimer = rotateTimer + delta_time
        if rotateTimer >= spinDuration then
            rectSprite:setPos(offScreen)
            rectCollider:setPos(offScreen)
            rotating = false
            rotateTimer = 0
        end
    end
end

local function spawnRotatingRect(delta_time)
    rotateTimer = rotateTimer + delta_time
    if rotateTimer >= spawnInterval and not rotating then
        startRotation(hyenaSprite:getPos())
        rotateTimer = 0
    end
end

function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
    elseif event.type == "keyup" then
        key_states[event.key] = false
    end
end

function update(delta_time)

    updateHyenaHealth()
    handlePlayerInput(key_states, playerSprite, delta_time)
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

        spawnRotatingRect(delta_time)
        updateRotation(delta_time)
    end

    handleCollision(playerCollider, hyenaCollider, playerSprite, 5, 5)
    nextBoss()
end
