load_texture("sprites/food_man")
load_texture("sprites/food_worms")
load_texture("sprites/food_rat")

interactive_point(16, 9.5, 5.5, "talk/ola.lua")
interactive_point(42, 12.5, 19.5, "talk/cook.lua")
interactive_point(42, 11.5, 19.5, "talk/food.lua")
interactive_point(42, 12.5, 21.5, "talk/food.lua")
interactive_point(44, 4.5, 25.5, "talk/doctor.lua")
interactive_point(44, 3.5, 24.5, "talk/nurse.lua")

function update_food_texture()
    local food_texture = "sprites/food"
    if sanity < 25 then
        food_texture = "sprites/food_man"
    elseif sanity < 50 then
        food_texture = "sprites/food_worms"
    elseif sanity < 75 then
        food_texture = "sprites/food_rat"
    end
    change_texture(42, 2, food_texture)
    change_texture(42, 3, food_texture)
end

function sanity_change()
    update_food_texture()
end
