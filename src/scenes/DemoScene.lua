local background = manager:addEntity("Background")
local backgroundImage = background:addSpriteComponent()
backgroundImage:loadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350, 200, 0))
print("Hello From DemoScene")

local h = 100
local HealthUI = manager:addEntity("HealthUI")
local healthUI = HealthUI:addTextComponent()
healthUI:Text(tostring(h), 16, Vec3(600.0, 0.0, 0.0), Vec3(1.0, 1.0, 1.0), Vec3(1.0, 1.0, 1.0))

local CONTROLLERTHRESHOLD = 8000

req("player")
initPlayer()
function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true

        if key_states["down"] then setPlayerDown(true) end
        if key_states["right"] then setPlayerRight(true) end
        if key_states["up"] then setPlayerUp(true) end
        if key_states["left"] then setPlayerLeft(true) end
        if event.key == "z" then triggerDash() end

    elseif event.type == "keyup" then
        key_states[event.key] = false

        if event.key == "down" then setPlayerDown(false) end
        if event.key == "right" then setPlayerRight(false) end
        if event.key == "up" then setPlayerUp(false) end
        if event.key == "left" then setPlayerLeft(false) end
    end
    if event.type == "controllerbuttondown" then
        controller_state[event.button] = true
        if controller_state["button_a"] then triggerDash() end
        
    elseif event.type == "controllerbuttonup" then
        controller_state[event.button] = false
    end
end

function update(delta_time)

    h = h - 1
    healthUI:setText(tostring(h))


    if math.abs(controller_state["right_stick_x"]) > CONTROLLERTHRESHOLD or
        math.abs(controller_state["right_stick_y"]) > CONTROLLERTHRESHOLD then
        updateArrowPosition()
    end
    if controller_state["left_stick_x"] > CONTROLLERTHRESHOLD then
        setPlayerRight(true)
    elseif controller_state["left_stick_x"] < -CONTROLLERTHRESHOLD then
        setPlayerLeft(true)
    else
        setPlayerRight(false)
        setPlayerLeft(false)
    end
    if controller_state["left_stick_y"] > CONTROLLERTHRESHOLD then
        setPlayerDown(true)
    elseif controller_state["left_stick_y"] < -CONTROLLERTHRESHOLD then
        setPlayerUp(true)
    else
        setPlayerDown(false)
        setPlayerUp(false)
    end
    updatePlayer(delta_time)
end
---------------------------WARNING-------------------------------
-- Update Loops anything called after Update will not be called--
-----------------------------------------------------------------
