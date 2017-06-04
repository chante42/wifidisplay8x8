#ifndef PAGE_MESSAGES_H
#define PAGE_MESSAGES_H




//
//   The MESSAGE PAGE
//
const char PAGE_Message[] PROGMEM = R"=====(
  <html lang='fr'>
    <head>
    
        <meta charset='UTF-8'>
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <link href="data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAwP/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQABAREQERABAAEBAAAAEAEBAQERABEQARARAQAAEAABAAEBERAREAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" rel="icon" type="image/x-icon" />
        <title> Afficher LED</title>
        <link rel="stylesheet" href="style.css" type="text/css" />
        <script src="microajax.js"></script> 
    </head>
    <body>
      <strong>Message</strong>
      <hr>
      <form action='/submit' method='GET'>
        <p>Message à afficher:</p><input type='text' name='message'><br>
        <p>Vitesse:</p><input type='text' name='vitesse'><br>
        <p>Luminositée</p><input type='text' name='luminosite'> (-1 pour éteindre)<br>
        <p>Timer présence</p><input type='text' name='presence'><br>
        <p>font</p><input type='text' name='font'><br>
        <input type='submit' value='Submit'><br>
      </form>

      <div id="mydynamicdata">Here comes the Dynamic Data in </div>   <!-- added a DIV, where the dynamic data goes to -->
      <script>                
        window.onload = function (){
        
              setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
        
        
        }
        function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
      </script>
    </body>
  </html>  
  
)=====";
#endif



void processExample()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", ""  ); 
}