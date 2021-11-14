function Decoder(bytes, port) {
  
    var decode = {};
  
    decode.zaehler = bytes[ 0 ];
  
    decode.zufallszahl = bytes[ 1 ];
  
    decode.laufzeit = bytes[ 2 ];
  
    return decode;
  }