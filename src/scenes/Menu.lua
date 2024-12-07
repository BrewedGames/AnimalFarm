local background = manager:addEntity("Background")
local restruant = manager:addEntity("restruant")

local overlay = manager:addEntity("Overlay")

local startButton = manager:addEntity("Startbutton")
local creditButton = manager:addEntity("Creditbutton")
local exitButton = manager:addEntity("Exitbutton")


local logo = manager:addEntity("Logo")
local moon = manager:addEntity("Moon")



local audio  = background:addAudioComponent()


local LogoSprite = logo:addSpriteComponent()
local backgroundImage = background:addSpriteComponent()
local moonSprite = moon:addSpriteComponent()
local overlaySprite = overlay:addSpriteComponent()




local restruantModel = restruant:addMeshComponent()

local startbutton = startButton:addButtonComponent()
local creditbutton = creditButton:addButtonComponent()
local exitbutton = exitButton:addButtonComponent()
startbutton:loadButton("./static/Start_Button.png", "./static/Start_Button_Hovered.png", "./static/Start_Button_Pressed.png", Vec2(120, 350), 0.2)
creditbutton:loadButton("./static/Credits_Button.png", "./static/Credits_Button_Hovered.png", "./static/Credits_Button_Pressed.png", Vec2(120, 400), 0.2)
exitbutton:loadButton("./static/Exit_Button.png", "./static/Exit_Button_Hovered.png", "./static/Exit_Button_Pressed.png", Vec2(120, 450), 0.2)


restruantModel:loadMesh("./src/meshes/Restruant.obj", "./src/textures/restruant_texture.png", Vec3(650, 100, 0), Vec3(10, 10, 10), Vec3(90, 0, 0))


LogoSprite:loadSprite("./static/Logo.png", 360, 160.79, Vec3(250, 550, 0))
backgroundImage:loadSprite("./static/background.jpg", 1280, 720, Vec3(640, 360, 0))
overlaySprite:loadSprite("./static/Menu_Overlay.png", 1280, 720, Vec3(640, 360, 0))
moonSprite:loadSprite("./static/Moon.png", 150, 150, Vec3(1155, 600, 0))


audio:setAudio("./src/audio/Celeste_Original_Soundtrack_First_Steps.mp3", true)
audio:setVolume(50) 
--audio:Play()

local angle = 0

startbutton:setButtonHovered(true)
local currentActiveButton = 0

local moveCooldown = 0.2  
local moveTimer = 0      




function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["space"] then
           GameScene:changeScene("Rhino")

        end
        if key_states["up"] then
            currentActiveButton = (currentActiveButton - 1) % 3
        end

        if key_states["down"] then
            currentActiveButton = (currentActiveButton + 1) % 3
        end


        if key_states["z"] then
            if currentActiveButton == 0 then
                startbutton:setButtonPressed(true)
                GameScene:changeScene("Hyena")
            elseif currentActiveButton == 1 then
                creditbutton:setButtonPressed(true)
                GameScene:changeScene("Credits")
            elseif currentActiveButton == 2 then
                exitbutton:setButtonPressed(true)
            end
        end
    elseif event.type == "keyup" then
        startbutton:setButtonPressed(false)
        creditbutton:setButtonPressed(false)
        exitbutton:setButtonPressed(false)
        key_states[event.key] = false
    end

    if event.type == "controllerbuttondown" then
        controller_state[event.button] = true
        if controller_state["button_a"] then
            if currentActiveButton == 0 then
                startbutton:setButtonPressed(true)
                GameScene:changeScene("Hyena")
            elseif currentActiveButton == 1 then
                GameScene:changeScene("Credits")
                creditbutton:setButtonPressed(true)
            elseif currentActiveButton == 2 then
                exitbutton:setButtonPressed(true)
            end
        end
    elseif event.type == "controllerbuttonup" then
        startbutton:setButtonPressed(false)
        creditbutton:setButtonPressed(false)
        exitbutton:setButtonPressed(false)
        controller_state[event.button] = false
    end
end



function update(delta_time)

    
    angle = angle + 10.0 * delta_time

    restruantModel:setRotation(Vec3(angle, 0, 0))

    if currentActiveButton == 0 then
        startbutton:setButtonHovered(true)
        creditbutton:setButtonHovered(false)
        exitbutton:setButtonHovered(false)
    elseif currentActiveButton == 1 then
        startbutton:setButtonHovered(false)
        creditbutton:setButtonHovered(true)
        exitbutton:setButtonHovered(false)
    elseif currentActiveButton == 2 then
        startbutton:setButtonHovered(false)
        creditbutton:setButtonHovered(false)
        exitbutton:setButtonHovered(true)
    end




    if moveTimer > 0 then
        moveTimer = moveTimer - delta_time
    end

    if controller_state[SDL_CONTROLLER_AXIS_LEFTY] > 8000 and moveTimer <= 0 then
        currentActiveButton = (currentActiveButton + 1) % 3
        print("Moving down with left stick")
        moveTimer = moveCooldown
    end

    if controller_state[SDL_CONTROLLER_AXIS_LEFTY] < -8000 and moveTimer <= 0 then
        currentActiveButton = (currentActiveButton - 1) % 3
        print("Moving up with left stick")
        moveTimer = moveCooldown
    end
end
---------------------------WARNING-------------------------------
-- Update Loops anything called after Update will not be called--
-----------------------------------------------------------------