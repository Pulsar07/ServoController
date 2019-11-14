const char ADMIN_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
  <title>Servo-Controller</title>
  ###<CSS>###
  </head>
  <body>
  <div id="so_fa">
  <div class="container">
  <h1>Servo-Controller</h1>
  <p>Version: <span id="id_version">0.10</span></p>

  <h2>Einstellungen</h2>

  <hr>
  <div class="container">
  <h3>WiFi - Einstellungen</h3>
  <p>&Auml;nderungen des WiFi-Einstellungen m&uuml;ssen zuerst gespeichert werden und wirken sich erst nach Neustart aus.</p>
  <h4>Zugangsdaten zur Verbindung mit WLAN:</h4>
  <div class="row">
    <div class="col-25">
      <input type="text" id="id_wlanSsid" name="ssid"
        onchange="sendNameValue(this.id, this.value)" placeholder="WLAN SSID">
    </div>
    <div class="col-75">
      <label for="ssid">WLAN - SSID (max. 15 Zeichen)</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
        <input type="text" id="id_wlanPasswd" name="password"
	  onchange="sendNameValue(this.id, this.value)" placeholder="WLAN Passwort">
    </div>
    <div class="col-75">
        <label for="password">WLAN - Passwort (max. 63 Zeichen) </label>
    </div>
  </div>
    <p>WLAN ist: <span id="id_wlanConnetion">nicht verbunden</span></p>
  </div>
  <div class="container">
    <h4>Zugangsdaten f&uuml;r AccessPoint  / 192.168.4.1 : </h4>
  <div class="row">
    <div class="col-25">
      <input type="checkbox" id="id_apActive" name="id_apActive" value="xx"
        onchange="sendNameValue(this.id, this.checked)" >
    </div>
    <div class="col-75">
      <label for="id_apActive"> AccessPoint ist aktiv</label>
    </div>
  </div>
   <div class="row">
    <div class="col-25">
      <input type="text" id="id_apSsid" name="apssid"
        onchange="sendNameValue(this.id, this.value)" placeholder="AccessPoint SSID">
    </div>
    <div class="col-75">
      <label for="id_apSsid">AccesPoint - SSID (max. 15 Zeichen)</label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
      <input type="text" id="id_apPasswd" name="id_apPasswd"
        onchange="sendNameValue(this.id, this.value)" placeholder="AccessPoint Passwort">
    </div>
    <div class="col-75">
     <label for="id_apPasswd">
	     Passwort des AccessPoint (AP) Zuganges (min. 8, max. 64 Zeichen) </label>
    </div>
  </div>
  </div>
<hr>
  <div class="container">
  <div class="row">
    <div class="col-25">
      <button type="button" id="id_save" name="cmd_saveConfig" value="yes" onclick="sendAll(); sendNameValue(this.name, this.value)">
      Speichern</button>
    </div>
    <div class="col-75">
     <label for="id_save">
      Speichert die Einstellungen
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
    <button type="button" id="id_reset" name="cmd_resetConfig" value="yes" onclick="sendNameValue(this.name, this.value)">
      Alles zur&uuml;cksetzten</button>
    </div>
    <div class="col-75">
      <label for="id_reset">
        Setzt alles Einstellungen zur&uuml;ck, WLAN Zugangsdaten werden gel&ouml;scht
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
	    <button type="button" id="id_backToRoot" onclick="back()">Zur&uuml;ck</button>
    </div>
    <div class="col-75">
      <label for="id_backToRoot">
         zur&uuml;ck zur Hauptseite, ohne Speichern
     </label>
    </div>
  </div>
  <div class="row">
    <div class="col-25">
    <button type="button" id="id_mcrestart" name="cmd_mcrestart" value="yes" onclick="sendNameValue(this.name, this.value)">
      Neustart Controller</button>
    </div>
    <div class="col-75">
      <label for="id_mcrestart">
        Startet den Sensor neu
     </label>
    </div>
  </div>
  <p>Alle &Auml;nderungen wirken sich sofort aus und k&ouml;nnen mit dem &quot;Zur&uuml;ck&quot;-Button benutzt/getestet werden. Ausnahmen sind WiFi-Daten, die sich erst nach &quot;Speichern&quot; und &quot;Neustart&quot; auswirken.</p>
  <p>Sollen Einstellungen (auch Kalibrierung) nach einem Neustart erhalten bleiben, muss auf jeden Fall &quot;Speichern&quot; geklickt werden.</p>
  </div>
</div>

  ###<SCRIPT>###
  <script>

  function back() {
    sendAll();
    setTimeout(backToRoot, 200, );
  }

  function backToRoot() {
    window.location.href='/';
  }

  function sendAll() {
    sendValueForId("id_wlanSsid");
    sendValueForId("id_wlanPasswd");
    sendValueForId("id_apPasswd");
  }

  getData(
    "id_version",
    "id_invertAngle",
    "nm_anglePrecision",
    "nm_referenceAxis",
    "id_invertAmplitude",
    "nm_precisionAmplitude",
    "nm_distancetype",
    "id_wlanSsid",
	"id_apSsid",
    "id_apActive",
    "id_wlanConnetion",
    "id_resp_calibrate");
  </script>
  </body>
  </html>
)=====";
