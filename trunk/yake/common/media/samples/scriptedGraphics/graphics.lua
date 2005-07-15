-- Sample of using yake::graphics Lua bindings
--

-- Get TheApp object
theApp = app.getApplication();

-- Get model::Graphical
scene = theApp:getGraphicalScene();
print( "Acquired scene object" );

grWorld = theApp:getGraphicalWorld();
print ( "Got graphical world" );

-- load scene from file
scene:fromDotScene( "../../media/samples/dotScene/DotScene1.scene", grWorld );

sn = scene:getSceneNode( "Sphere01", true );
print( "Acquired scene node" );


snPosition = yake.Vector3( 500, 0, 0 );
print ( "Prepared position" );
sn:setPosition( snPosition );


