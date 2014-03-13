function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

var getOptions = function() {
	//Get info
	var response = HTTPGET("http://172.20.64.41:8000/apiquestions/1/?format=json");
		
	//Convert to JSON
	var json = JSON.parse(response);
	
	//Extract the data
	var question = json.question_text;
	var op1 = json.options[0];
	var op2 = json.options[1];
	var op3 = json.options[2];
	
	//Console output to check all is working.
	//console.log("It is " + temperature + " degrees in " + location + " today!");
	
	//Construct a key-value dictionary	
	var dict = {"KEY_QUESTION" : question, "KEY_OP1": op1, "KEY_OP2": op2, "KEY_OP3": op3};
	
	//Send data to watch for display
	Pebble.sendAppMessage(dict);

	console.log("JAVASCRIPT: Sent: " + JSON.stringify(dict));
};

Pebble.addEventListener("ready",
  function(e) {
	//App is ready to receive JS messages
	console.log("JavaScript app ready and running!");
	getOptions();
  }
);

Pebble.addEventListener("appmessage",
  function(e) {
	//Watch wants new data!
	getOptions();
  }
);