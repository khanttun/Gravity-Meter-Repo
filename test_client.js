const WebSocket = require('ws');

const ws = new WebSocket('ws://localhost:3000');

ws.on('open', function open() {
    console.log('Connected to server');

    // Wait a bit then send water command
    setTimeout(() => {
        console.log('Sending water command...');
        ws.send(JSON.stringify({ type: 'water' }));
    }, 1000);
});

ws.on('message', function incoming(data) {
    console.log('Received:', data.toString());
});

ws.on('close', function close() {
    console.log('Disconnected');
});

ws.on('error', function error(err) {
    console.error('Error:', err);
});

// Exit after 3 seconds
setTimeout(() => {
    console.log('Test finished, exiting.');
    process.exit(0);
}, 3000);
