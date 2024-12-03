<<<<<<< Updated upstream
print("hello from the player script")
=======

local current = 100
function initPlayer()
	local player = manager:addEntity("Player")
	local sprite = player:addSpriteComponent()
	local playerCollider = player:addColliderComponent()

	sprite:loadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(140, 100, 1), true, 32, 8, 100)
	sprite:setAnimation("WalkBack", 0, 7, 100)
	sprite:setAnimation("WalkLeft", 8, 15, 100)
	sprite:setAnimation("WalkRight", 16, 23, 100)
	sprite:setAnimation("WalkFront", 24, 31, 100)

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

    local speed = 1

    if key_states["lshift"] and canDash then
        speed = 40
        canDash = false
        dashCooldown = 5
        print("dahsed")--maybe add these as text in game
    elseif not canDash then
        speed = 1
    end

    if not canDash then
        dashCooldown = dashCooldown - delta_time
        if dashCooldown < 0 then
            dashCooldown = 0
            canDash = true
            print("next dash ready")--maybe add these as text in game
        end
    end

    if key_states["s"] then
        direction = "WalkBack"
        newY = newY - speed
    elseif key_states["d"] then
        direction = "WalkRight"
        newX = newX + speed
    elseif key_states["w"] then
        direction = "WalkFront"
        newY = newY + speed
    elseif key_states["a"] then
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
        sprite:clearAnimation()
        if direction then
            sprite:playAnimation(direction)
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

function rectCollision(otherCollider, playerCollider)
	if(otherCollider:isColliding(playerCollider))then
		return true
	end
	return false
end
>>>>>>> Stashed changes



player = manager:addEntity("Player")

player.isStatic = false

sprite = player:addSpriteComponent()
playerCollider = player:addColliderComponent()

sprite:loadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(40, 100, 1), true, 32, 8, 100)
playerCollider:setTag("Player")
playerCollider:addCapsuleCollider(sprite:getPos().x, sprite:getPos().y, 50, 100)

sprite:setAnimation("WalkBack", 0, 7, 100)
sprite:setAnimation("WalkLeft", 8, 15, 100)
sprite:setAnimation("WalkRight", 16, 23, 100)
sprite:setAnimation("WalkFront", 24, 31, 100)