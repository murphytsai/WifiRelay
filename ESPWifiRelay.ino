#include <ESP8266WiFi.h>

const char* ssid = "";            // fill in here your router or wifi SSID
const char* password = "";  // fill in here your router or wifi password

#define RELAY 0                       // relay connected to  GPIO0
#define LED 2

WiFiServer server(80);

void setup() {
  Serial.begin(115200);  // must be same baudrate with the Serial Monitor
  
  // refer to : https://arduinoinfo.mywikis.net/wiki/ArduinoPower#More_Complex_Systems:
  digitalWrite(RELAY, HIGH); // Default: High means OFF
  pinMode(RELAY, OUTPUT);
  int relay_status = digitalRead(RELAY);
  Serial.println(relay_status);
  digitalWrite(RELAY, HIGH); // Default: High means OFF

  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  //Serial.print("https://192.168.0.178/");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request
  int value = digitalRead(RELAY);
  
  if (request.indexOf("/RELAY=ON") != -1) {
    Serial.println("RELAY=ON");
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, LOW);
    value = LOW;
  }
  else if (request.indexOf("/RELAY=OFF") != -1) {
    Serial.println("RELAY=OFF");
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED, HIGH);
    value = HIGH;
  }
  else if (request.indexOf("/DOOR_OPEN") != -1) {                                 
    Serial.println("Start DOOR_OPEN");
    digitalWrite(RELAY, LOW);
    digitalWrite(LED, LOW);
    value = LOW;

    delay(500);
    Serial.println("Stop DOOR_OPEN");
    digitalWrite(RELAY, HIGH);
    digitalWrite(LED, HIGH);
    value = 200;    
  } 

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");  //  this is a must
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP8266 RELAY Control</title></head>");
  client.print("Relay is now: ");

  if (value == HIGH) {
    // HIGH
    client.print("CLOSE");
  } else if (value == LOW) {
    // LOW
    client.print("OPEN");
  } else if (value == 200) {
    // Just trigger open door!
    client.print("DOOR IS OPEN!");
  }
  client.println("<br><br>");
  //client.println("Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<br>");
  //client.println("Turn <a href=\"/RELAY=ON\">ON</a> RELAY<br>");
  client.println("Turn <a href=\"/DOOR_OPEN\">DOOR_OPEN</a> RELAY<br>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

