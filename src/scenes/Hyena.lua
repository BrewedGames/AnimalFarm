local background = manager:addEntity("Background")
local backgroundImage = background:addSpriteComponent()
backgroundImage:loadSprite("./static/hyena_map.jpg", 1920, 1080, Vec3(350, 200, 0))

req("player")
local playerData = initPlayer()
local playerSprite, playerCollider = playerData.sprite, playerData.collider