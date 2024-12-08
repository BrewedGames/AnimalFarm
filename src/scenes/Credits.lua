local background = manager:addEntity("Background")
local overlay = manager:addEntity("Overlay")
local moon = manager:addEntity("Moon")
local creditimage = manager:addEntity("CreditImage")
local creditsImage = creditimage:addSpriteComponent()
local backgroundImage = background:addSpriteComponent()
local moonSprite = moon:addSpriteComponent()
local overlaySprite = overlay:addSpriteComponent()

backgroundImage:loadSprite("./static/background.jpg", 1280, 720, Vec3(640, 360, 0))
overlaySprite:loadSprite("./static/Menu_Overlay.png", 1280, 720, Vec3(640, 360, 0))
creditsImage:loadSprite("./static/Credits.png", 1280, 720, Vec3(640, 360, 0))
moonSprite:loadSprite("./static/Moon.png", 150, 150, Vec3(1155, 600, 0))



function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["z"] then
            GameScene:changeScene("Menu")
        end
        if key_states["escape"] then
            GameScene:changeScene("Menu")
        end
    elseif event.type == "keyup" then
        key_states[event.key] = false
    end

    if event.type == "controllerbuttondown" then
        controller_state[event.button] = true
        if controller_state["button_a"] then
            GameScene:changeScene("Menu")
        end
        if controller_state["button_b"] then
            GameScene:changeScene("Menu")
        end
    elseif event.type == "controllerbuttonup" then
        controller_state[event.button] = false
    end
end