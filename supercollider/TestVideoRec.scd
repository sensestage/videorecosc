OSCFunc.trace( false );

n = NetAddr.new( "127.0.0.1", 57151 );

n.sendMsg( "/videorec/record", 1 );

n.sendMsg( "/videorec/record", 0 );

n.sendMsg( "/videorec/quit" );

OSCdef( \videorec_started, { |msg| msg.postln }, "/videorec/started", n );
OSCdef( \videorec_quit, { |msg| msg.postln }, "/videorec/quit", n );
OSCdef( \videorec_frame, { |msg| msg.postln }, "/videorec/frame", n );
