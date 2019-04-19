#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

const char* ssid = "myssid"; // fill in here your router or wifi SSID
const char* password = "ssidpassword"; // fill in here your router or wifi password
const char* dnsName = "garagedoor"; // mDNS Hostname

#define RELAY 0 // relay connected to  GPIO0

ESP8266WebServer server(80);
 
void setup() 
{
  Serial.begin(115200); // must be same baudrate with the Serial Monitor
  while (!Serial) {
      delay(10);
  }
  Serial.println(dnsName);
 
  pinMode(RELAY,OUTPUT);
  digitalWrite(RELAY, HIGH);
 
  // Connect to WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if(WiFi.status() == WL_CONNECTED) { //If WiFi connected to hot spot then start mDNS
    if (MDNS.begin(dnsName)) {
      Serial.println("mDNS started");
    }
  }
 
  // Start the server
  server.on("/", handle_OnConnect);
  server.on("/BUTTON", handle_BUTTON);
  server.on("/info", handle_Info);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("Server started");
  MDNS.addService("http", "tcp", 80);

}
 
void loop() 
{
  MDNS.update();

  // Check if a client has connected
  server.handleClient();
  
  delay(500);
}

void handle_OnConnect() {
  Serial.println("new client");
  server.send(200, "text/html", SendHTML()); 
}

void handle_BUTTON() {
  Serial.println("BUTTON Pressed");
  digitalWrite(RELAY,LOW);
  delay(1000);
  digitalWrite(RELAY,HIGH);
  server.send(200, "text/html", SendHTML());
}

void handle_Info() {
  // Print Information to the System Console
  Serial.println("--- Info ---");
  Serial.print("Connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("mDNS Hostname: ");
  Serial.println(dnsName);
  if(WiFi.status() == WL_CONNECTED) { //If WiFi connected to hot spot then start mDNS
    Serial.println("WiFi.status() == WL_CONNECTED");
  } else {
    Serial.println("WiFi.status() != WL_CONNECTED");
  }
  Serial.print("Client IP: ");
  Serial.println(server.client().remoteIP());
  Serial.println("");
  
  // Send Information to the Web Interface
  server.send(200, "text/html", SendInfo());
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  ptr +="<link rel=\"icon\" href=\"data:,\">\n";
  ptr +="<style>\n";
  ptr +="html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +=".button { padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n";
  ptr +="</style>\n";
  ptr +="<title>ESP8266-01 RELAY Control</title></head>\n";
  ptr +="<body><h1>ESP8266-01 RELAY Control</h1>\n";
  ptr +="<br>\n";
  ptr +="<a href=\"/BUTTON\"><button class=\"button\">Press</button></a> GPIO0 Relay Button<br>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String SendInfo(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  ptr +="<link rel=\"icon\" href=\"data:,\">\n";
  ptr +="<style>\n";
  ptr +="html {font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +=".button { padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n";
  ptr +="</style>\n";
  ptr +="<title>ESP8266-01 RELAY Control</title></head>\n";
  ptr +="<body><h1>ESP8266-01 RELAY Control</h1>\n";
  ptr +="<br><div align=left>\n";
  ptr +="Client IP: ";
  ptr +=server.client().remoteIP().toString();
  ptr +="<br>\n";
  ptr +="Connected to: ";
  ptr +=ssid;
  ptr +="<br>\n";
  ptr +="mDNS Hostname: ";
  ptr +=dnsName;
  ptr +="<br>\n";
  ptr +="Server IP : ";
  ptr += WiFi.localIP().toString();
  ptr +="<br>\n";
  if(WiFi.status() == WL_CONNECTED) { //If WiFi connected to hot spot then start mDNS
    ptr +="WiFi.status() == WL_CONNECTED\n";
  } else {
    ptr +="WiFi.status() != WL_CONNECTED\n";
  }
  ptr +="<br>\n";
  ptr +="<br>\n";
  ptr +="mDNS.local/BUTTON<br>\n";
  ptr +="mDNS.local/info<br>\n";
  ptr +="</div><br>\n<br>\n";
  ptr +="<a href=\"/BUTTON\"><button class=\"button\">Press</button></a> GPIO0 Relay Button<br>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
