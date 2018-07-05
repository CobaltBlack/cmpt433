"use strict";
/*
	DOM Basics - Showing how to use the DOM.
*/

const POLL_INTERVAL_MS = 1000;
const UPDATE_TIMEOUT = 1000;
const ERROR_DISPLAY_TIME = 2000;
const COMMAND_DELIM = ";"

var socket = io.connect();

var lastUpdateTimeBeatbox = Date.now();
var lastUpdateTimeNodejs = Date.now();

var errorTimeout;

// Run when webpage fully loaded
$(document).ready(function() {

	// Register callback functions for each button
	$('#modeNone').click(	function() { setServerBeatMode("none");  });
	$('#modeRock1').click(	function() { setServerBeatMode("rock1"); });
	$('#modeRock2').click(	function() { setServerBeatMode("rock2"); });

	$('#volumeUp').click(	function() { setServerVolume(getDisplayVolume() + 5); });
	$('#volumeDown').click(	function() { setServerVolume(getDisplayVolume() - 5); });

	$('#bpmUp').click(		function() { setServerBpm(getDisplayBpm() + 5); });
	$('#bpmDown').click(	function() { setServerBpm(getDisplayBpm() - 5); });

	$('#playHihat').click(	function() { sendServerCommand("play hihat"); });
	$('#playSnare').click(	function() { sendServerCommand("play snare"); });
	$('#playBass').click(	function() { sendServerCommand("play bass");  });

	// Incoming control messages
	socket.on('serverReply', function(data) {
		handleServerCommands(data);
	});
	
	// Poll server for new data
	pollServer();
});

//
// Beat mode
//
const MODE_ID_TO_DISPLAY = {
	"none": "None",
	"rock1": "Rock #1",
	"rock2": "Rock #2",
}

function setServerBeatMode(newMode) {
	sendServerCommand("mode " + newMode); 
}

function setDisplayBeatMode(newMode) {
	$('#modeid').html(MODE_ID_TO_DISPLAY[newMode]);
}

//
// BPM functions
//
function setServerBpm(newBpm) {
	sendServerCommand("bpm " + newBpm); 
}

function getDisplayBpm() {
	return parseInt($('#bpmId').attr("value"));
}

function setDisplayBpm(newBpm) {
	$('#bpmId').attr("value", parseInt(newBpm));
}

//
// Volume functions
//
function setServerVolume(newVolume) {
	sendServerCommand("volume " + newVolume); 
}

function getDisplayVolume() {
	return parseInt($('#volumeId').attr("value"));
}

function setDisplayVolume(newVolume) {
	$('#volumeId').attr("value", parseInt(newVolume));
}

//
// Server functions
//
function setUptime(uptime) {
	// uptime is returned as number of seconds
	// Parse it into hours, minutes, seconds
	var totalSeconds = parseInt(uptime);
	var hours = Math.floor(totalSeconds / 3600);
	totalSeconds %= 3600;
	var minutes = Math.floor(totalSeconds / 60);
	var seconds = totalSeconds % 60;

	$("#status").html("Device uptime: " + hours + ":" + minutes + ":" + seconds);

	lastUpdateTimeBeatbox = Date.now();
}

function pollServer() {
	sendServerCommand("update");
	window.setTimeout(pollServer, POLL_INTERVAL_MS);

	if ((Date.now() - lastUpdateTimeNodejs) > UPDATE_TIMEOUT) {
		setError("No response from Node.js server. Is it running?")
	}
	else if ((Date.now() - lastUpdateTimeBeatbox) > UPDATE_TIMEOUT) {
		setError("No response from beatbox application. Is it running?")
	}
}

function sendServerCommand(data) {
	socket.emit('clientCommand', data);
};

//
// Handling server commands
//

// Handles multiple commands seperated by COMMAND_DELIM
function handleServerCommands(data) {
	var commands = data.split(COMMAND_DELIM);
	for (var i in commands) {
		handleServerCommand(commands[i]);
	}
}

// Handles single command
function handleServerCommand(command) {
	var parsedWords = command.split(" ");
	if (parsedWords.length == 0) {
		return;
	}

	var primaryCommand = parsedWords[0];
	var subCommand = parsedWords[1];

	if (primaryCommand == "mode") {
		setDisplayBeatMode(subCommand);
	}
	else if (primaryCommand == "volume") {
		setDisplayVolume(subCommand);
	}
	else if (primaryCommand == "bpm") {
		setDisplayBpm(subCommand);
	}
	else if (primaryCommand == "uptime") {
		setUptime(subCommand);
	}
	else if (primaryCommand == "error") {
		setError(subCommand);
	}
	else if (primaryCommand == "nodejsping") {
		lastUpdateTimeNodejs = Date.now();
	} 
	else {
		console.log("Error: Unrecognized command %s", primaryCommand);
	}
}

function setError(errorMsg) {
	$("#error-text").html(errorMsg);
	$('#error-box').show();

	clearTimeout(errorTimeout);
	errorTimeout = window.setTimeout(function() { $('#error-box').hide(); }, ERROR_DISPLAY_TIME);
}
