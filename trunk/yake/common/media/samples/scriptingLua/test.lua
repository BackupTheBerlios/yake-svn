print("from a file");
a = 1;
b = 3;
print( a .. " + " .. b .. " = " .. a+b );
print("creating player...\n");
p = yake.Player();
print("setting name to YAKE\n");
p:setName('YAKE');
print("reading name from player object... player name is " .. p:getName() );
