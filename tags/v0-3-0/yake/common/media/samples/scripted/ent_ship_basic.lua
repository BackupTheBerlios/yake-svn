--state.idle.message.timeout = function()
--	changeStateTo(..)
--end


events.server.onSpawn = function()
	print "\n(ship) events.server.onSpawn()"
	theEntity:createGraphical("../../media/samples/dotScene/DotScene1.scene");
	theEntity:getGraphical():setPosition(0,50,0)
end
events.server.onTick = function()
	print "\n(ship) events.server.onTick()"
end
