req("playerSettings")
local player = manager:addEntity("Player")
local sprite = player:addSpriteComponent()
local playerCollider = player:addColliderComponent()
local arrow = manager:addEntity("Arrow")
local arrowSprite = arrow:addSpriteComponent()



function initPlayer()

    sprite:loadSprite("./static/armadillo-spritesheet.png", 150, 150,
                      Vec3(140, 100, 1), true, 25, 5, 100)

    playerCollider:setTag("Player")
    playerCollider:addCapsuleCollider(sprite:getPos().x, sprite:getPos().y, 50,
                                      60)

    sprite:setAnimation("Idle", 0, 0, 100)
    sprite:setAnimation("WalkBack", 0, 2, 100)
    sprite:setAnimation("WalkBackLeft", 3, 5, 100)
    sprite:setAnimation("WalkBackRight", 6, 8, 100)
    sprite:setAnimation("WalkLeft", 9, 11, 100)
    sprite:setAnimation("WalkRight", 12, 14, 100)
    sprite:setAnimation("WalkFront", 15, 17, 100)
    sprite:setAnimation("WalkFrontLeft", 18, 20, 100)
    sprite:setAnimation("WalkFrontRight", 21, 23, 100)

    prevDirX = 0
    prevDirY = 1

    arrowSprite:loadSprite("./static/arrow.png", 20, 20, Vec3(140, 100, 1))
    arrowSprite:setPos(Vec3(sprite:getPos().x + arrowRadius, sprite:getPos().y,
                            sprite:getPos().z))

    local function decreaseHealth(amount)
        current = current - amount
        print(current)

        if (current <= 0) then
            manager:removeEntity("Player")
            print("playerded")
        end
    end

    return {
        entity = player,
        sprite = sprite,
        collider = playerCollider,
        decreaseHealth = decreaseHealth
    }
end

function setPlayerUp(result) playerUp = result end

function setPlayerDown(result) playerDown = result end

function setPlayerLeft(result) playerLeft = result end

function setPlayerRight(result) playerRight = result end

function resetPlayerInput()
    playerUp = false
    playerDown = false
    playerLeft = false
    playerRight = false
end

local dashDirectionX = 0
local dashDirectionY = 0

function triggerDash()
    if canDash then
        isDashing = true
        dashTimer = dashDuration
        canDash = false
        dashCooldown = 1.0

        RumbleController(45000, 50000, 120)

        if playerUp or playerDown or playerLeft or playerRight then
            prevDirX = (playerRight and 1 or 0) - (playerLeft and 1 or 0)
            prevDirY = (playerUp and 1 or 0) - (playerDown and 1 or 0)
        end

        if prevDirX == 0 and prevDirY == 0 then
            prevDirX = 1
            prevDirY = 0
        end

        local length = math.sqrt(prevDirX * prevDirX + prevDirY * prevDirY)
        if length ~= 0 then
            prevDirX = prevDirX / length
            prevDirY = prevDirY / length
        end
    else
        RumbleController(10000, 5000, 30)
        RumbleController(10000, 5000, 30)
    end
end

