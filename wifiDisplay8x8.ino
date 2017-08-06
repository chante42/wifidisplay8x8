void toto(){
}
// -=- OCH 20170718
// BUG a la con qui empeche la connection en http par moment

//
// la fct toto ne sert a rien, mais quand je l'enlÃ¨ve j'&i l'erreur suivante sans comprendre pourquoi????
// wifiDisplay8x8:119: error: expected primary-expression before 'void'
//
//  ARDUINO :
// il faut ajouter la librairy (Croquis-> Inclure une bibliothÃ¨que -> Ajouter la bibliothÃ¨que en .ZIP)
//     https://github.com/MajicDesigns/MD_MAX72XX/archive/master.zip
//     https://github.com/MajicDesigns/MD_Parola/archive/master.zip
//
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#include <Wire.h>  // This library is already built in to the Arduino IDE
#define ACCESS_POINT_NAME  "ESP-LED-8x8"        
#define ACCESS_POINT_PASSWORD  "12345678" 
//#define ADMIN_TIMEOUT 600  // Defines the Time in Seconds, when the Admin-Mode will be diabled
#define ADMIN_TIMEOUT 60


#include "helpers.h"
#include "global.h"


//
//Include the HTML, STYLE and Script "Pages"
//
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "PAGE_NetworkConfiguration.h"
#include "Page_Message.h"


// Turn on debug statements to the serial output
#define  DEBUG  1

#if  DEBUG
#define PRINT(s, x) { Serial.print(s); Serial.print(x); }
#define PRINTS(x) Serial.print(x)
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif


// Define the number of devices we have in the chain and the hardware interface
#define MAX_DEVICES 4
#define MAXLENMESSAGE  200

struct LineDefinition {
  MD_Parola  P;                   // object definition
  char    curMessage[MAXLENMESSAGE];   // message for this display
  boolean newMessageAvailable;    // true if new message arrived
};

// Add new entries for more lines.
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
struct LineDefinition  Line[] =
{
  { MD_Parola(D4, MAX_DEVICES), "abc", false }
};

#define MAX_LINES   (sizeof(Line)/sizeof(LineDefinition))


uint8_t frameDelay = 25;  // default frame delay value
textEffect_t  scrollEffect = PA_SCROLL_LEFT;

uint8_t putLine = 0;


// Capteur de prÃ©sence ne fonctionne pas
//
//  CAPTEUR      NODEMCU
//  2 (centre)   D6  - - GPIO12
int PresencePin     = D6;
int PresenceState   = LOW;             // we start, assuming no motion detected
int PresenceVal     = 0;

//
// Variable Globale
#define WIFI_CONNECTION_TIMEOUT_DEFAULT 60 // 60*500 ms 
unsigned short WifiConnectTimeOut = 0;

//WiFiServer server(80);
//ESP8266WebServer server(80); dans global.h

char  Message [MAXLENMESSAGE] ;
String messageString = "Olivier Chanteloup 2017/04/24  eÃ© eÃ¨ aÃ ";


int Luminosite = 15;
int TimerPresence = 300; // s
int Fonte = 0;
//
// dÃ©finition de l'etat du montage pour tentÃ© d'economiser de l'Ã©nergie
#define STATE_NORMAL    1
#define STATE_LOWPOWER  2
#define STATE_SLEEP     3
byte GlobalState = STATE_NORMAL;

// DÃ©claration de fonction :
void replaceVariable(void);

