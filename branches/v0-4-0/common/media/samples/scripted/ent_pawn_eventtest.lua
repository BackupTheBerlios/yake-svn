events.server.onSpawn = function()
	print("  theEntity.id=", theEntity:getId())
	print("  theEntity.age=", theEntity:getAge())

	theEntity:createEvent("custom1")
	--OR: createEvent( theEntity, "custom1" )
end
events.server.onTick = function()
	if (theEntity:getAge() == 2) then
		theEntity:fireEvent("custom1")
		--OR: fireEvent( theEntity, "custom1" )
	end
end
events.server.custom1 = function()
	print "\nevents.server.custom1() -> YAY!"
end
