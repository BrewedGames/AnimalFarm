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
function handlePlayerInput(key_states, sprite)
		local direction
		if key_states["s"] then
			direction = "WalkBack"
			sprite:setPos(Vec3(sprite:getPos().x, sprite:getPos().y-1,sprite:getPos().z))
		elseif key_states["d"] then
			direction = "WalkRight"
			sprite:setPos(Vec3(sprite:getPos().x + 1, sprite:getPos().y,sprite:getPos().z))
		elseif key_states["w"] then
			direction = "WalkFront"
			sprite:setPos(Vec3(sprite:getPos().x, sprite:getPos().y+1,sprite:getPos().z))
		elseif key_states["a"] then
			direction = "WalkLeft"
			sprite:setPos(Vec3(sprite:getPos().x - 1, sprite:getPos().y,sprite:getPos().z))
		else
			direction = nil
		end

		if direction ~= lastDir then
			sprite:clearAnimation()
			if direction then
				sprite:playAnimation(direction)
			end
			lastDir = direction
		end
end

function rectCollision(otherCollider, playerCollider)
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