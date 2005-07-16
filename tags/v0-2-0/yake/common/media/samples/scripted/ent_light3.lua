-- a flickering light
-- state based... which is total overkill but should illustrate the point.
events.server.onSpawn = function()
	time = getSim():getTime()
	lowLimit = 0.05
	highLimit = 1.1
	timeLeft = -1
	theEntity:addState("on")
	theEntity:addState("off")
	theEntity:changeStateTo("off")
end
events.server.onTick = function()
	timeLeft = timeLeft - ( getSim():getTime() - time )
	if (timeLeft < 0) then
		timeLeft = rand(lowLimit,highLimit)
		time = getSim():getTime()
		if theEntity:getState() == "on" then
			theEntity:changeStateTo("off")
		else
			theEntity:changeStateTo("on")
		end
	end
end
states = {
	on = {
		server = {
			enter = function()
				theEntity:enable(true)
			end
		}
	},
	off = {
		server = {
			enter = function()
				print("off.server.enter")
				theEntity:enable(false)
			end,
			step = function()
				print("off.server.step")
			end,
			exit = function()
				print("off.server.exit")
			end
		}
	}
}
