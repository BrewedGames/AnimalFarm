-- script.lua
local result = add(10, 20)
print("Addition result:", result)

print_message("Hello from Lua in a separate file!")
-- Lua script
local entity = manager:addEntity("MyEntity")
print(entity:getID())  -- Print the entity's ID
entity:setName("UpdatedEntity")

-- Optionally, add components if you've exposed them
entity:addComponent("SpriteComponent")

