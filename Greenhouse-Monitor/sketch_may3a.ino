/*********
  Elliott Mills, 
  Widely using the software of Rui Santos
  Complete Santos project details at:
  https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  https://randomnerdtutorials.com/esp32-esp8266-web-server-physical-button/
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "DHT.h"
#include <Servo.h>

#define SERVOPIN 17    // Digital pin connected to the servo
#define DHTTYPE DHT22  // DHT 22 sensor type
#define DHTPIN 22      // Digital pin connected to the DHT sensor
#define SOILPIN 34     // ADC pin connected to the capacitive soil sensor

// WIFI creadentials
const char *ssid = "4_acton";        // set wifi ID
const char *password = "REDACTED";  // can't incude my wifi password now

float h = 0;  // humiditiy
float t = 0;  // temperature
float m = 0;  // soil moisture level

bool makeVentOpen = 0;  // boolean to record a command coming in to open/close the vent
bool isVentOpen = 0;    // boolean to record the state of the vent

Servo servo1;              // create servo class instance
DHT dht(DHTPIN, DHTTYPE);  // Instantiate dht with set pin and type

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>Greenhouse Monitoring System</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>Greenhouse Plant Monitor</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Vent State</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
    </div>
  </div>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperature</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units">&deg;C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Humidity</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">&percnt;</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Soil Moisture</span>
    <span id="soil moisture">%MOISTURE%</span>
    <sup class="units">&percnt;</sup>
  </p>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("moisture").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/moisture", true);
  xhttp.send();
}, 10000 ) ;

</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  ws.textAll(String(makeVentOpen));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0) {
      makeVentOpen = !makeVentOpen;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/////////////////// additions ///////////////////
String processor(const String &var) {
  Serial.println(var);
  if (var == "STATE") {
    if (makeVentOpen) {
      return "OPEN";
    } else {
      return "CLOSED";
    }
  } else if (var == "TEMPERATURE") {
    return String(t);
  } else if (var == "HUMIDITY") {
    return String(h);
  } else if (var == "MOISTURE") {
    return String(m);
  }
  return String();
}

// opens the vent (turns servo 180 degrees), saving the state afterwards
void OpenVent() {
  for (int posDegrees = 0; posDegrees <= 180; posDegrees++) {
    servo1.write(posDegrees);
  }
  isVentOpen = true;
}

// closes the vent (turns servo 180 degrees), saving the state afterwards
void CloseVent() {
  for (int posDegrees = 180; posDegrees >= 0; posDegrees--) {
    servo1.write(posDegrees);
  }
  isVentOpen = false;
}

// Returns the analogue reading of the sensor as a percentage
float soilMoistureLevel() {
  // values are backward to intuition, 2700 is dry, 900 is wet

  int moistureRead = analogRead(SOILPIN);           // read the value from the sensor
  int percentageWetIndicator = (2700 - 900) / 100;  // from data gathered in field testing

  if (moistureRead > 2700)                                       // if over 2700 reading
    return 1;                                                    // return 1%... dry
  else if (moistureRead > 900 && moistureRead < 2700)            // if within normal range
    return 100 - (moistureRead - 900) / percentageWetIndicator;  // return percetage within the range (inverted)
  else if (moistureRead < 900)                                   // if under 900
    return 100;                                                  // damn that's damn!
  else                                                           // gotta catch those rogue states...
    return 0;                                                    // 0 represents an error, this shouldn't be able to happen
}

void setup() {
  Serial.begin(115200);     // Begin Serial port for debugging
  servo1.attach(SERVOPIN);  // initialise the servo
  dht.begin();              // initialise the DHT22
  ///////////////////    end    ///////////////////


  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();

  /////////////////// additions ///////////////////
  delay(2000);  // Wait a few seconds for slow sensors (DHT has 250ms readtime avg, 2s max) and less data transfer

  h = dht.readHumidity();     // read humidity
  t = dht.readTemperature();  // Read temperature as Celsius (the default)
  m = soilMoistureLevel();    // read humidity

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // print readings to serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Raw Moisture Pin Reading: "));
  Serial.println(analogRead(SOILPIN));

  if (makeVentOpen && !isVentOpen) {         // if make open command comes and it's closed
    OpenVent();                              // open it
  } else if (!makeVentOpen && isVentOpen) {  // if close command and it's open
    CloseVent();                             // close it
  } else {                                   // else do nothing
  }
  ///////////////////    end    ///////////////////
}