
local current = 100
local playerspeed = 10
local dashspeed = 50

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
function handlePlayerInput(key_states, sprite, delta_time, player)
    local direction
    local newX = sprite:getPos().x
    local newY = sprite:getPos().y

    local speed = playerspeed

    if key_states["z"] and canDash then
        speed = dashspeed
        canDash = false
        dashCooldown = 5
        print("dahsed")--maybe add these as text in game
    elseif not canDash then
        speed = playerspeed
    end

    if not canDash then
        dashCooldown = dashCooldown - delta_time
        if dashCooldown < 0 then
            dashCooldown = 0
            canDash = true
            print("next dash ready")--maybe add these as text in game
        end
    end

    if key_states["down"] then
        direction = "WalkBack"
        newY = newY - speed
    elseif key_states["right"] then
        direction = "WalkRight"
        newX = newX + speed
    elseif key_states["up"] then
        direction = "WalkFront"
        newY = newY + speed
    elseif key_states["left"] then
        direction = "WalkLeft"
        newX = newX - speed
    else
        direction = nil
    end

    if newX < 0 then
        newX = 0
    elseif newX > screenWidth - playerWidth then
        newX = screenWidth - playerWidth
    end

    if newY < 0 then
        newY = 0
    elseif newY > screenHeight - playerHeight then
        newY = screenHeight - playerHeight
    end

    sprite:setPos(Vec3(newX, newY, sprite:getPos().z))

    if direction ~= prevDir then

        if direction then

        end
        prevDir = direction
    end
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
