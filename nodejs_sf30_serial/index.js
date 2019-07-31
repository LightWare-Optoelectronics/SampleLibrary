const chalk = require('chalk');
const SerialPort = require('serialport');

let socket = new SerialPort('/dev/ttyUSB1', { baudRate: 921600 });
		
socket.on('open', (data) => {
	console.log('Serial opened');
});

socket.on('error', (err) => {
	console.log('lwSerialEndpoint: error ' + err.message);
});

socket.on('close', () => {
	console.log('Serial port closed');
});

let byteState = 0;
let byteH = 0;
let resultCount = 0;
let resultsPerSecond = 0;

setInterval(() => {
	//console.log(resultCount + ' Hz');
	resultsPerSecond = resultsPerSecond * 0.1 + resultCount;
	resultCount = 0;
}, 1000);

socket.on('data', (data) => {

	for (let i = 0; i < data.length; ++i) {
		let b = data[i];
		
		if (data[i] & 0x80) {
			byteState = 1;
			byteH = b & 0x7F;
		} else {
			if (byteState) {
				let distance = (byteH << 7) | b;
				console.log((distance / 100).toFixed(2) + ' m ' + Math.floor(resultsPerSecond) + '/s');
				++resultCount;
			}			
		}
	}
});