//
// Page envoyer lors GET Submit
//
void handleSubmitPage() {
  String tmp = "";
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "message" and server.arg("message") != "") {
        // do something here with value from server.arg(i);
        //server.arg("message").toCharArray(Message, server.arg("message").length() + 1);
        messageString = server.arg("message");
        strcpy(Message,messageString.c_str());
        PRINTS("message : |");
        PRINTS(Message);
        Serial.println("|");
        
        for (unsigned int j = 0; j < strlen(Message); j++) {
          Serial.print(Message[j], DEC);
          if (Message[j] == 195) {
            if (Message[j + 1] == 160) { // Ã 
              Message[j + 1] = 133;
            }
            else if (Message[j + 1] == 169) { // Ã©
              Message[j + 1] = 130;
            }
            else if (Message[j + 1] == 168) { //Ã¨
              Message[j + 1] = 138;
            }
            else if (Message[j + 1] == 167) { // Ã§
              Message[j + 1] = 135;
            }
            else if (Message[j + 1] == 170) { //Ãª
              Message[j + 1] = 136;
            }
            strcpy(Message + j, Message + j + 1);

          } // fin IF 195
          else if (Message[j] == 194) {// Â°
            if (Message[j + 1] == 176) {
              Message[j + 1] = 248;
            }
            strcpy(Message + j, Message + j + 1);
          } // fin if 194
        } // fin FOR
        tmp += "<hr><br>";
        tmp += "<p>Message :'" + String(Message) + "' envoyÃ©</p><br>";

        messageString = String(Message);
        replaceVariable();
        //strcpy(Line[0].curMessage,Message);
        Line[0].newMessageAvailable = true;
      } //fin if Message

      if (server.argName(i) == "vitesse" and server.arg("vitesse") != "" ) {
        Line[0].P.setSpeed( atoi(server.arg("vitesse").c_str()));
        tmp += "<p>vitesse :'" + String(server.arg("vitesse")) + "' envoyÃ©</p><br>";
      } //fin if vitesse

      if (server.argName(i) == "font" and server.arg("font") != "" ) {
        Fonte = atoi(server.arg("font").c_str());
        
        if (Fonte == 0) {
          Line[0].P.setFont(NULL);
        } else if (Fonte == 1) {
          Line[0].P.setFont(font_SINCLAIR_8x8);
        } else if (Fonte == 2) {
          Line[0].P.setFont(font_bold_8x8);
        } else if (Fonte == 3) {
          Line[0].P.setFont(font_6x8);
        } else {
          Line[0].P.setFont(NULL  );
        } 
        
        tmp += "<p>font :'" + String(server.arg("font")) + "' envoyÃ©</p><br>";
      } //fin if font

      if (server.argName(i) == "luminosite" and server.arg("luminosite") != "" ) {
        Luminosite = atoi(server.arg("luminosite").c_str());
        
        tmp += "<p>luminositÃ© :'" + String(server.arg("luminosite")) + "' envoyÃ©</p><br>";
        if (Luminosite >= 0 and Luminosite <= 15) {
          GlobalState = STATE_NORMAL;
          Line[0].P.setIntensity(Luminosite);
          Line[0].P.displayShutdown(false);
          Line[0].P.displaySuspend(false);
          Line[0].P.displayReset();
        } // fin if
        else if (Luminosite == -1 ) { // Passe en mode eteinte
          Line[0].P.displaySuspend(true);
          Line[0].P.displayShutdown(true);
          GlobalState = STATE_LOWPOWER;
        }
        else {
          Luminosite = 5;
        }
      }// fin if luminosite
      if (server.argName(i) == "timer_presence" and server.arg("timer_presence") != "" ) {
        TimerPresence = atoi(server.arg("timer_presence").c_str());
        tmp += "<p>time presence :'" + String(server.arg("timer_presence")) + "' envoyÃ©</p><br>";
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

  char luminosite_s[5];
  char timerPresence_s[5];
  char fonte_s[5];
  String values ="";
  // s'il y a des caractere accentuÃ©, il y a un Pb de rÃ©afichage dans la page html
  //values += "message|"        + (String)  message            +"|input\n";
  values += "vitesse|"        + (String) Line[0].P.getSpeed() + "|input\n";
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

  for (uint8_t i=0; i<MAX_LINES; i++)
  {
    if(Line[i].P.displayAnimate())
    {
      
      if (Line[i].newMessageAvailable)
      {
        strcpy(Line[i].curMessage, Message);
        Line[i].newMessageAvailable = false;
        PRINT("\nLine ", i);
        PRINT(" msg: \n", Line[i].curMessage);
      }
      Line[i].P.displayReset();
    }
  }

}  // end of updateDisplay

