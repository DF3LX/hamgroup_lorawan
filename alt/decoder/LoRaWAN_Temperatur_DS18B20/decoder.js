function Decoder(bytes, port) {

    var decode = {};

    decode.temperatur = ( ((bytes[0] << 8) | bytes[1]) / 10 ) - 100;
    decode.temperatur = round( decode.temperatur );

    decode.bytes = bytes;

    return decode;
}

function round(num) {
    var m = Number((Math.abs(num) * 100).toPrecision(15));
    return Math.round(m) / 100 * Math.sign(num);
}