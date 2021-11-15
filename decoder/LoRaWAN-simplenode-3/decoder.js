function Decoder(bytes, port) {
  
    var decode = {};
  
    decode.zaehler = ((bytes[0] << 8) | bytes[1]);
  
    decode.zufallszahl = bytes[ 2 ];
  
    decode.laufzeit = ((bytes[3] << 8) | bytes[4]);
  
    decode.bytes = bytes;
  
    return decode;
  }