
# 🚗 ESP32 Smart Parking System

A Wi-Fi enabled Smart Parking System built using an ESP32, Ultrasonic Sensor, IR Sensor, and Servo Motor.  
The system hosts a real-time web dashboard for monitoring parking occupancy and controlling a barrier gate.

---

## 📌 Features

- 📡 Real-time parking spot detection (Ultrasonic Sensor)
- 🚧 Remote gate control (Servo Motor)
- 🔴 IR object detection status
- 🌐 Web-based dashboard (Tailwind CSS UI)
- 🔄 Live AJAX updates (no page refresh required)
- 📶 Wi-Fi connectivity via ESP32

---

## 🛠 Hardware Requirements

- ESP32 Development Board
- Ultrasonic Sensor (HC-SR04)
- Servo Motor (SG90 or similar)
- IR Sensor Module (Digital Output)
- External 5V regulated power supply (for servo)
- Jumper wires
- Breadboard (optional)

---

## 🔌 Pin Connections

| Component | Signal | ESP32 GPIO | Notes |
|------------|--------|------------|-------|
| **Ultrasonic Sensor** | VCC | 5V | Power |
| | GND | GND | Ground |
| | TRIG | GPIO 5 | Output |
| | ECHO | GPIO 18 | Input |
| **Servo Motor** | Signal | GPIO 19 | PWM |
| | VCC | External 5V | ⚠ DO NOT use ESP32 5V |
| | GND | GND | Common ground required |
| **IR Sensor** | DO | GPIO 34 | Input Only |
| | VCC | 3.3V / 5V | Check module specs |

⚠ Always connect the external 5V ground to ESP32 GND.

---

## 📦 Software Requirements

- Arduino IDE
- ESP32 Board Package (Espressif)
- Library:
  - `ESP32Servo` (Install via Library Manager)

Built-in libraries used:
- `WiFi.h`
- `WebServer.h`

---

## 🚀 Installation & Setup

### 1️⃣ Clone Repository

```bash
git clone https://github.com/Crystal-Flower/ESP32-Smart-Parking.git
````

### 2️⃣ Open Arduino Sketch

Open:

```
ESP32-Smart-Parking/esp32_parking_system.ino
```

### 3️⃣ Configure Wi-Fi

Update credentials in the sketch:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 4️⃣ Select Board & Upload

* Board: **ESP32 Dev Module**
* Select correct COM port
* Click **Upload**

---

## 🌐 Access Web Dashboard

1. Open Serial Monitor (115200 baud)
2. Note the IP address shown
3. Open browser and visit:

```
http://<ESP32_IP_ADDRESS>
```

Example:

```
http://192.168.1.100
```

---

## 🖥 Dashboard Overview

### Parking Status

* Displays:

  * Distance (cm)
  * AVAILABLE / OCCUPIED indicator
* Threshold default: 25 cm

### Gate Control

* Open Gate button
* Close Gate button
* Live angle display

### IR Sensor

* Shows:

  * OBJECT DETECTED
  * CLEAR

---

## 🔄 API Endpoints

| Endpoint             | Method | Description      |
| -------------------- | ------ | ---------------- |
| `/`                  | GET    | Dashboard HTML   |
| `/status`            | GET    | JSON status data |
| `/gate?action=open`  | GET    | Open gate        |
| `/gate?action=close` | GET    | Close gate       |

---

## 🧠 System Architecture

```
Ultrasonic Sensor → ESP32 → Web Server → Browser Dashboard
IR Sensor ---------↑
Servo Motor  ←------ Gate Control Commands
```

---

## 🛠 Troubleshooting

**Servo not moving?**

* Check external 5V supply
* Ensure common ground connection

**Wi-Fi not connecting?**

* Confirm SSID/password
* Ensure 2.4GHz network

**Ultrasonic inaccurate?**

* Check Echo voltage compatibility
* Adjust distance threshold

**IR always triggered?**

* Adjust onboard potentiometer

---

## 📸 Future Improvements

* Multiple parking slots
* Cloud database integration
* Mobile app version
* RFID-based vehicle access
* Camera-based license plate detection

---

## 📜 License

This project is open-source and available under the MIT License.

---

## 👩‍💻 Author

Crystal-Flower
GitHub: [https://github.com/Crystal-Flower](https://github.com/Crystal-Flower)

---

⭐ If you found this project useful, consider giving it a star!
