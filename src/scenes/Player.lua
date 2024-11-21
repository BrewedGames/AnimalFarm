_G.player = manager:addEntity("Player")

_G.playerSprite = _G.player:addSpriteComponent()
_G.playerCollider = _G.player:addColliderComponent()

_G.currentHealth = 100;
_G.maxHealth = 100;

_G.playerSprite:loadSprite("./static/Sample_SpriteSheet.bmp", 100, 100, Vec3(40, 100, 1), true, 32, 8, 100)

_G.playerCollider:setTag("Player")
_G.playerCollider:addCapsuleCollider(_G.playerSprite:getPos().x, _G.playerSprite:getPos().y, 50, 100)

_G.playerSprite:setAnimation("WalkBack", 0, 7, 100)
_G.playerSprite:setAnimation("WalkLeft", 8, 15, 100)
_G.playerSprite:setAnimation("WalkRight", 16, 23, 100)
_G.playerSprite:setAnimation("WalkFront", 24, 31, 100)

function _G.updateHealth(damage)
    _G.currentHealth = _G.currentHealth - damage;
    if _G.currentHealth < 0 then
        _G.currentHealth = _G.maxHealth;--reset for now
    end
end

local key_states = {}

function _G.handlePlayerInput(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["s"] then
            _G.playerSprite:clearAnimation()
            _G.playerSprite:playAnimation("WalkBack")
        end
        if key_states["d"] then
            _G.playerSprite:clearAnimation()
            _G.playerSprite:playAnimation("WalkRight")
        end
        if key_states["w"] then
            _G.playerSprite:clearAnimation()
            _G.playerSprite:playAnimation("WalkFront")
        end
        if key_states["a"] then
            _G.playerSprite:clearAnimation()
            _G.playerSprite:playAnimation("WalkLeft")
        end
    elseif event.type == "keyup" then
        _G.playerSprite:clearAnimation()
        key_states[event.key] = false
    end
end

function _G.updatePlayerMovement(delta_time)

    _G.newPos = _G.playerSprite:getPos() 

    if key_states["s"] then
        _G.newPos.y = _G.newPos.y - 1
    end
    if key_states["d"] then
        _G.newPos.x = _G.newPos.x + 1
    end
    if key_states["w"] then
        _G.newPos.y = _G.newPos.y + 1
    end
    if key_states["a"] then
        _G.newPos.x = _G.newPos.x - 1
    end

    _G.playerCollider:setPos(newPos)
    _G.playerSprite:setPos(newPos)
end
