local current = 100
local playerspeed = 10
local dashspeed = 50

local CONTROLLERTHRESHOLD = 8000

function initPlayer()
	local player = manager:addEntity("Player")
	local sprite = player:addSpriteComponent()
	local playerCollider = player:addColliderComponent()

	sprite:loadSprite("./static/armidillio.png", 50, 80.4, Vec3(140, 100, 1))

	playerCollider:setTag("Player")
	playerCollider:addCapsuleCollider(sprite:getPos().x, sprite:getPos().y, 50, 100)

	local function decreaseHealth(amount)
		current = current - amount
        print(current)

        if(current <= 0)then
            manager:removeEntity("Player")
            print("playerded")
        end
	end

	return {
		entity = player,
		sprite = sprite,
		collider = playerCollider,
		decreaseHealth = decreaseHealth,
	}
end

local prevDir = nil
local screenWidth = 1280
local screenHeight = 720
local playerWidth = 100
local playerHeight = 100

local dashCooldown = 0
local canDash = true
local acceleration = 50
local deceleration = 30
local velocityX = 0
local velocityY = 0
local maxSpeed = 250 

function handlePlayerInput(key_states, sprite, delta_time, player)
    local newX = sprite:getPos().x
    local newY = sprite:getPos().y


    local dx, dy = 0, 0

    if key_states["up"] then
        dy = dy + 1
    end
    if key_states["down"] then
        dy = dy - 1
    end
    if key_states["right"] then
        dx = dx + 1
    end
    if key_states["left"] then
        dx = dx - 1
    end

    if controller_state["left_stick_x"] > CONTROLLERTHRESHOLD then
        dx = dx + 1
    elseif controller_state["left_stick_x"] < -CONTROLLERTHRESHOLD then
        dx = dx - 1
    end

    if controller_state["left_stick_y"] > CONTROLLERTHRESHOLD then
        dy = dy - 1
    elseif controller_state["left_stick_y"] < -CONTROLLERTHRESHOLD then
        dy = dy + 1
    end


    if dx ~= 0 or dy ~= 0 then
        local length = (dx * dx + dy * dy) ^ 0.5
        dx = dx / length
        dy = dy / length
    end


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


    if newX < 0 then newX = 0 end
    if newX > screenWidth - playerWidth then newX = screenWidth - playerWidth end
    if newY < 0 then newY = 0 end
    if newY > screenHeight - playerHeight then newY = screenHeight - playerHeight end

    sprite:setPos(Vec3(newX, newY, sprite:getPos().z))
end


function handleCollision(playerCollider, otherCollider, playerSprite, bounceAmountX, bounceAmountY)
    if playerCollider:isColliding(otherCollider) then
        local playerPos = playerSprite:getPos()
        local otherPos = otherCollider:getPos()
        
        local bounceDirection = Vec3(
            playerPos.x > otherPos.x and 1 or -1,
            playerPos.y > otherPos.y and 1 or -1, 0
        )
        
        playerSprite:setPos(Vec3(
            playerPos.x + bounceDirection.x * bounceAmountX,
            playerPos.y + bounceDirection.y * bounceAmountY, playerPos.z
        ))
    end
end

function rectCollision(otherCollider, playerCollider, otherWidth, otherHeight)
	if(otherCollider:isColliding(playerCollider))then
		return true
	end
	return false
end

function playerCollision(playerCollider, otherCollider)
	if(playerCollider:isColliding(otherCollider))then
		return true
	end
	return false
end