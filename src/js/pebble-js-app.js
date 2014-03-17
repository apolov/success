function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

var getOptions = function() {
	//Get info
	var response = HTTPGET("http://172.20.63.6:8000/apiquestions/1/?format=json");
		
	//Convert to JSON
	var json = JSON.parse(response);
	
	//Extract the data
	var question = json.question_text;
	var op1 = json.options[0];
	var op2 = json.options[1];
	var op3 = json.options[2];
	
	//Construct a key-value dictionary	
	var dict = {"KEY_QUESTION" : question, "KEY_OP1": op1, "KEY_OP2": op2, "KEY_OP3": op3};
	
	//Send data to watch for display
	Pebble.sendAppMessage(dict);

	//Console output dict1 to check all is working.
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

Pebble.addEventListener("appmessage", function(e) {
  var selection = e.payload.SELECTION;
  console.log("JAVASCRIPT: Sent: "+ JSON.stringify(e.payload.SELECTION));
  switch(selection) {
    case 1:
      sendSelection("1");
      break;
    case 2:
      sendSelection("2");
      break;
    case 3:
      sendSelection("3");
      break;
  }
});

function sendSelection(sel) {
  var res;
  var req = new XMLHttpRequest();
  req.open('GET', 'http://172.20.63.6:8000/vote/'+ sel, false);
  req.onload = function(e) {
    //if(req.responseText === undefined) {
    //  Pebble.showSimpleNotificationOnPebble("SmartStart", "Error sending command");
     // sessid = null;
      //options.username = null;
      //options.password = null;
      //options.sessid = null;
      //return null;
    //}
    res = JSON.parse(req.responseText);
    //respondBack(res.Return.ResponseSummary.StatusCode);
  }
  req.send(null);
}