#ifndef PAGE_MESSAGES_H
#define PAGE_MESSAGES_H




//
//   The MESSAGE PAGE
//
const char PAGE_Message[] PROGMEM = R"=====(
<html lang='fr'>
    <head>
        <meta name='viewport' content='width=device-width, initial-scale=1' />
        <meta http-equiv='Content-Type' content='text/html; charset=utf-8' />
        <link href='data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAwP/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQABAREQERABAAEBAAAAEAEBAQERABEQARARAQAAEAABAAEBERAREAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA' rel='icon' type='image/x-icon' />
        <title> Afficher LED</title>
        <link rel='stylesheet' href='style.css' type='text/css' />
        <script src='microajax.js'></script> 
    </head>
    <body>

      <div id='title'>Message</div>
      <hr>
      <form action='submit' method='GET'>
        <p><div class="infoBulle" href="">Message à  afficher:
           <span><h3>les variables spéciales</h3>
            <p>&lsaquo;jj/mm/aa&rsaquo; : Affiche la date courante selon le format de la balise</p>
            <p>&lsaquo;hh:mm:ss&rsaquo; : Affiche l'heure courante selon le format de la balise</p>
           </span>
          </div>
        </p>
        <input id='message' type='text' name='message' maxlength="200" size="40"><br>
        
        <p>Vitesse :
        <select  id='vitesse' name='vitesse'>
          <option value="480">escargot</option>
          <option value="240">tortue</option>
          <option value="120">très lent</option>
          <option value="60">lent</option>
          <option value="30">normal</option>
          <option value="0">très vite</option>
          <option value="15">vite</option>          
        </select></p>

        <p>Luminositée :
        <select  id='luminosite' name='luminosite'>
          <option value="-1">Eteindre</option>
          <option value="0">0</option>
          <option value="1">1</option>
          <option value="2">2</option>
          <option value="3">3</option>
          <option value="4">4</option>
          <option value="5">5</option>
          <option value="6">6</option>
          <option value="7">7</option>
          <option value="8">8</option>
          <option value="9">9</option>
          <option value="10">10</option>
          <option value="11">11</option>
          <option value="12">12</option>
          <option value="13">13</option>
          <option value="14">14</option>
          <option value="15">15</option>
        </select></p>
        
        <p>Timer présence :
        <select  id='timer_presence' name='timer_presence'>
          <option value="30">30 s</option>
          <option value="60">1 mn </option>
          <option value="180">3 mn</option>
          <option value="300">5 mn</option>
          <option value="600">10 mn</option>
          <option value="900">15mn </option>
          <option value="1800">30 mn</option>
          <option value="3200">1 h</option>
        </select></p>
        
        <p>font :
        <select  id='font' name='font'>
          <option value="0">default</option>
          <option value="1">SINCLAIR_8x8</option>
          <option value="2">font_bold_8x8</option>
          <option value="3">font_6x8</option>
        </select></p><br>
        
        <p>Rotation :
        <select  id='rotation' name='rotation'>
          <option value="0">Normal</option>
          <option value="90">90° droite </option>
          <option value="180">180 <option>
          <option value="300">320</option>
        </select></p>
        
        <input type='submit' value='Submit' style='width:150px' class='btn btn--m btn--blue'><br>
      </form>
      <hr>
      <a href='admin.html' class='bottom'>page d'administration</a>
      <script>                
        window.onload = function (){
        
              setValues('/getCurrentMessage');  //-- this function calls the function on the ESP      
        
        
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


