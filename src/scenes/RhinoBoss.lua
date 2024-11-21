--local background = manager:addEntity("Background")
local player = _G.player
local rhino = manager:addEntity("Rhino")
local skull = manager:addEntity("Skull")
local rhinoSprite = rhino:addSpriteComponent()
local rhinoCollider = rhino:addColliderComponent()

--local backgroundImage = background:addSpriteComponent()

local angle = 0

rhinoSprite:loadSprite("./static/rhena.png", 100, 100, Vec3(500, 500, 0))
--backgroundImage:loadSprite("./static/rhena_map.jpg", 1280, 720, Vec3(640, 360, -100))

rhinoCollider:setTag("Rhino")
rhinoCollider:addCircleCollider(rhinoSprite:getPos().x, rhinoSprite:getPos().y, 60)

function on_event(event)

    _G.handlePlayerInput(event)
end

function update(delta_time)
    _G.updatePlayerMovement(delta_time)
    if _G.playerCollider.isColliding(rhinoCollider) then
        print("a")
    end
    angle = angle + 30.0 * delta_time
    
end
