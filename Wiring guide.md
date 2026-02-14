
# 🚗 Smart Parking System Setup Guide (ESP32)

This guide details the connections and initial setup steps required for your **Smart Parking System using ESP32**.

---

## 1️⃣ Hardware Connections

> ⚠️ **IMPORTANT:**  
> - Always double-check your ESP32 board's voltage requirements and GPIO capabilities.  
> - Use an **external 5V regulated power supply** for the servo motor.  
> - The ESP32’s internal 3.3V regulator **cannot safely power a servo under load**.

---

### 🔌 Component Wiring Table

| Component | Signal | ESP32 Pin (GPIO) | Notes |
|------------|--------|------------------|-------|
| **Ultrasonic Sensor (HC-SR04)** | VCC | 5V | Power supply |
|  | GND | GND | Ground |
|  | Trig | 5 | Digital Output (pulse trigger) |
|  | Echo | 18 | Digital Input (reflection pulse) |
| **Servo Motor (e.g., SG90)** | VCC (Red) | External 5V | ❗ DO NOT use ESP32 5V pin |
|  | GND (Brown) | GND | Must share ground with ESP32 |
|  | Signal (Orange) | 19 | PWM Output (angle control) |
| **IR Sensor (Digital)** | VCC | 3.3V or 5V | Check module specs (often 3.3V safe) |
|  | GND | GND | Ground |
|  | DO (Digital Out) | 34 | Input-only GPIO on ESP32 |

---

### ⚡ Power Supply Notes

- Servo motor must use a **separate 5V regulated supply**
- Connect **servo GND to ESP32 GND** (common ground required)
- Avoid powering high-current devices from ESP32 directly

---

## 2️⃣ Arduino IDE Setup

### ✅ Install ESP32 Board Support

1. Open **Arduino IDE**
2. Go to: `File > Preferences`
3. Add ESP32 board manager URL (if not already added)
4. Go to: `Tools > Board > Boards Manager`
5. Search for **ESP32**
6. Install **ESP32 by Espressif Systems**

---

### 📚 Required Libraries

This project uses:

- `WiFi.h` (Included with ESP32 core)
- `WebServer.h` (Included with ESP32 core)
- `ESP32Servo.h`  
  - Install via **Library Manager**
  - Search: `"ESP32Servo"`

---

### 🧠 Upload the Code

1. Open Arduino IDE
2. Create a new sketch
3. Copy contents of:
```

esp32_parking_system.ino

````
4. ⚠️ **CRITICAL:** Update Wi-Fi credentials:
```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
````

5. Select:

   * **Board:** ESP32 Dev Module
   * **Correct COM Port**
6. Click **Upload**

---

## 3️⃣ System Operation

### 📟 Monitor Serial Output

1. Open **Serial Monitor**
2. Set baud rate to: `115200`
3. ESP32 will attempt Wi-Fi connection

---

### 🌐 Get IP Address

Once connected, you’ll see something like:

```
Connected to WiFi
IP Address: 192.168.1.100
```

---

### 🖥️ Access Web Dashboard

1. Open a browser
2. Make sure device is on **same Wi-Fi network**
3. Enter:

```
http://192.168.1.100
```

(Replace with your displayed IP)

---

## 4️⃣ Testing the System

### 🚘 Parking Spot Detection

* Place an object (toy car or hand)
* Within **25 cm range**
* Dashboard should change:

```
AVAILABLE ➜ OCCUPIED
```

---

### 🚧 Gate Control

* Click **Open Gate**

* Servo should rotate

* Dashboard updates gate status

* Click **Close Gate**

* Servo returns to original position

---

### 🔴 IR Sensor Testing

* Pass an object through IR beam
* Check "IR Sensor" status in dashboard
* Status should update accordingly

---

## 🛠️ Troubleshooting Tips

* Servo not moving?

  * Check external 5V power
  * Confirm common ground
* No Wi-Fi connection?

  * Verify SSID & password
  * Check router frequency (2.4GHz required)
* Ultrasonic inaccurate?

  * Check Echo voltage level (may need voltage divider)
* IR always triggered?

  * Adjust sensitivity potentiometer

---

## 🎯 System Summary

* Real-time parking detection
* Web-based dashboard
* Remote gate control
* Live IR monitoring
* Wi-Fi enabled via ESP32

---

## 🚀 You're Ready!

Your Smart Parking System is now configured and ready to use.

Happy Building! 🎉

```
```
