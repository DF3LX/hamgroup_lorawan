String SendHTML(float Temperature, float Humidity, float Pressure, float IAQ, float Carbon, float VOC, const char* IAQsts) {
String html = "<!DOCTYPE html>";
html += "<html>";
html += "<head>";
html += "<title>BME680 Webserver</title>";
html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.7.2/css/all.min.css'>";
html += "<link rel='stylesheet' type='text/css' href='styles.css'>";
html += "<style>";
html += "body { background-color: #fff; font-family: sans-serif; color: #333333; font: 12px Helvetica, sans-serif box-sizing: border-box;}";
html += "#page { margin: 18px; background-color: #fff;}";
html += ".container { height: inherit; padding-bottom: 18px;}";
html += ".header { padding: 18px;}";
html += ".header h1 { padding-bottom: 0.3em; color: #f4a201; font-size: 25px; font-weight: bold; font-family: Garmond, 'sans-serif'; text-align: center;}";
html += "h2 { padding-bottom: 0.2em; border-bottom: 1px solid #eee; margin: 2px; text-align: center;}";
html += ".box-full { padding: 18px; border 1px solid #ddd; border-radius: 1em 1em 1em 1em; box-shadow: 1px 7px 7px 1px rgba(0,0,0,0.4); background: #fff; margin: 18px; width: 300px;}";
html += "@media (max-width: 494px) { #page { width: inherit; margin: 5px auto; } #content { padding: 1px;} .box-full { margin: 8px 8px 12px 8px; padding: 10px; width: inherit;; float: none; } }";
html += "@media (min-width: 494px) and (max-width: 980px) { #page { width: 465px; margin 0 auto; } .box-full { width: 380px; } }";
html += "@media (min-width: 980px) { #page { width: 930px; margin: auto; } }";
html += ".sensor { margin: 10px 0px; font-size: 2.5rem;}";
html += ".sensor-labels { font-size: 1rem; vertical-align: middle; padding-bottom: 15px;}";
html += ".units { font-size: 1.2rem;}";
html += "hr { height: 1px; color: #eee; background-color: #eee; border: none;}";
html += "</style>";

//Ajax Code Start
  html += "<script>\n";
  html += "setInterval(loadDoc,1000);\n";
  html += "function loadDoc() {\n";
  html += "var xhttp = new XMLHttpRequest();\n";
  html += "xhttp.onreadystatechange = function() {\n";
  html += "if (this.readyState == 4 && this.status == 200) {\n";
  html += "document.body.innerHTML =this.responseText}\n";
  html += "};\n";
  html += "xhttp.open(\"GET\", \"/\", true);\n";
  html += "xhttp.send();\n";
  html += "}\n";
  html += "</script>\n";
  //Ajax Code END

  html += "</head>";
html += "<body>";
html += "<div id='page'>";
html += "<div class='header'>";
html += "<h1>BME680 IAQ Monitoring System</h1>";
html += "</div>";
html += "<div id='content' align='center'>";
html += "<div class='box-full' align='left'>";
html += "<h2>";
html += "IAQ Status: ";
html += IAQsts;
html += "</h2>";
html += "<div class='sensors-container'>";

//For Temperature
html += "<div class='sensors'>";
html += "<p class='sensor'>";
html += "<i class='fas fa-thermometer-half' style='color:#0275d8'></i>";
html += "<span class='sensor-labels'> Temperature </span>";
html += Temperature;
html += "<sup class='units'>°C</sup>";
html += "</p>";
html += "<hr>";
html += "</div>";

//For Humidity
html += "<p class='sensor'>";
html += "<i class='fas fa-tint' style='color:#0275d8'></i>";
html += "<span class='sensor-labels'> Humidity </span>";
html += Humidity;
html += "<sup class='units'>%</sup>";
html += "</p>";
html += "<hr>";

//For Pressure
html += "<p class='sensor'>";
html += "<i class='fas fa-tachometer-alt' style='color:#ff0040'></i>";
html += "<span class='sensor-labels'> Pressure </span>";
html += Pressure;
html += "<sup class='units'>hPa</sup>";
html += "</p>";
html += "<hr>";

//For VOC IAQ
html += "<div class='sensors'>";
html += "<p class='sensor'>";
html += "<i class='fab fa-cloudversify' style='color:#483d8b'></i>";
html += "<span class='sensor-labels'> IAQ </span>";
html += IAQ;
html += "<sup class='units'>PPM</sup>";
html += "</p>";
html += "<hr>";

//For C02 Equivalent
html += "<p class='sensor'>";
html += "<i class='fas fa-smog' style='color:#35b22d'></i>";
html += "<span class='sensor-labels'> Co2 Eq. </span>";
html += Carbon;
html += "<sup class='units'>PPM</sup>";
html += "</p>";
html += "<hr>";

//For Breath VOC
html += "<p class='sensor'>";
html += "<i class='fas fa-wind' style='color:#0275d8'></i>";
html += "<span class='sensor-labels'> Breath VOC </span>";
html += VOC;
html += "<sup class='units'>PPM</sup>";
html += "</p>";


html += "</div>";
html += "</div>";
html += "</div>";
html += "</div>";
html += "</div>";
html += "</body>";
html += "</html>";
return html;
}