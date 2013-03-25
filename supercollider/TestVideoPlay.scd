n = NetAddr.new( "127.0.0.1", 57151 );

OSCdef( \videoplay_started, { |msg| msg.postln }, "/videoplay/started", n );
OSCdef( \videoplay_quit, { |msg| msg.postln }, "/videoplay/quit", n );

n.sendMsg( "/videoplay/frame", 20 );

Tdef( \player, { 100.do{ |i| n.sendMsg( "/videoplay/frame", i ); 0.1.wait; } } );

Tdef( \player, { (1..100).scramble.do{ |i| n.sendMsg( "/videoplay/frame", i ); 0.1.wait; } } );

Tdef( \player ).play;

n.sendMsg( "/videoplay/quit" );
