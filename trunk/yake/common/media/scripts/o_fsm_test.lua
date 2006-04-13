print("> Starting object script...");
event = {
	on_spawn = function()
		print("  script event.on_spawn")
		--ent:postCmd("go to","jack's bar")
		--OR: ev:postCmd(ent,"go to","jack's bar")
		--ent:postCmd("die")
	end,
	on_tick = function()
		print("  script event.on_tick")
	end,
	on_die = function()
		print("  script event.on_die")
	end,
	-- various:
	onArrived = function() --triggered as a result of "go to"
	end
}
fsm = {
	main = {
		states = {
			awakening,
			alive,
			dead
		},
		transitions = {
			{awakening,"spawn",alive},
			{alive,"die",dead}
		}
	}
}
state = {
	alive = {
		on_enter = function()
			print("  script state.alive.on_enter")
		end,
		on_tick = function()
			print("  script state.alive.on_tick")
		end,
		on_exit = function()
			print("  script state.alive.on_exit")
		end
	}
}
print("> Object script up.");

function main()
	print("main() calling charGoTo()")
	charGoTo("blah")
	print("main() resuming from charGoTo()")
end
