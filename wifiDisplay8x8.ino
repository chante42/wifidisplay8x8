// Demo of MAX7219_Dot_Matrix library - sideways scrolling
// Author: Nick Gammon
// Date: 2 October 2015

// Scrolls a pixel at a time.
// https://github.com/nickgammon/bitBangedSPI
// https://github.com/SensorsIot/MAX7219-4-digit-display-Library-for-ESP8266-/tree/master/MAX7219_Dot_MatrixESP-master

#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>  // This library is already built in to the Arduino IDE

#include <SPI.h>
#include <bitBangedSPI.h>
#include <MAX7219_Dot_Matrix.h>

const byte NbMax7219 = 8;
const char* ssid = "Bbox-0E7760"; // put your router name
const char* password = "n0uswifi12";// put your password 
 

// 12 chips (display modules), hardware SPI with load on D10
MAX7219_Dot_Matrix display(NbMax7219, 2);  // Chips / LOAD 
// MAX7219      NODEMCU
// VCC          3.3v
// GND          GND
// Din          D7
// CS :         D4
// CLK          D5

unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 30;  // mS
unsigned int Luminosite = 15;
int  messageOffset;
#define MAXLENMESSAGE  200

//WiFiServer server(80);
ESP8266WebServer server(80);

char  message [MAXLENMESSAGE] = "Olivier Chanteloup 2017/04/24  eé eè aà";

String getHtmlPageHeader() {
  String  page   = "<html lang='fr'>";
          page  += "<head>";
          page  +=    "<meta charset='UTF-8'>";
          page  +=    "<title> Afficher LED</title>";
          page  +=    "<style> body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
          page  += "</head>";
          page  += "<body>";
  return page;
}
              
String getHtmlPageMiddle() {
  String  page  =     "<form action='/submit' method='GET'>";
          page  +=      "<p>Message à afficher:</p><input type='text' name='message'><br>";
          page  +=      "<p>Vitesse:</p><input type='text' name='vitesse'><br>";          
          page  +=      "<p>Luminositée</p><input type='text' name='luminosite'><br>";          
          page  +=      "<input type='submit' value='Submit'><br>";
          page  +=    "</form>";
  return page;
}

String getHtmlPageFooter() { 
  String  page =  "</body>";
          page += "</html>";
  return page;
}
        
//
//   setup
//
void setup (){
  display.begin ();

  strncpy(message,"get IP ....", MAXLENMESSAGE -1);
  updateDisplay();
  Serial.begin(115200);
  delay(100);
  
 // We start by connecting to a WiFi network
 
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
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  strncpy(message,"ip: ", MAXLENMESSAGE -1);
  strncat(message, WiFi.localIP().toString().c_str(), MAXLENMESSAGE -1);
  message[MAXLENMESSAGE -2] = 0;

  server.on("/", handleRootPage);
  server.on("/submit", handleSubmitPage);
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());
}  // end of setup


//
// updateDisplay
//
void updateDisplay (){
  display.sendSmooth (message, messageOffset);
  
  // next time show one pixel onwards
  if (messageOffset++ >= (int) (strlen (message) * 8)) {
    messageOffset = - NbMax7219 * 8;
  }
}  // end of updateDisplay

//
// Page envoyer lors GET /
//
void handleRootPage(){
   server.send(200, "text/html", getHtmlPageHeader() + getHtmlPageMiddle() + getHtmlPageFooter());
}

//
// Page envoyer lors GET Submit
//
void handleSubmitPage(){
  String tmp = "";
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "message" and server.arg("message") != "") {
         // do something here with value from server.arg(i);
         server.arg("message").toCharArray(message, server.arg("message").length()+1);
         Serial.println("message : ");  
         Serial.println(message);
         for (int j = 0; j < strlen(message); j++) {
            Serial.print(message[j], DEC);   
            if (message[j] == 195) {
              if (message[j+1] == 160) { // à
                message[j+1] = 133;
              }
              else if (message[j+1] == 169) { // é
                message[j+1] = 130;
              }
              else if (message[j+1] == 168) { //è 
                message[j+1] = 138;
              }
              else if (message[j+1] == 167) { // ç
                message[j+1] = 135;
              }
              else if (message[j+1] == 170) { //ê
                message[j+1] = 136;
              }
              strcpy(message + j, message + j +1);
              
            } // fin IF 195
            else if (message[j] == 194) {// °
              if (message[j+1] == 176) {
                message[j+1] = 248;
              }
              strcpy(message + j, message +j +1);
            } // fin if 194
         } // fin FOR 
         tmp += "<p>message :'"+ String(message)+"' envoyé</p><br>";
      } //fin if
      if (server.argName(i) == "vitesse" and server.arg("vitesse") != "" ) {
        MOVE_INTERVAL = atoi(server.arg("vitesse").c_str());
        tmp += "<p>vitesse :'"+ String(server.arg("vitesse"))+"' envoyé</p><br>";
      } //fin if vitesse
      if (server.argName(i) == "luminosite" and server.arg("luminosite") != "" ) {
        Luminosite  = atoi(server.arg("luminosite").c_str());
        tmp += "<p>luminosité :'"+ String(server.arg("luminosite"))+"' envoyé</p><br>";
        if (Luminosite >=0 and Luminosite <= 15) {
          display.setIntensity (Luminosite);
        } // fin if 
        else {
          Luminosite = 5;
        }
      }// fin if luminosite
   } // fin for
   tmp += "<hr><br>";
  } // fin if
  
  
   server.send(200, "text/html", getHtmlPageHeader()+ tmp +getHtmlPageMiddle() +getHtmlPageFooter());

}

//
// loop
//
void loop () { 
  // update display if time is up
  if (millis () - lastMoved >= MOVE_INTERVAL) {
    updateDisplay ();
    lastMoved = millis ();
  }

  // do other stuff here    
  server.handleClient();
  
  
}  // end of loop

