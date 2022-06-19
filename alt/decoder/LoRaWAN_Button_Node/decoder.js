function Decoder(bytes, port) {
  
    var decode = {};
    decode.button = ( bytes[ 0 ] >> 4 );        // Tasterstatus
    decode.buttonTrigger = bytes[ 0 ] & 0x0F;   // Trigger = Taster betätigt
    decode.bytes = bytes;                       // nur zur Anschauung
    return decode;
  }