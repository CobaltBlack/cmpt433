"use strict";
/*
	DOM Basics - Showing how to use the DOM.
*/

const POLL_INTERVAL_MS = 5000
const COMMAND_DELIM = ";"

var socket = io.connect();

// Run when webpage fully loaded
$(document).ready(function() {

	// Define button behaviours

	// Register callback functions for each button
	$('#modeNone').click(	function() { sendServerCommand("mode none"); 	});
	$('#modeRock1').click(	function() { sendServerCommand("mode rock1"); 	});
	$('#modeRock2').click(	function() { sendServerCommand("mode rock2"); 	});
	$('#volumeUp').click(	function() { sendServerCommand("volume up"); 	});
	$('#volumeDown').click(	function() { sendServerCommand("volume down"); 	});
	$('#bpmUp').click(		function() { sendServerCommand("bpm up"); 		});
	$('#bpmDown').click(	function() { sendServerCommand("bpm down"); 	});
	$('#playHihat').click(	function() { sendServerCommand("play hihat"); 	});
	$('#playSnare').click(	function() { sendServerCommand("play snare"); 	});
	$('#playBass').click(	function() { sendServerCommand("play bass"); 	});

	// Incoming control messages
	socket.on('serverReply', function(data) {
		handleServerCommands(data);
	});

	// Outgoing control messages
	
	// Poll server for new data
	window.setTimeout(pollServer, POLL_INTERVAL_MS);
});

function pollServer() {
	sendServerCommand("update");
}

// Handles multiple commands seperated by COMMAND_DELIM
function handleServerCommands(data) {
	console.log("received server command:", data);
	var commands = data.split(COMMAND_DELIM);
	for (command in commands) {
		handleServerCommand(command);
	}
}

// Handles single command
function handleServerCommand(command) {
	var parsedWords = command.split(" ");
	if (parsedWords.length == 0) {
		return;
	}

	var primaryCommand = parsedWords[0];

	if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else if (primaryCommand == "") {
		
	}
	else {
		console.log("Error: Unrecognized command");
	}
}

function sendServerCommand(data) {
	socket.emit('clientCommand', data);
};
