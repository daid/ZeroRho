

function startTrench()
    addTrench(100, 0, 50)
end

function endTrench()
    addTrench(50, 0, 50)
    addFinish();
    addTrench(500, 0, 50)
end
