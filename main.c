/*
  Smart Parking System - ESP32 Arduino Sketch

  This sketch sets up an ESP32 as a Wi-Fi access point or connects to an existing network.
  It hosts a single-file web server that provides a responsive dashboard for:
  1. Monitoring parking spot occupancy using an Ultrasonic Sensor.
  2. Controlling a barrier gate using a Servo Motor.
  3. Displaying a simple status for an IR sensor (e.g., for general entry detection).

  Hardware Connections (Refer to README.md for details):
  - Ultrasonic Sensor (HC-SR04): Trig Pin, Echo Pin
  - Servo Motor (e.g., SG90): Servo Pin
  - IR Sensor (Digital Output): IR Sensor Pin

  NOTE: The HTML content includes Tailwind CSS via CDN and JavaScript for AJAX updates.
  The server provides two endpoints: "/" for the HTML and "/status" for JSON data updates.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ------------------------------------
// 1. CONFIGURATION
// ------------------------------------
// WiFi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Pin Definitions
const int TRIG_PIN = 5;  // Ultrasonic Trigger (e.g., GPIO 5)
const int ECHO_PIN = 18; // Ultrasonic Echo (e.g., GPIO 18)
const int SERVO_PIN = 19; // Servo Motor Signal (e.g., GPIO 19)
const int IR_PIN = 34;    // IR Sensor Digital Output (e.g., GPIO 34 - Input Only)

// Parking Logic Constants
const float MAX_DISTANCE_CM = 25.0; // Max distance for spot to be considered 'occupied' (adjust based on setup)
const int MAX_PARKING_DISTANCE = 400; // Max distance for the sensor in cm (HC-SR04 limit)

// Servo Constants
const int SERVO_OPEN_ANGLE = 90;  // Angle to open the barrier (e.g., 90 degrees)
const int SERVO_CLOSED_ANGLE = 0; // Angle to close the barrier (e.g., 0 degrees)

// Global State
bool isGateOpen = false;
bool isSpotOccupied = false;
unsigned long lastSensorReadTime = 0;
const long sensorInterval = 500; // Read sensor every 500ms

// ------------------------------------
// 2. GLOBALS & INITIALIZATION
// ------------------------------------
WebServer server(80);
Servo gateServo;

// ------------------------------------
// 3. ULTRASONIC SENSOR FUNCTIONS
// ------------------------------------

// Measures distance in centimeters using the ultrasonic sensor
float measureDistance() {
  // Clear the trigger pin by setting it LOW for 2 us
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigger pin HIGH for 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echo pin, returns the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance: Speed of sound = 343 m/s or 0.0343 cm/us.
  // Distance = (Time * Speed of Sound) / 2
  float distanceCm = duration * 0.0343 / 2;

  // Clamp max reading to prevent overflow issues
  if (distanceCm > MAX_PARKING_DISTANCE || distanceCm < 0) {
    distanceCm = MAX_PARKING_DISTANCE;
  }

  return distanceCm;
}

// ------------------------------------
// 4. SERVO CONTROL FUNCTIONS
// ------------------------------------

void setGate(bool open) {
  if (open) {
    gateServo.write(SERVO_OPEN_ANGLE);
    isGateOpen = true;
    Serial.println("Gate: OPEN");
  } else {
    gateServo.write(SERVO_CLOSED_ANGLE);
    isGateOpen = false;
    Serial.println("Gate: CLOSED");
  }
  // Small delay for the servo to move
  delay(500);
}

void openGate() {
  setGate(true);
}

void closeGate() {
  setGate(false);
}

// ------------------------------------
// 5. PARKING LOGIC & STATUS UPDATE
// ------------------------------------

void updateStatus() {
  float distance = measureDistance();
  int irValue = digitalRead(IR_PIN);

  if (distance < MAX_DISTANCE_CM) {
    isSpotOccupied = true;
  } else {
    isSpotOccupied = false;
  }

  Serial.printf("Distance: %.2f cm | Occupied: %s | IR Status: %s\n",
                distance, isSpotOccupied ? "YES" : "NO", irValue == LOW ? "DETECTED" : "CLEAR");
}

// ------------------------------------
// 6. WEB SERVER HANDLERS
// ------------------------------------

// Serves the main HTML dashboard
void handleRoot() {
  // The HTML content with Tailwind CSS CDN for styling and embedded JavaScript for AJAX
  const char* htmlContent = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Parking Dashboard</title>
    <!-- Tailwind CSS CDN -->
    <script src="https://cdn.tailwindcss.com"></script>
    <style>
        body { font-family: 'Inter', sans-serif; background-color: #f7f9fc; }
        .status-card { transition: all 0.3s ease; }
    </style>
</head>
<body class="p-4 md:p-8">
    <div class="max-w-4xl mx-auto">
        <h1 class="text-3xl font-bold text-gray-800 mb-6 border-b pb-2">Smart Parking System (ESP32)</h1>

        <div class="grid md:grid-cols-3 gap-6 mb-8">
            <!-- Parking Spot Status Card -->
            <div id="parkingStatusCard" class="status-card bg-white p-6 rounded-xl shadow-lg border-2 border-gray-100 md:col-span-2">
                <h2 class="text-xl font-semibold mb-4 text-gray-700 flex items-center">
                    <svg class="w-6 h-6 mr-2 text-indigo-500" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M17.657 16.657L13.414 20.9a1.998 1.998 0 01-2.828 0l-4.244-4.243a8 8 0 1111.314 0z"></path><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 11a3 3 0 11-6 0 3 3 0 016 0z"></path></svg>
                    Parking Spot Status
                </h2>
                <p class="text-gray-500 mb-4">Spot distance: <span id="distanceCm" class="font-mono text-sm bg-gray-100 px-2 py-1 rounded">-- cm</span></p>
                <div class="flex items-center space-x-3">
                    <span id="occupancyIndicator" class="w-4 h-4 rounded-full"></span>
                    <p id="occupancyText" class="text-2xl font-bold">---</p>
                </div>
            </div>

            <!-- Gate Control Card -->
            <div class="bg-white p-6 rounded-xl shadow-lg border-2 border-gray-100">
                <h2 class="text-xl font-semibold mb-4 text-gray-700 flex items-center">
                    <svg class="w-6 h-6 mr-2 text-green-500" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 15v2m-6-6v6m-6-6v6m18-6v6m-6-6v6m-6-6v6m-6-6v6M3 9a9 9 0 0118 0v7a2 2 0 01-2 2H5a2 2 0 01-2-2V9z"></path></svg>
                    Barrier Gate Control
                </h2>
                <p id="gateStatusText" class="text-lg font-medium mb-4">Status: <span class="font-bold text-gray-600">--</span></p>
                <div class="flex space-x-3">
                    <button onclick="sendCommand('open')" id="openBtn" class="bg-green-500 hover:bg-green-600 text-white font-bold py-2 px-4 rounded-lg shadow-md transition duration-150 active:scale-95">Open Gate</button>
                    <button onclick="sendCommand('close')" id="closeBtn" class="bg-red-500 hover:bg-red-600 text-white font-bold py-2 px-4 rounded-lg shadow-md transition duration-150 active:scale-95">Close Gate</button>
                </div>
            </div>
        </div>
        
        <!-- System Status Card -->
        <div class="bg-white p-6 rounded-xl shadow-lg border-2 border-gray-100">
            <h2 class="text-xl font-semibold mb-4 text-gray-700 flex items-center">
                <svg class="w-6 h-6 mr-2 text-yellow-500" fill="none" stroke="currentColor" viewBox="0 0 24 24" xmlns="http://www.w3.org/2000/svg"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 10V3L4 14h7v7l9-11h-7z"></path></svg>
                System Information
            </h2>
            <div class="grid grid-cols-2 gap-4">
                <p><strong>IR Sensor:</strong> <span id="irStatusText" class="font-medium">---</span></p>
                <p><strong>Gate Angle:</strong> <span id="gateAngleText" class="font-medium">--°</span></p>
            </div>
        </div>

    </div>

    <script>
        const API_URL = '/status';
        const PARKED_COLOR = 'bg-red-500';
        const AVAILABLE_COLOR = 'bg-green-500';
        
        async function fetchStatus() {
            try {
                const response = await fetch(API_URL);
                if (!response.ok) throw new Error('Network response was not ok');
                const data = await response.json();
                updateDashboard(data);
            } catch (error) {
                console.error("Could not fetch status:", error);
                document.getElementById('occupancyText').textContent = 'ERROR';
            }
        }

        function updateDashboard(data) {
            // 1. Parking Spot Status
            const statusCard = document.getElementById('parkingStatusCard');
            const indicator = document.getElementById('occupancyIndicator');
            const text = document.getElementById('occupancyText');
            const distanceText = document.getElementById('distanceCm');

            distanceText.textContent = `${data.distance_cm.toFixed(2)} cm`;
            
            if (data.is_occupied) {
                indicator.className = 'w-4 h-4 rounded-full ' + PARKED_COLOR;
                text.textContent = 'OCCUPIED';
                text.className = 'text-2xl font-bold text-red-600';
                statusCard.classList.remove('border-green-300');
                statusCard.classList.add('border-red-300');
            } else {
                indicator.className = 'w-4 h-4 rounded-full ' + AVAILABLE_COLOR;
                text.textContent = 'AVAILABLE';
                text.className = 'text-2xl font-bold text-green-600';
                statusCard.classList.remove('border-red-300');
                statusCard.classList.add('border-green-300');
            }

            // 2. Gate Status
            const gateText = document.getElementById('gateStatusText').querySelector('span');
            const gateAngleText = document.getElementById('gateAngleText');

            if (data.is_gate_open) {
                gateText.textContent = 'OPEN';
                gateText.className = 'font-bold text-green-500';
            } else {
                gateText.textContent = 'CLOSED';
                gateText.className = 'font-bold text-red-500';
            }
            gateAngleText.textContent = `${data.current_angle}°`;

            // 3. IR Sensor Status
            const irText = document.getElementById('irStatusText');
            if (data.ir_status == 0) {
                irText.textContent = 'OBJECT DETECTED';
                irText.className = 'font-bold text-yellow-600';
            } else {
                irText.textContent = 'CLEAR';
                irText.className = 'font-medium text-gray-500';
            }
        }

        async function sendCommand(command) {
            console.log(`Sending command: ${command}`);
            const openBtn = document.getElementById('openBtn');
            const closeBtn = document.getElementById('closeBtn');

            // Simple button disable/re-enable for feedback
            openBtn.disabled = true;
            closeBtn.disabled = true;

            try {
                // Using fetch for GET command to keep server simple, but POST is better practice
                const response = await fetch(`/gate?action=${command}`);
                if (!response.ok) throw new Error('Command failed on server');
                // The status will be updated by the next scheduled fetchStatus()
            } catch (error) {
                console.error("Error sending command:", error);
                alert('Failed to send command to ESP32!'); // Using custom modal in a real app
            } finally {
                openBtn.disabled = false;
                closeBtn.disabled = false;
                // Wait a moment before manually refreshing status to show change
                setTimeout(fetchStatus, 600);
            }
        }

        // Start fetching status updates every 1 second
        document.addEventListener('DOMContentLoaded', () => {
            fetchStatus();
            setInterval(fetchStatus, 1000);
        });
    </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", htmlContent);
}

// Serves the real-time status as JSON
void handleStatus() {
  updateStatus(); // Read sensors just before serving status

  String json = "{";
  json += "\"is_occupied\":" + String(isSpotOccupied ? "true" : "false") + ",";
  json += "\"distance_cm\":" + String(measureDistance(), 2) + ",";
  json += "\"ir_status\":" + String(digitalRead(IR_PIN)) + ","; // LOW (0) means detected, HIGH (1) means clear
  json += "\"is_gate_open\":" + String(isGateOpen ? "true" : "false") + ",";
  json += "\"current_angle\":" + String(gateServo.read());
  json += "}";

  server.send(200, "application/json", json);
}

// Handles gate commands (e.g., /gate?action=open or /gate?action=close)
void handleGateControl() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    if (action == "open") {
      openGate();
      server.send(200, "text/plain", "Gate opened.");
      return;
    } else if (action == "close") {
      closeGate();
      server.send(200, "text/plain", "Gate closed.");
      return;
    }
  }
  server.send(400, "text/plain", "Invalid action. Use /gate?action=open or /gate?action=close");
}

// ------------------------------------
// 7. SETUP AND LOOP
// ------------------------------------

void setup() {
  Serial.begin(115200);

  // Sensor Pin Setup
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW); // Start low
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT_PULLUP); // IR sensor typically works best with pullup

  // Servo Setup
  gateServo.attach(SERVO_PIN);
  closeGate(); // Ensure gate is closed on startup

  // Wi-Fi Connection
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Web Server Routing
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/gate", handleGateControl);

  // Start Server
  server.begin();
  Serial.println("HTTP Server started on port 80");
}

void loop() {
  server.handleClient();

  // Passive Status Update (the web interface fetches status via AJAX, but we update the internal state periodically)
  if (millis() - lastSensorReadTime > sensorInterval) {
    updateStatus();
    lastSensorReadTime = millis();
  }
}
