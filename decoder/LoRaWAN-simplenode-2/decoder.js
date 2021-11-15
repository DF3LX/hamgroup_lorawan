function Decoder(bytes, port) {
  
    var decode = {};
  
    decode.zaehler = bytes[ 0 ];
  
    decode.bytes = bytes;
  
    return decode;
  }