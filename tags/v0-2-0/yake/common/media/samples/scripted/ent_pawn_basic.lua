--state.idle.message.timeout = function()
--	changeStateTo(..)
--end


events.server.onSpawn = function()
	print "\n(pawn) events.server.onSpawn()"
	print("  theEntity.id=", theEntity:getId())
	print("  theEntity.age=", theEntity:getAge())

	theEntity:createEvent("custom1")
	--OR: createEvent( theEntity, "custom1" )
	
	theEntity:createGraphical("../../media/samples/dotScene/DotScene1.scene");
	theEntity:getGraphical():setPosition(0,50,0)
end
events.server.onTick = function()
	print "\n(pawn) events.server.onTick()"
	print("  current time=", getSim():getTime())
	print("  theEntity.age=", theEntity:getAge())
	evt = theEntity:getEventByName("custom1")
	
	if (theEntity:getAge() > 2) then
		theEntity:fireEvent("custom1")
		--OR: fireEvent( theEntity, "custom1" )
	end
end
events.server.custom1 = function()
	print "\n(pawn) events.server.custom1() -> YAY!"
end
