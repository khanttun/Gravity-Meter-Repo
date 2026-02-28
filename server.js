const express = require('express');
const WebSocket = require('ws');
const path = require('path');

const app = express();
const PORT = 3000;

// Serve static files from the 'public' directory
app.use(express.static(path.join(__dirname, 'public')));

// Create HTTP server
const server = app.listen(PORT, () => {
    console.log(`Server is listening on port ${PORT}`);
});

// Create WebSocket server attached to the HTTP server
const wss = new WebSocket.Server({ server });

let currentMoisture = 100; // Simulated moisture level (0-100)

wss.on('connection', (ws) => {
    console.log('Client connected');

    // Send current state to new client
    ws.send(JSON.stringify({ type: 'moisture', value: currentMoisture }));

    ws.on('message', (message) => {
        try {
            const data = JSON.parse(message);
            console.log('Received:', data);

            if (data.type === 'water') {
                console.log('Water command received');
                // Simulate watering: jump to 100% immediately (or notify ESP32)
                currentMoisture = 100;
                broadcast({ type: 'moisture', value: currentMoisture });
                
                // In a real scenario, we'd forward this to the ESP32 here
                // broadcastToESP32({ type: 'pump', state: 'on' });
            } else if (data.type === 'moisture_update') {
                // Update from ESP32
                currentMoisture = data.value;
                broadcast({ type: 'moisture', value: currentMoisture });
            }
        } catch (e) {
            console.error('Error parsing message:', e);
        }
    });

    ws.on('close', () => {
        console.log('Client disconnected');
    });
});

function broadcast(data) {
    wss.clients.forEach((client) => {
        if (client.readyState === WebSocket.OPEN) {
            client.send(JSON.stringify(data));
        }
    });
}

// Simulation loop (for demo purposes if no ESP32 is connected)
// Decreases moisture over time to simulate drying
setInterval(() => {
    if (currentMoisture > 0) {
        currentMoisture -= 1; // Dry out
        broadcast({ type: 'moisture', value: currentMoisture });
    }
}, 2000); // Update every 2 seconds
