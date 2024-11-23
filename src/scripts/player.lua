print("hello from the player script")



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