events = {}
events.client = {}
events.server = {}

events.server.onSpawn = function()
	time = getSim():getTime()
end
events.server.onTick = function()
	local delta = getSim():getTime() - time
	if (delta > 1) then
		time = getSim():getTime()
		theEntity:enable( not theEntity:isEnabled() )
	end
end