//
// Replace Variable
//
void replaceVariable(void) {
  String messageStringTmp;

  messageStringTmp = messageString;
  Serial.printf("\nmessageStringTmp : %s\n", messageStringTmp.c_str());
  messageStringTmp.replace("<hh:mm:ss>", (String) DateTime.hour + ":" + (String) + DateTime.minute +  ":"  + (String)  DateTime.second );
  messageStringTmp.replace("<jj/mm/aa>", (String) DateTime.day  + "/" + (String)  DateTime.month + "/" + (String)  DateTime.year );
  messageStringTmp.replace("<jj/mm>", (String) DateTime.day  + "/" + (String)  DateTime.month  );
  strncpy(Message, messageStringTmp.c_str(), MAXLENMESSAGE -1);
  Message[messageStringTmp.length()] = '\0';
  Message[MAXLENMESSAGE] = '\0';
  PRINTS("message:|");
  PRINTS(Message);
  PRINTS("|\n");
  Line[0].newMessageAvailable = true;
}

//
// wifiTraite
//
void wifiTraite (){
  
}
//
//   setup
//
void setup () {
  
  Serial.begin(57600);
  // We start by connecting to a WiFi network

  EEPROM.begin(512);
  delay(500);
  Serial.println("Starting ES8266");
  if (!ReadConfig())  {
    // DEFAULT CONFIG
    config.ssid                       = "freebox_tourettes";
    config.password                   = "AlbanGabriel";
    config.dhcp                       = true;
    config.IP[0]                      = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
    config.Netmask[0]                 = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
    config.Gateway[0]                 = 192;config.Gateway[1] = 168;config.Gateway[2] = 1;config.Gateway[3] = 1;
    config.ntpServerName              = "0.pool.ntp.org";
    config.Update_Time_Via_NTP_Every  =  0;
    config.timezone                   = -10;
    config.daylight                   = true;
    config.DeviceName                 = "Not Named";
    config.AutoTurnOff                = false;
    config.AutoTurnOn                 = false;
    config.TurnOffHour                = 0;
    config.TurnOffMinute              = 0;
    config.TurnOnHour                 = 0;
    config.TurnOnMinute               = 0;
    config.NbMax7219                  = 2;
    WriteConfig();
    Serial.println("General config applied");
  }
  
  PRINTS("Nb LED MAX7219 : |");
  Serial.print(config.NbMax7219);
  PRINTS("|\n");
  PRINTS("WIFI SSID : |");
  Serial.print(config.ssid);
  PRINTS("|\n");
  PRINTS("WIFI PASSWAORD : |");
  Serial.print(config.password);
  PRINTS("|\n");
  for (uint8_t i=0; i<MAX_LINES; i++)
  {
    Line[i].P.begin();
    Line[i].P.displayClear();
    Line[i].P.displaySuspend(false);

    Line[i].P.displayScroll(Line[i].curMessage, PA_LEFT, scrollEffect, frameDelay);

    strcpy(Line[i].curMessage, "Hello! Led 8 x 8 ");
  }
  
  
  Serial.print("\n[Parola Scrolling Display Multi Line]\n");
  Serial.print(MAX_LINES);
  Serial.print(" lines\nType a message for the scrolling display\nStart message with display number\nEnd message line with a newline");


  
  replaceVariable();
  updateDisplay();

  ConfigureWifi();
  

  //server.on ( "/", processExample  );
  //  http:xx.xx.xx.xx/message.html test si mode admin et dans ce cas redirect sur admin.html
  server.on ("/",             []() {  if (!AdminEnabled) { Serial.println("/ REDIRECT message.html"); server.send ( 200, "text/html", PAGE_Message );} else {Serial.println("/ Redirecte admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );}   }  );
  server.on ("/message.html", []() { Serial.println("message.html"); server.send ( 200, "text/html", PAGE_Message );   }  );
  server.on ("/mes.html",     []() { Serial.println("message.html"); server.send ( 200, "text/html", PAGE_Message );   }  );

  server.on ("/getCurrentMessage", send_message_values_html);
  server.on ("/submit",       handleSubmitPage);
  server.on ("/favicon.ico",  []() { Serial.println("favicon.ico"); server.send ( 200, "text/html", "" );   }  );
  server.on ("/admin.html",   []() { Serial.println("admin.html"); server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
  server.on ("/config.html",  send_network_configuration_html );
  server.on ("/info.html",    []() { Serial.println("info.html"); server.send ( 200, "text/html", PAGE_Information );   }  );
  server.on ("/ntp.html",     send_NTP_configuration_html  );
  server.on ("/general.html", send_general_html  );
  server.on ("/style.css",    []() { Serial.println("style.css"); server.send ( 200, "text/css", PAGE_Style_css );  } );
  server.on ("/microajax.js", []() { Serial.println("microajax.js"); server.send ( 200, "application/javascript", PAGE_microajax_js );  } );
  server.on ("/admin/values", send_network_configuration_values_html );
  server.on ("/admin/connectionstate", send_connection_state_values_html );
  server.on ("/admin/infovalues", send_information_values_html );
  server.on ("/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ("/admin/generalvalues", send_general_configuration_values_html);
  server.on ("/admin/devicename",     send_devicename_value_html);
  server.onNotFound ( []() { Serial.println("Page Not Found"); server.send ( 400, "text/html", "Page not Found" );   }  );
  server.begin();

  PRINTS( "HTTP server started\n" );

  tkSecond.attach(1,Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive

  messageString = "Mode Admin, SSID:'"+(String)ACCESS_POINT_NAME+"',password:'"+(String)ACCESS_POINT_PASSWORD+"' http://192.168.4.1";
  Line[0].newMessageAvailable = true;
  replaceVariable();

  Serial.printf("Web server started, open %s in a web browser\n", WiFi.localIP().toString().c_str());

  // initialise le detecteur de prÃ©sence
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
    if (AdminTimeOutCounter > ADMIN_TIMEOUT)
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

      while ( (WiFi.status() != WL_CONNECTED)  && (WifiConnectTimeOut > 0 ) ){
        delay(500);
        Serial.print(".");
        WifiConnectTimeOut--;
      }

      if (WifiConnectTimeOut != 0) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        messageString = "http://"+WiFi.localIP().toString() +", Bonjour nous somme le <jj/mm/aa> <hh:mm:ss>";
  
        Line[0].newMessageAvailable = true;
        // Met a jour l'horloge tout de suite 
        NTPRefresh();
      }
      else {
        AdminEnabled = true;
        AdminTimeOutCounter = 0;
        WifiConnectTimeOut = WIFI_CONNECTION_TIMEOUT_DEFAULT;
        Serial.println("");
        Serial.println("Impossible de se connectÃ© au wifi, retour en mode Admin");
        ConfigureWifi();
      }
      

    } // FIN if AdminTimeOutCounter > ADMIN_TIMEOUT
  } // fin IF  AdminEnable
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
       if ( (DateTime.hour == config.TurnOnHour) && (DateTime.minute == config.TurnOnMinute) ) {
          Serial.println("SwitchON");
       }
     }


     Minute_Old = DateTime.minute;
     if (config.AutoTurnOff)  {
       if ( (DateTime.hour == config.TurnOffHour)  && (DateTime.minute == config.TurnOffMinute) )   {
          Serial.println("SwitchOff");
       }
     }
  }

  if (GlobalState == STATE_NORMAL) {
    if (UnixTimestamp % 60 == 0) {
      replaceVariable();
    }
    // update display if time is up
      updateDisplay ();
      
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

  // permet d'afficher la bonne heure en continue
  // do other stuff here
  server.handleClient();


}  // end of loop


