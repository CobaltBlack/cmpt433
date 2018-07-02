"use strict";
/*
	DOM Basics - Showing how to use the DOM.
*/

var socket = io.connect();

// Run when webpage fully loaded
$(document).ready(function() {

	// Define button behaviours

	// Register a callback function for the changeBtn button:
	$('#modeNone').click(function() {
		// Log a message and call other function.
		console.log("Clicked NONE!");
		console.log("Hello world!");
		sendServerCommand("hello server");
	});

	// Incoming control messages
	socket.on('serverReply', function(data) {
		handleServerCommand(data);
	});

	// Outgoing control messages
	
});

function handleServerCommand(data) {
	console.log("received server command:", data);
}

function sendServerCommand(data) {
	socket.emit('clientCommand', data);
};
