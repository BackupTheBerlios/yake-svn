-- a flickering light. in constrast to ent_light.lua this one is event based.
events.server.onSpawn = function()
	addTimer(0) -- returns an id which we ignore here.
	lowLimit = 0.05
	highLimit = 1.1
end
events.server.onTick = function()
end
events.server.onTimer = function(timerId)
	theEntity:enable( not theEntity:isEnabled() )
	local timeUntilNextChange = rand(lowLimit,highLimit)
	addTimer(timeUntilNextChange)
end