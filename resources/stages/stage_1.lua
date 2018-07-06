include("trench.lua")
include("objects.lua")


music("Juhani Junkala [Retro Game Music Pack] Level 1")
function run()
    startTrench()
    --Start with a very tight opening, just to set the mood.
    addTrench(50, 0, 5)--Close up
    addTrench(50, 0, 5)--Tight path
    --Open wide, and add a bit of a forest
    addTrench(50, 0, 20)--Opening up to the forest
    addTrench(50, 0, 20)--Leading into the forest
    addTrench(40, 0, 20)--Forest patch
    for n=1,4 do
        treeSmall(random(-8, 8), random(-40, 0))
    end
    for n=1,4 do
        treeSimple(random(-8, 8), random(-40, 0))
    end
    
    addTrench(100, 0, 20)--Small nothing before a tree line with a small forest.
    for n=1,4 do--Initial tree line
        treeSmall(-10 + n * 4 + random(-1, 1), random(-1, 1))
    end
    addTrench(50, 0, 20)--Forest patch
    for n=1,6 do
        treeSmall(random(-7, 7), random(-40, 0))
    end
    
    --ZigZag trench
    addTrench(50, 0, 8)
    for n=1,4 do
        addTrench(50, 6, 8)
        addTrench(50, -6, 8)
    end
    
    --Create some room vor the pine V
    addTrench(30, 0, 20)
    addTrench(150, 0, 20)
    for n=1,20 do   --Pine V
        pineTall(-2-n*0.3, -n*5)
        pineTall( 2+n*0.3, -n*5)
    end
    addTrench(450, 0, 20)
    for n=1,21 do
        pineShort(random(-6, 8), -n*20)
        pineSmall(random(-8, 6), -n*20+10)
    end

    addTrench(10, -10, 10)
    addTrench(50, -10, 10)
    addTrench(10,   0, 30)
    addTrench(90,   0, 30)
    addTrench(10,  10, 10)
    for n=1,8 do treePlateau(n*2-6, -n*10) end
    addTrench(50,  10, 10)
    addTrench(10,   0, 30)
    addTrench(90,   0, 30)
    addTrench(10, -10, 10)
    for n=1,8 do treePlateau(-n*2+6, -n*10) end
    addTrench(50, -10, 10)
--[[
    for n=1,100 do
        local x = random(-5, 5)
        local width = random(9, 15)
        --local x = random(-7, 7)
        --local width = random(9, 11)
        addTrench(50, x, width)
        
        addStaticObject("tree_small", 1.0, 0.4, {x + random(-width*0.4, width*0.4), random(-25, 0)})
    end
--]]
    endTrench()
end
