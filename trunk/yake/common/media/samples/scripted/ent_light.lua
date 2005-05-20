events.server.onSpawn = function()
	time = getSim():getTime()
	lowLimit = 0.05
	highLimit = 1.2
	timeLeft = -1
end
events.server.onTick = function()
	timeLeft = timeLeft - ( getSim():getTime() - time )
	if (timeLeft < 0) then
		timeLeft = rand(lowLimit,highLimit)
		time = getSim():getTime()
		theEntity:enable( not theEntity:isEnabled() )
	end
end
