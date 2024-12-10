local background = manager:addEntity("Background")
local restruant = manager:addEntity("restruant")

local overlay = manager:addEntity("Overlay")

local startButton = manager:addEntity("Startbutton")
local creditButton = manager:addEntity("Creditbutton")
local exitButton = manager:addEntity("Exitbutton")


local logo = manager:addEntity("Logo")
local moon = manager:addEntity("Moon")

local buttonAudio = manager:addEntity("ButtonAudio")
local buttonAudioAccept = manager:addEntity("ButtonAudioAccept")

local audio  = background:addAudioComponent()
local buttonAudioComponent = buttonAudio:addAudioComponent()
local buttonAudioAcceptComponent = buttonAudioAccept:addAudioComponent()


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


restruantModel:loadMesh("./src/meshes/Restruant.obj", "./src/textures/restruant_texture.png", Vec3(650, 50, 0), Vec3(10, 10, 10), Vec3(90, 0, 0))


LogoSprite:loadSprite("./static/Logo.png", 360, 160.79, Vec3(250, 550, 0))
backgroundImage:loadSprite("./static/background.jpg", 1280, 720, Vec3(640, 360, 0))
overlaySprite:loadSprite("./static/Menu_Overlay.png", 1280, 720, Vec3(640, 360, 0))
moonSprite:loadSprite("./static/Moon.png", 150, 150, Vec3(1155, 600, 0))



audio:setAudio("./src/audio/Beauty Flow.mp3", true)
audio:setVolume(50)
audio:setLoop(true)
audio:Play()


buttonAudioComponent:setAudio("./src/audio/button_hover.mp3", false)
buttonAudioComponent:setVolume(100)

buttonAudioAcceptComponent:setAudio("./src/audio/Button_selected.wav", false)
buttonAudioAcceptComponent:setVolume(60)

local angle = 0

startbutton:setButtonHovered(true)
local currentActiveButton = 0

local moveCooldown = 0.2  
local moveTimer = 0      


local sceneChangePending = false
local pendingSceneName = nil
local sceneChangeDelay = 0.2 
local sceneChangeTimer = 0

function on_event(event)
    if event.type == "keydown" then
        key_states[event.key] = true
        if key_states["space"] then
           GameScene:changeScene("DemoScene")

        end
        if key_states["up"] then
            currentActiveButton = (currentActiveButton - 1) % 3
            buttonAudioComponent:Play()
        end

        if key_states["down"] then
            currentActiveButton = (currentActiveButton + 1) % 3
            buttonAudioComponent:Play()
        end


        if key_states["z"] then
            if currentActiveButton == 0 then
                startbutton:setButtonPressed(true)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Hyena"
                sceneChangeTimer = sceneChangeDelay
            elseif currentActiveButton == 1 then
                creditbutton:setButtonPressed(true)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Credits"
                sceneChangeTimer = sceneChangeDelay
            elseif currentActiveButton == 2 then
                exitbutton:setButtonPressed(true)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Exit"
                sceneChangeTimer = sceneChangeDelay
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
                RumbleController(35000, 40000, 150)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Hyena"
                sceneChangeTimer = sceneChangeDelay
            elseif currentActiveButton == 1 then
                creditbutton:setButtonPressed(true)
                RumbleController(35000, 40000, 150)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Credits"
                sceneChangeTimer = sceneChangeDelay
            elseif currentActiveButton == 2 then
                exitbutton:setButtonPressed(true)
                RumbleController(35000, 40000, 150)
                buttonAudioAcceptComponent:Play()
                sceneChangePending = true
                pendingSceneName = "Exit"
                sceneChangeTimer = sceneChangeDelay
            end
        end
    elseif event.type == "controllerbuttonup" then
        startbutton:setButtonPressed(false)
        creditbutton:setButtonPressed(false)
        exitbutton:setButtonPressed(false)
        controller_state[event.button] = false
    end
end

local swayAmplitude = 30.0  
local swaySpeed = 0.5    

function update(delta_time)

    
    local swayAngle = swayAmplitude * math.sin(swaySpeed * os.clock())


    restruantModel:setRotation(Vec3(swayAngle, 0, 0))

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

    if sceneChangePending then
        sceneChangeTimer = sceneChangeTimer - delta_time
        if sceneChangeTimer <= 0 then
            if pendingSceneName == "Exit" then
                quitGame()
            else
            GameScene:changeScene(pendingSceneName)
            sceneChangePending = false
            end
        end
    end


    if moveTimer > 0 then
        moveTimer = moveTimer - delta_time
    end

    if controller_state[SDL_CONTROLLER_AXIS_LEFTY] > 8000 and moveTimer <= 0 then
        currentActiveButton = (currentActiveButton + 1) % 3
        RumbleController(20000, 25000, 70)
        buttonAudioComponent:Play()
        moveTimer = moveCooldown
    end

    if controller_state[SDL_CONTROLLER_AXIS_LEFTY] < -8000 and moveTimer <= 0 then
        currentActiveButton = (currentActiveButton - 1) % 3
        RumbleController(20000, 25000, 70)
        buttonAudioComponent:Play()
        moveTimer = moveCooldown
    end
end
---------------------------WARNING-------------------------------
-- Update Loops anything called after Update will not be called--
-----------------------------------------------------------------