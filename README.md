## BME280 Sensors with ESP32 BLE Server Integration

A **serverless connection to a web page via Bluetooth** enables a web application to directly communicate with nearby Bluetooth-enabled devices using the browser, without requiring a backend server. This is typically achieved using the **Web Bluetooth API**, which allows the web page (served from anywhere, even static hosting) to connect, read, and write data to Bluetooth Low Energy (BLE) devices.

**Example Use Case:** A static HTML page connects to an IoT sensor or ESP32 board via BLE to read temperature or control LEDs, all locally without internet or server involvement.

**Key Points:**

* No server or cloud needed; logic runs entirely in the browser.
* Works with Bluetooth Low Energy (BLE) devices.
* Enabled in Chrome and some Chromium-based browsers.


**The repository structure has 2 folders:**
  * for ESP32 project integrated with BME 280 sensors (temperature/humidity/pressure/altitude)
  * HTML static page allows for serverless BLE connection using Web Bluetooth API allows websites to communicate with Bluetooth devices

**Sample of static HTML reading sensors values in real time:**

![image](https://github.com/user-attachments/assets/f7244125-adc4-4929-a644-922c4ed6cb79)

