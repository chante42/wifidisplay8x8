// Demo of MAX7219_Dot_Matrix library - sideways scrolling
// Author: Nick Gammon
// Date: 2 October 2015

// Scrolls a pixel at a time.
// https://github.com/nickgammon/bitBangedSPI
// https://github.com/SensorsIot/MAX7219-4-digit-display-Library-for-ESP8266-/tree/master/MAX7219_Dot_MatrixESP-master
//
// NTP Librairir cliente :
// https://github.com/arduino-libraries/NTPClient

// A explorer la gestion de la consomation electrique
//
// A explorer pout l'auto config Wifi
// https://github.com/tzapu/WiFiManager/blob/master/examples/AutoConnectWithFeedback/AutoConnectWithFeedback.ino
//
// https://www.john-lassen.de/en/projects/esp-8266-arduino-ide-webconfig
//
//
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <SPI.h>
#include <bitBangedSPI.h>
#include "MAX7219_Dot_Matrix.h"

/*
Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "Page_Message.h"

#define ACCESS_POINT_NAME  "ESP-LED-8x8"        
#define ACCESS_POINT_PASSWORD  "12345678" 
#define AdminTimeOut 120  // Defines the Time in Seconds, when the Admin-Mode will be diabled
#define AdminTimeOut 15


const byte NbMax7219 = 2;
// 15 chips (display modules), hardware SPI with load on D10
// -=- initialise une premiere fois et recommence dans setup() pour avoir le nombre de led parametrable dans admin.html
MAX7219_Dot_Matrix display(NbMax7219, 2);  // Chips / LOAD
  // MAX7219      NODEMCU
  // VCC          3.3v
  // GND          GND
  // Din          D7
  // CS :         D4
  // CLK          D5



// Capteur de présence ne fonctionne pas
//
//  CAPTEUR      NODEMCU
//  2 (centre)   D6  - - GPIO12
int PresencePin = D6;
int PresenceState = LOW;             // we start, assuming no motion detected
int PresenceVal = 0;

//
// Variable Globale
unsigned long lastMoved = 0;
unsigned long MOVE_INTERVAL = 30;  // mS


#define MAXLENMESSAGE  200

//WiFiServer server(80);
//ESP8266WebServer server(80); dans global.h

char  message [MAXLENMESSAGE] ;
String messageString = "Olivier Chanteloup 2017/04/24  eé eè aà";


int Luminosite = 15;
int TimerPresence = 300; // s
int Fonte = 0;
//
// définition de l'etat du montage pour tenté d'economiser de l'énergie
#define STATE_NORMAL    1
#define STATE_LOWPOWER  2
#define STATE_SLEEP     3
byte GlobalState = STATE_NORMAL;

//
// Page envoyer lors GET Submit
//
void handleSubmitPage() {
  String tmp = "";
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "message" and server.arg("message") != "") {
        // do something here with value from server.arg(i);
        //server.arg("message").toCharArray(message, server.arg("message").length() + 1);
        messageString = server.arg("message");
        strcpy(message,messageString.c_str());
        Serial.print("message : |");
        Serial.print(message);
        Serial.println("|");
        
        for (unsigned int j = 0; j < strlen(message); j++) {
          Serial.print(message[j], DEC);
          if (message[j] == 195) {
            if (message[j + 1] == 160) { // à
              message[j + 1] = 133;
            }
            else if (message[j + 1] == 169) { // é
              message[j + 1] = 130;
            }
            else if (message[j + 1] == 168) { //è
              message[j + 1] = 138;
            }
            else if (message[j + 1] == 167) { // ç
              message[j + 1] = 135;
            }
            else if (message[j + 1] == 170) { //ê
              message[j + 1] = 136;
            }
            strcpy(message + j, message + j + 1);

          } // fin IF 195
          else if (message[j] == 194) {// °
            if (message[j + 1] == 176) {
              message[j + 1] = 248;
            }
            strcpy(message + j, message + j + 1);
          } // fin if 194
        } // fin FOR
        tmp += "<hr><br>";
        tmp += "<p>message :'" + String(message) + "' envoyé</p><br>";

        messageString = String(message);
        replaceVariable();
      } //fin if message

      if (server.argName(i) == "vitesse" and server.arg("vitesse") != "" ) {
        MOVE_INTERVAL = atoi(server.arg("vitesse").c_str());
        tmp += "<p>vitesse :'" + String(server.arg("vitesse")) + "' envoyé</p><br>";
      } //fin if vitesse

      if (server.argName(i) == "font" and server.arg("font") != "" ) {
        Fonte = atoi(server.arg("font").c_str());
        display.selectFont(Fonte);
        tmp += "<p>font :'" + String(server.arg("font")) + "' envoyé</p><br>";
      } //fin if font

      if (server.argName(i) == "luminosite" and server.arg("luminosite") != "" ) {
        Luminosite  = atoi(server.arg("luminosite").c_str());
        tmp += "<p>luminosité :'" + String(server.arg("luminosite")) + "' envoyé</p><br>";
        if (Luminosite >= 0 and Luminosite <= 15) {
          display.begin ();
          display.setIntensity (Luminosite);
          GlobalState = STATE_NORMAL;
        } // fin if
        else if (Luminosite == -1 ) { // Passe en mode eteinte
          display.end();
          GlobalState = STATE_LOWPOWER;
        }
        else {
          Luminosite = 5;
        }
      }// fin if luminosite
      if (server.argName(i) == "timer_presence" and server.arg("timer_presence") != "" ) {
        TimerPresence = atoi(server.arg("timer_presence").c_str());
        tmp += "<p>time presence :'" + String(server.arg("timer_presence")) + "' envoyé</p><br>";
      } //fin if presence
    } // fin for

  } // fin if


  server.send(200, "text/html", PAGE_Message);

}// FIN handleSubmitPage()

//
//   FILL THE PAGE WITH VALUES
//

void send_message_values_html()
{

  char move_s[5];
  char luminosite_s[5];
  char timerPresence_s[5];
  char fonte_s[5];
  String values ="";
  // s'il y a des caractere accentué, il y a un Pb de réafichage dans la page html
  //values += "message|"        + (String)  message            +"|input\n";
  values += "vitesse|"        + (String) itoa(MOVE_INTERVAL, move_s,10) + "|input\n";
  values += "luminosite|"     + (String) itoa(Luminosite, luminosite_s,10)    + "|input\n";
  values += "timer_presence|" + (String) itoa(TimerPresence, timerPresence_s,10) + "|input\n";
  values += "font|"           + (String) itoa(Fonte, fonte_s,10)         + "|input\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  
}

//
// updateDisplay
//
void updateDisplay () {
  static int  messageOffset;  

  display.sendSmooth (message, messageOffset);

  // next time show one pixel onwards
  if (messageOffset++ >= (int) (strlen (message) *8)) {
    messageOffset = - config.NbMax7219 * 8;   
    replaceVariable();
    Serial.print("len mes :");
    Serial.println((int) (strlen (message) * 8));
  }
}  // end of updateDisplay

//
// Replace Variable
//
void replaceVariable() {
  String messageStringTmp;

  messageStringTmp = messageString;
  messageStringTmp.replace("<hh:mm:ss>", (String) DateTime.hour + ":" + (String) + DateTime.minute +  ":"  + (String)  DateTime.second );
  messageStringTmp.replace("<jj/mm/aa>", (String) DateTime.day  + "/" + (String)  DateTime.month + "/" + (String)  DateTime.year );
  messageStringTmp.replace("<jj/mm>", (String) DateTime.day  + "/" + (String)  DateTime.month  );
  strncpy(message, messageStringTmp.c_str(), MAXLENMESSAGE -1);
  message[messageStringTmp.length()] = '\0';
  message[MAXLENMESSAGE] = '\0';
  Serial.print("message:|");
  Serial.print(message);
  Serial.println("|");
}

//
//   setup
//
void setup () {
  
  Serial.begin(9600);
  // We start by connecting to a WiFi network

  EEPROM.begin(512);
  delay(500);
  Serial.println("Starting ES8266");
  if (!ReadConfig())  {
    // DEFAULT CONFIG
    config.ssid           = "MYSSID";
    config.password       = "MYPASSWORD";
    config.dhcp           = true;
    config.IP[0]          = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0]     = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0]     = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName  = "0.de.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  0;
    config.timezone       = -10;
    config.daylight       = true;
    config.DeviceName     = "Not Named";
    config.AutoTurnOff    = false;
    config.AutoTurnOn     = false;
    config.TurnOffHour    = 0;
    config.TurnOffMinute  = 0;
    config.TurnOnHour     = 0;
    config.TurnOnMinute   = 0;
    config.NbMax7219      = 2;
    WriteConfig();
    Serial.println("General config applied");
  }
  
  Serial.print("Nb LED MAX7219 : |");
  Serial.print(config.NbMax7219);
  Serial.println("|");
  // 15 chips (display modules), hardware SPI with load on D10
  display.reinit(config.NbMax7219, 2);  // Chips / LOAD
  // MAX7219      NODEMCU
  // VCC          3.3v
  // GND          GND
  // Din          D7
  // CS :         D4
  // CLK          D5

  
  replaceVariable();
  updateDisplay();


  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }

  ConfigureWifi();
  

  //server.on ( "/", processExample  );
  server.on("/", []() { Serial.println("message.html"); server.send ( 200, "text/html", PAGE_Message );   }  );
  server.on("/getCurrentMessage", send_message_values_html);
  server.on("/submit", handleSubmitPage);
  server.on ( "/favicon.ico",   []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );
  server.on ( "/admin.html", []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  server.on ( "/style.css", []() { Serial.println("style.css"); server.send ( 200, "text/css", PAGE_Style_css );  } );
  server.on ( "/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "application/javascript", PAGE_microajax_js );  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename",     send_devicename_value_html);
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
  server.begin();

  Serial.println( "HTTP server started" );

  tkSecond.attach(1,Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive

  if (strcmp(config.ssid.c_str(), "MYSSID") == 0) {
    messageString = "http://192.168.4.1/admin.html";
  }
  else {
    messageString = "Mode Admin : http://192.168.4.1/admin.html";
  }
  replaceVariable();

  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  // initialise le detecteur de présence
  pinMode(PresencePin, INPUT);

  // Positionne en etat normal
  GlobalState = STATE_NORMAL;

}  // end of setup





//
// loop
//
void loop () {

  if (AdminEnabled)
  {
    if (AdminTimeOutCounter > AdminTimeOut)
    {
       AdminEnabled = false;
       Serial.println("Admin Mode disabled!");
       WiFi.disconnect();
       WiFi.softAPdisconnect(true);
       WiFi.mode(WIFI_STA);
      WiFi.begin((const char *)config.ssid.c_str(), (const char *)config.password.c_str());
      //Serial.print("connection to: |");
      //Serial.print(config.ssid);
      //Serial.print("|  passwd: |");
      //Serial.print(config.password);
      //Serial.println("|");

      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }

      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      messageString = "Bonjour nous somme le <jj/mm/aa> <hh:mm:ss>";
    }
  }
  if (config.Update_Time_Via_NTP_Every  > 0 )  {
    if (cNTP_Update > 5 && firstStart)    {
      NTPRefresh();
      cNTP_Update =0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )    {
      NTPRefresh();
      cNTP_Update =0;
    }
  }

  if(DateTime.minute != Minute_Old)  {
     Minute_Old = DateTime.minute;
     if (config.AutoTurnOn)     {
       if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute) {
          Serial.println("SwitchON");
       }
     }


     Minute_Old = DateTime.minute;
     if (config.AutoTurnOff)  {
       if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)   {
          Serial.println("SwitchOff");
       }
     }
  }

  if (GlobalState == STATE_NORMAL) {
    // update display if time is up
    if (millis () - lastMoved >= MOVE_INTERVAL) {
      updateDisplay ();
      lastMoved = millis ();
    }
  }

  // gestion du capteur infra rouge, mais ne focntionne pas ?????
  PresenceVal = digitalRead(PresencePin);
  if (PresenceVal == HIGH) {
    //   Serial.println("Hight");
    if (PresenceState == LOW) {
      PresenceState = HIGH;
      Serial.println("Motion detected!");
      Serial.println("PresenceVal = Hight");
    }
  }// fin PresenceVal
  else {
    //    Serial.println("Low");
    if (PresenceState == HIGH) {
      PresenceState = LOW;
      Serial.println("Motion ended!");
      Serial.println("PresenceVal = Low");
    }
  } //fin else presenceVAL

  // do other stuff here
  server.handleClient();



}  // end of loop

