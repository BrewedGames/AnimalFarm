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

	local health = 
	{
		current = 100,
		max = 100,
	}

	local function decreaseHealth(amount)
		health.current = math.max(0, health.current - amount)
		if health.current <= 0 then
			die()
		end
	end

	local function resetHealth()
		health.current = health.max
	end

	local function die()
		print("ded")
	end

	return {
		entity = player,
		sprite = sprite,
		collider = playerCollider,
		health = health,
		decreaseHealth = decreaseHealth,
		resetHealth = resetHealth,
		die = die,
	}
end

local prevDir = nil
local screenWidth = 1280
local screenHeight = 720
local playerWidth = 100
local playerHeight = 100
function handlePlayerInput(key_states, sprite)
    local direction
    local newX = sprite:getPos().x
    local newY = sprite:getPos().y

    if key_states["s"] then
        direction = "WalkBack"
        newY = newY - 1
    elseif key_states["d"] then
        direction = "WalkRight"
        newX = newX + 1
    elseif key_states["w"] then
        direction = "WalkFront"
        newY = newY + 1
    elseif key_states["a"] then
        direction = "WalkLeft"
        newX = newX - 1
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

--might change
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
