const char SERVO_page[] PROGMEM = R"=====(
  <!DOCTYPE html>
  <html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=0.5>
    <meta http-equiv="cache-control" content="no-cache, must-revalidate, post-check=0, pre-check=0" />
    <meta http-equiv="cache-control" content="max-age=0" />
    <meta http-equiv="expires" content="0" />
    <meta http-equiv="expires" content="Tue, 01 Jan 1980 1:00:00 GMT" />
    <meta http-equiv="pragma" content="no-cache" />
    <title>Servo-Controller</title>
    ###<CSS>###
  </head>
  <body>
  <div class="container">
  <h1>ServoController</h1>
  <p class="version">Version: <span id="id_version">0.00</span></p>
  </div class="container">
  <hr>
  <div class="container">
  <h2>Steuerung eines RC-Servos &uuml;ber den Browser:</h2>
  </div>
  <div class="row">
    <div class="col-left">
      <label> </label>
    </div>
    <div class="col-double">
      <label>PWM-Impulsbreite</label>
    </div>
    <div class="col-double">
      <label>Prozent</label>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label>Servoweg in :</label>
    </div>
    <div class="col-double">
      <label class=measureValue>  <span id="id_pwm_value">0</span>&micro;s</label>
    </div>
    <div class="col-double">
      <label class=measureValue>  <span id="id_percent_value">0</span>%</label>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label> modifizieren :</label>
    </div>
    <div class="col-double">
      <input type="number" class=measureInput id="id_pwm_setvalue" onchange="sendNameValue(this.id, this.value)"
         style="width: 6em" maxlength="4" min="750" max="2250" step="1" value="1500"><span class=measureInput>&micro;s</span>
    </div>
    <div class="col-double">
      <input type="number" class=measureInput id="id_percent_setvalue" onchange="sendNameValue(this.id, this.value)"
         style="width: 6em" maxlength="3" min="-125" max="+125" step="1" value="0"><span class=measureInput>%</span>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label>Positions-Slider :</label>
    </div>
    <div class="col-large">
      <div class="slidecontainer">
        <input type="range" min="-125" max="125" value="0" 
          class="slider" id="id_pos_slider"
          onchange="sendNameValue(this.id, this.value)"
          oninput="sendNameValue(this.id, this.value)">
      </div>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label for="id_start">Setzte Position :</label>
    </div>
    <div class="col-large">
      <a class=c_button id="id_load_1" name="cmd_load_1" value="true"
        onclick="sendNameValue(this.name, this.value)">
        <span id=id_load_pos_1>-</span>%</a>
      <a class=c_button id="id_load_2" name="cmd_load_2" value="true"
        onclick="sendNameValue(this.name, this.value)">
        <span id=id_load_pos_2>-</span>%</a>
      <a class=c_button id="id_load_3" name="cmd_load_3" value="true"
        onclick="sendNameValue(this.name, this.value)">
        <span id=id_load_pos_3>-</span>%</a>
      <a class=c_button id="id_load_4" name="cmd_load_4" value="true"
        onclick="sendNameValue(this.name, this.value)">
        <span id=id_load_pos_4>-</span>%</a>
      <a class=c_button id="id_load_5" name="cmd_load_5" value="true"
        onclick="sendNameValue(this.name, this.value)">
        <span id=id_load_pos_5>-</span>%</a>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label for="id_start">Position &uuml;bernehmen :</label>
    </div>
    <div class="col-large">
      <a class=c_button id="id_store_1" name="cmd_store_1" value="true"
        onclick="sendNameValue(this.id, this.name)">Save</a>
      <a class=c_button id="id_store_2" name="cmd_store_2" value="true"
        onclick="sendNameValue(this.id, this.name)">Save</a>
      <a class=c_button id="id_store_3" name="cmd_store_3" value="true"
        onclick="sendNameValue(this.id, this.name)">Save</a>
      <a class=c_button id="id_store_4" name="cmd_store_4" value="true"
        onclick="sendNameValue(this.id, this.name)">Save</a>
      <a class=c_button id="id_store_5" name="cmd_store_5" value="true"
        onclick="sendNameValue(this.id, this.name)">Save</a>
    </div>
  </div>
  <div class="row">
    <div class="col-left">
      <label>hertellerspezifische<br>Einstellungen :</label>
    </div>
    <div class="col-large">
      <label>
          <span id="id_vendor_settings">?</span>
	  (
	  min: <span id="id_pulse_width_min">?</span>&micro;s/
	  -100%: <span id="id_pulse_width_max">?</span>&micro;s/
	  +100%: <span id="id_pulse_width_n100">?</span>&micro;s/
	  max: <span id="id_pulse_width_p100">?</span>&micro;s
	  )
      </label>
    </div>
  </div>
  <hr>
  <div class="row">
    <div class="col-left">
      <label for="id_settings">
        Hier k&ouml;nnen die Einstellungen des Messsensors ge&auml;ndert werden
      </label>
    </div>
    <div class="col-large">
      <button type="button" id="id_settings" onclick="window.location.href='/adminPage'">Einstellungen &auml;ndern</button>
    </div>
  </div>
  </div class="container">
  <br><br><a href="https://github.com/Pulsar07/ServoController">Projektseite auf GitHub</a>
  ###<SCRIPT>###
  <script>
  getData("id_pwm_value", "id_percent_value", "id_pwm_setvalue", "id_percent_setvalue", "id_pos_slider",
    "id_load_pos_1", "id_load_pos_2", "id_load_pos_3", "id_load_pos_4", "id_load_pos_5",
    "id_pulse_width_min", "id_pulse_width_max", "id_pulse_width_p100", "id_pulse_width_n100", 
    "id_vendor_settings", "id_version"
  );
  </script>
  </body>
  </html>
)=====";