function updatePlayer(delta_time)
    local newX = sprite:getPos().x
    local newY = sprite:getPos().y
    local newAnimation = nil

    local dx, dy = 0, 0

    -- Movement input
    if not isDashing then
        if playerUp and playerLeft then
            dy = dy + 0.5
            dx = dx - 0.5
            newAnimation = "WalkFrontLeft"
        elseif playerUp and playerRight then
            dy = dy + 0.5
            dx = dx + 0.5
            newAnimation = "WalkFrontRight"
        elseif playerDown and playerLeft then
            dy = dy - 0.5
            dx = dx - 0.5
            newAnimation = "WalkBackLeft"
        elseif playerDown and playerRight then
            dy = dy - 0.5
            dx = dx + 0.5
            newAnimation = "WalkBackRight"
        elseif playerUp then
            dy = dy + 1
            newAnimation = "WalkFront"
        elseif playerDown then
            dy = dy - 1
            newAnimation = "WalkBack"
        elseif playerLeft then
            dx = dx - 1
            newAnimation = "WalkLeft"
        elseif playerRight then
            dx = dx + 1
            newAnimation = "WalkRight"
        else
            newAnimation = nil
        end
    end

    if dx ~= 0 or dy ~= 0 then
        prevDirX = dx
        prevDirY = dy
    end

    if isDashing then
        newX = newX + prevDirX * dashspeed * delta_time
        newY = newY + prevDirY * dashspeed * delta_time
        dashTimer = dashTimer - delta_time
        if dashTimer <= 0 then isDashing = false end
    else

        velocityX = velocityX + dx * acceleration
        velocityY = velocityY + dy * acceleration

        if velocityX > maxSpeed then velocityX = maxSpeed end
        if velocityX < -maxSpeed then velocityX = -maxSpeed end
        if velocityY > maxSpeed then velocityY = maxSpeed end
        if velocityY < -maxSpeed then velocityY = -maxSpeed end

        if dx == 0 then
            velocityX = velocityX * (1 - deceleration * delta_time)
        end
        if dy == 0 then
            velocityY = velocityY * (1 - deceleration * delta_time)
        end

        newX = newX + velocityX * delta_time
        newY = newY + velocityY * delta_time
    end

    if not canDash then
        dashCooldown = dashCooldown - delta_time
        if dashCooldown <= 0 then canDash = true end
    end

    if newX < 0 then newX = 0 end
    if newX > screenWidth - playerWidth then newX = screenWidth - playerWidth end
    if newY < 0 then newY = 0 end
    if newY > screenHeight - playerHeight then
        newY = screenHeight - playerHeight
    end

    sprite:setPos(Vec3(newX, newY, sprite:getPos().z))
    playerCollider:setPos(Vec3(newX, newY, sprite:getPos().z))

    updateArrowPosition()

    if newAnimation ~= currentAnimation then
        if newAnimation then
            sprite:playAnimation(newAnimation, true)
        else
            sprite:clearAnimation()
            sprite:playAnimation("Idle", true)
        end
        currentAnimation = newAnimation
    end
end


function updateArrowPosition()
    local playerPos = sprite:getPos()
    local stickX = controller_state["right_stick_x"]
    local stickY = controller_state["right_stick_y"]

    -- Update the angle based on the stick input
    if math.abs(stickX) > CONTROLLERTHRESHOLD or math.abs(stickY) > CONTROLLERTHRESHOLD then
        lastAngle = math.atan2(-stickY, stickX)
    end

    local offsetX = math.cos(lastAngle) * arrowRadius
    local offsetY = math.sin(lastAngle) * arrowRadius

    -- Update the arrow's position
    arrowSprite:setPos(Vec3(playerPos.x + offsetX, playerPos.y + offsetY, playerPos.z))

    -- Apply the rotation with an offset
    arrowSprite:setRotation(math.deg(lastAngle) + arrowRotationOffset)
end

function handleCollision(playerCollider, otherCollider, playerSprite,
                         bounceAmountX, bounceAmountY)
    if playerCollider:isColliding(otherCollider) then
        local playerPos = playerSprite:getPos()
        local otherPos = otherCollider:getPos()

        local bounceDirection = Vec3(playerPos.x > otherPos.x and 1 or -1,
                                     playerPos.y > otherPos.y and 1 or -1, 0)

        playerSprite:setPos(Vec3(
                                playerPos.x + bounceDirection.x * bounceAmountX,
                                playerPos.y + bounceDirection.y * bounceAmountY,
                                playerPos.z))
    end
end

function rectCollision(otherCollider, playerCollider, otherWidth, otherHeight)
    if (otherCollider:isColliding(playerCollider)) then return true end
    return false
end

function playerCollision(playerCollider, otherCollider)
    if (playerCollider:isColliding(otherCollider)) then return true end
    return false
end
