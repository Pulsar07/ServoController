const char CSS[] PROGMEM = R"=====(
<style>
    * {
      box-sizing: border-box;
    }

    h2, h3, h4, p {
      padding: 0px 0px 0px 40px;
    }

    input[type=text], select, textarea {
      width: 100%;
      padding: 5px;
      border: 1px solid #ccc;
      border-radius: 4px;
      resize: vertical;
    }

    input[type=checkbox].config, input[type=radio].config, input[type=number].config {
      margin: 5px 12px 5px 5px;
      border: 1px solid #ccc;
      border-radius: 4px;
      display: block;
    }

    label {
      padding: 6px 12px 6px 0;
      display: block;
    }

    label.input {
      border: 1px solid #ccc;
      border-radius: 4px;
      display: inline-block;
    }
    input[type=submit] {
      background-color: #4CAF50;
      color: white;
      padding: 12px 20px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      float: right;
    }

    button {
      padding: 3px;
    }

    button:hover {
      background-color: #99f;
    }

    .container {
      border-radius: 5px;
      background-color: #f2f2f2;
      padding: 8px;
    }

    .measureRemark {
      font-size: .8em;
    }

    .measureValue, .flightphaseValue {
      font-size: 1.5em;
      font-weight:bold;
    }

    .measureInput {
      font-size: 1.3em;
      font-weight:bold;
    }

    .version {
      text-align: right;
    }

    .col-large {
      text-align: left;
      float: right;
      width: 65%;
      padding-left: 5px;
    }

    .col-large input[type=number] {
      width: 5em;
    }

    .col-double {
      text-align: center;
      float: left;
      width: 30%;
      padding-left: 5px;
    }

    .col-small {
      text-align: center;
      float: left;
      width: 32.5%;
      padding-left: 5px;
      background-color: #ccc;
    }

    .col-left {
      text-align: right;
      float: left;
      width: 25%;
    }

    .row {
      background-color: #fff;
      margin: 5px 0px 5px 0px;
    }

    .row:after {
     /*  margin: 2px */
      content: "";
      display: table;
      clear: both;
    }

    a.c_button {
      display:inline-block;
      padding:0.3em 0.8em;
      margin:0 0.3em 0.3em 0;
      width: 14.2%;
      border-radius:2em;
      box-sizing: border-box;
      text-decoration:none;
      font-weight:500;
      color:#000000;
      background-color: #4CAF50;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
      text-align:center;
      transition: all 0.2s;
    }


    a.c_button:hover {
      opacity: 1;
    }

    /* Responsive layout - when the screen is less than 600px wide, make the two columns stack on top of each other instead of next to each other
    @media screen and (max-width: 600px) {
    */
    @media (max-aspect-ratio: 8/5) {
      body {
        background: #66f;
      }

      .col-small, .col-double, .col-left, .col-large, input[type=submit], input[type=number] {
        text-align: left;
        width: 100%;
        margin-top: 0;
      }

      h2, h3, h4, p {
        padding: 0px 0px 0px 0px;
      }

      a.c_button{
       display:block;
       margin:0.2em;
      }
    }

.switch {
 position: relative;
 display: inline-block;
 width: 60px;
 height: 34px;
}

/* Hide default HTML checkbox */
.switch input {
 opacity: 0;
 width: 0;
 height: 0;
}

.slidecontainer {
  width: 80%;
}

.slider {
  -webkit-appearance: none;
  width: 100%;
  height: 25px;
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider:hover {
  opacity: 1;
}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  background: #4CAF50;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: 25px;
  height: 25px;
  background: #4CAF50;
  cursor: pointer;
}

</style>
)=====";
