"use strict";
$(function() {
  var BAUD_RATE = 9600;
  var arduino = new SerialDevice("COM4", BAUD_RATE);
  var intervalId = 1;

  arduino.onConnect.addListener(function() {
    console.log("Connected to: " + arduino.path);
  });

  arduino.onReadLine.addListener(function(line) {
	  var mode, condition, i;
	  var shutters = [];
	  var shutters_desc = [];
	  console.log(line);
	  var attr = line.split(",");
	  if (parseInt(attr[0]) == 999) {
		  switch (parseInt(attr[1])) {
			  case 0:
				mode = "Automatic";
				break;
			  case 1:
				mode = "Manual";
				break;
			  default:
				mode = "Unknown";
		  }
		  var light_level = parseInt(attr[2]);
		  var light_threshold = parseInt(attr[3]);
		  switch (parseInt(attr[4])) {
			  case 0:
				condition = "Dark";
				break;
			  case 1:
				condition = "Sunny";
				break;
			  default:
				condition = "Unknown";
		  }
		  shutters[0] = parseInt(attr[5]);
		  shutters[1] = parseInt(attr[6]);
		  shutters[2] = parseInt(attr[7]);
		  shutters[3] = parseInt(attr[8]);
		  for (i = 0; i < shutters.length; ++i){
			  switch (shutters[i]) {
				  case 1:
					shutters_desc[i] = "is running up";
					break;
				  case 2:
					shutters_desc[i] = "is running down";
					break;
				  case 3:
				    shutters_desc[i] = "is up";
					break;
				  case 4:
				    shutters_desc[i] = "is down";
					break;
				  default:
				    shutters_desc[i] = "unknown";
			  }
			  //console.log(shutters_desc[i]);
		  }
		  //console.log("array length before: " + shutters_desc.length);
		  updateUI(mode, light_level, light_threshold, condition, shutters_desc);
		  //console.log("Mode: " + mode);
	  }
  });
  
  function updateUI(mode, light_level, light_threshold, condition, shutters_desc) {
	  var i;
	  //console.log("array length after: " + shutters_desc.length);
	  document.getElementById("mode").innerText = mode;
	  document.getElementById("light_level").innerText = light_level;
	  document.getElementById("light_threshold").innerText = light_threshold;
	  document.getElementById("condition").innerText = condition;
	  for (i = 0; i < shutters_desc.length; ++i) {
		  //console.log("array contents: " + shutters_desc[i]);
		  document.getElementById("shutter".concat(i + 1)).innerText = shutters_desc[i];
	  }
  }
  
  arduino.connect();
  
  $("[type=button]").on("click", function(event){
    var buttonType = $(event.currentTarget).attr("id"); 
    //console.log("Button pressed: " + buttonType);
    arduino.send(buttonType + "\n");
  });
  
  //setTimeout(function(){arduino.send("b\n");}, 1000);
  
  window.addEventListener('focusin', function() {
    intervalId = setInterval(function(){arduino.send("z\n");}, 1000);
  });
	
  window.addEventListener('focusout', function() {
    clearInterval(intervalId);
  });
});

