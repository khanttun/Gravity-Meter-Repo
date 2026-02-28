# Gravity Meter - Smart Soil Moisture Dashboard

A real-time dashboard visualizing soil moisture with a playful "Gravity Meter".

## Project Overview

- **Back-end:** Node.js + Express + WebSocket (`ws`)
- **Front-end:** HTML/CSS/JS (Vanilla)
- **Firmware:** C++ (Arduino/PlatformIO) for ESP32

The dashboard shows an icon that "falls" as the soil dries (moisture level drops). Clicking "WATER" triggers the ESP32 to activate a pump, restoring the moisture level (and the icon floats back up!).

## Quick Start
1.  **Install Dependencies:**
    ```bash
    npm install
    ```
2.  **Start Server:**
    ```bash
    npm start
    ```
3.  **Open Dashboard:**
    [http://localhost:3000](http://localhost:3000)

## Hardware Setup (ESP32)
1.  Open `firmware/main.cpp`.
2.  Update `ssid`, `password`, and `ws_host` (your PC's IP).
3.  Flash to ESP32.
