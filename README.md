# Basic-Heater-control-system
 This project implements a Heating Control System using an ESP32 microcontroller. It monitors temperature and humidity using the DHT22 sensor and controls a heater, fan, and
 buzzer based on predefined thresholds. The current status is displayed on a 16x2 I2C LCD and logged to Serial.

# Key Features:- 
Temperature-based state tracking (Idle, Heating, Stabilizing, Target Reached, Overheat)- Serial logging of sensor readings and system state- LCD output display- Visual feedback with a buzzer during overheat condition- FreeRTOS task for non-blocking periodic execution .

# Hardware Components- 
 -ESP32 Dev Module
 - DHT22 Temperature and Humidity Sensor
 - I2C 16x2 LCD Display (address: 0x27)
- Buzzer
- 2-channel Relay Module (Fan and Heater control)
- Jumper wires and Breadboar

# Serial Output Example
 [Temp: 24.00C] [Humidity: 40.00%] [Heater: ON] [State: HEATING]
 [Temp: 25.20C] [Humidity: 41.00%] [Heater: OFF] [State: TARGET_REACHED]
 [Temp: 36.10C] [Humidity: 39.50%] [Heater: OFF] [State: OVERHEAT
 
