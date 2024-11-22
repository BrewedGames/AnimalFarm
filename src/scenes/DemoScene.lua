local background = manager:addEntity("Background")
local backgroundImage = background:addSpriteComponent()
backgroundImage:loadSprite("./static/sample_background.jpg", 1920, 1080, Vec3(350, 200, 0))
print("Hello From DemoScene")

-- as you can see you can access globals set in the previous scene
print(a_very_important_constant)

function on_event(event)

         key_states[event.key] = true
         if key_states["space"] then
            -- print("space")
         end

 end
 
 
 
 function update(delta_time)
    --print("test")
 end
 ---------------------------WARNING-------------------------------
 -- Update Loops anything called after Update will not be called--
 -----------------------------------------------------------------