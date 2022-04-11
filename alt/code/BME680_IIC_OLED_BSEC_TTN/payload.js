function Decoder(bytes, port) {

  var data = {};
  
  data.temperature = (((bytes[0] + (bytes[1] << 8))/ 100) - 100)
  data.temperature = round(data.temperature)
  data.humidity = (bytes[2] + (bytes[3] << 8)) / 100
  data.pressure = (bytes[4] + (bytes[5] << 8)) / 10
  data.iaq = (bytes[6] + (bytes[7] << 8))
  
  if (data.iaq > 0)
  {
    data.co2 = (bytes[8] + (bytes[9] << 8)) 
    data.voc = (bytes[10] + (bytes[11] << 8)) / 100
    
    if ((data.iaq > 0) && (data.iaq <= 50))
    {
      data.iaqstatus = "Good"
    }
    else if ((data.iaq > 51) && (data.iaq <= 100))
    {
      data.iaqstatus = "Average"
    }
    else if ((data.iaq > 51) && (data.iaq <= 100))
    {
      data.iaqstatus = "Little Bad"
    }
    else if ((data.iaq > 101) && (data.iaq <= 105))
    {
      data.iaqstatus = "Bad"
    }
    else if ((data.iaq > 151) && (data.iaq <= 120))
    {
      data.iaqstatus = "Worse"
    }
    else if ((data.iaq > 201) && (data.iaq <= 300))
    {
      data.iaqstatus = "Very Bad"
    }
    else if ((data.iaq > 301) && (data.iaq <= 500))
    {
      data.iaqstatus = "Very Very Bad"
    }
    
  }
  
   return {
    data: data
  };
}

function round(num) {

  var m = Number((Math.abs(num) * 100).toPrecision(15));

return Math.round(m) / 100 * Math.sign(num);

}