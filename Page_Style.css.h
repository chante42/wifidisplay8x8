
const char PAGE_Style_css[] PROGMEM = R"=====(

body 	{ background-color: #000000; color: #fa0909; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;} 
hr 		{ background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; } 
input 	{ color : #fa0909; background-color: #101010; border:solid 1px white; border-radius:7px; text-align:center; box-shadow:0 0 6px;}
select 	{ color : #fa0909; background-color: #101010; border:solid 1px white; border-radius:7px; text-align:right; box-shadow:0 0 6px; }

#title { font-weight: bold; text-align:center; border:solid 1px white; border-radius:12px; margin : 0.5em;}
.btn, .btn:link, .btn:visited {  

	border-radius: 0.3em;  
	border-style: solid;  
	border-width: 1px;  
	color: #ffffff;  
	display: inline-block;  
	font-family: avenir, helvetica, arial, sans-serif;  
	letter-spacing: 0.15em;  
	margin-bottom: 0.5em;  
	padding: 1em 0.75em;  
	text-decoration: none;  
	text-transform: uppercase;  
	-webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;  
	transition: color 0.4s, background-color 0.4s, border 0.4s; 
} 

.btn:hover, .btn:focus {
	color: #7FDBFF;  
	border: 1px solid #7FDBFF;  
	-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
	transition: background-color 0.3s, color 0.3s, border 0.3s; 
}

.btn:active {  
	color: #0074D9;  
	border: 1px solid #0074D9;  
	-webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
	transition: background-color 0.3s, color 0.3s, border 0.3s; 
} 

.btn--s {  
	font-size: 12px; 
}

.btn--m { 
	font-size: 14px; 
}

.btn--l {  
		font-size: 20px;  border-radius: 0.25em !important; 
} 

.btn--full, .btn--full:link {
	border-radius: 0.25em; 
	display: block;  
	margin-left: auto; 
	margin-right: auto; 
	text-align: center; 
	width: 100%; 
} 

.btn--blue:link, .btn--blue:visited {
	color: #fff;  
	background-color: #0074D9; 
}

.btn--blue:hover, .btn--blue:focus {
	color: #fff !important;  
	background-color: #0063aa;  
	border-color: #0063aa; 
}

.btn--blue:active {
	color: #fff; 
	background-color: #001F3F;  border-color: #001F3F; 
}

.bottom {
	font-size: 10px; 
}
@media screen and (min-width: 32em) {
	.btn--full {  
	max-width: 16em !important; } 
}
.infoBulle
{
  text-decoration:none;
  position:relative;
}
 
 
.infoBulle span
{
  display:none;
 
  -moz-border-radius:6px;
  -webkit-border-radius:6px;
  border-radius:6px;
  color:black;
  background:white; 
}
 
 
.infoBulle span img
{
  float:left;
  margin:0px 8px 8px 0;
}
 
 
.infoBulle:hover span
{
  display:block;
  position:absolute;
  top:0;
  left:0;
  z-index:1000;
  width:auto;
  max-width:320px;
  min-height:128px;
  border:1px solid black;
  margin-top:12px;
  margin-left:32px;
  overflow:hidden;
  padding:8px;
}
)=====";
 