include("trench.lua")


music("Juhani Junkala [Retro Game Music Pack] Level 1")
function run()
    startTrench()
    
    for n=1,100 do
        local x = random(-5, 5)
        local width = random(9, 15)
        --local x = random(-7, 7)
        --local width = random(9, 11)
        addTrench(50, x, width)
        
        addStaticObject("tree_small", 1.0, 0.4, {x + random(-width*0.4, width*0.4), random(-25, 0)})
    end
    
    endTrench()
end
