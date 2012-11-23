n = NetAddr.new( "127.0.0.1", 57151 );

OSCdef( \videorec_started, { |msg| msg.postln }, "/videorec/started", n );
OSCdef( \videorec_quit, { |msg| msg.postln }, "/videorec/quit", n );
OSCdef( \videorec_record_on, { |msg| msg.postln }, "/videorec/record/start", n );
OSCdef( \videorec_record_off, { |msg| msg.postln }, "/videorec/record/stop", n );
OSCdef( \videorec_frame, { |msg| msg.postln }, "/videorec/frame", n );

n.sendMsg( "/videorec/record", 1 );

n.sendMsg( "/videorec/record", 0 );

n.sendMsg( "/videorec/quit" );

