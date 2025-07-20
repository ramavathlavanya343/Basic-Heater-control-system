#include <DHTesp.h>  // For temperature sensor
#include <LiquidCrystal_I2C.h> // For LCD display

const int DHT_PIN = 15;     // DHT22 data pin
const int fanPin = 2;       // Fan (overheat control)
const int heaterPin = 4;    // Heater control pin
const int buzzerPin = 5;    // Buzzer for overheat warning

const float TARGET_TEMP = 25.0;     // Target temperature (°C)
const float OVERHEAT_TEMP = 35.0;   // Overheat threshold
const float HYSTERESIS = 0.5;       // To avoid rapid toggling


enum State { 
  
IDLE, 
HEATING,
STABILIZING,
TARGET_REACHED, 
OVERHEAT, 

  };
State currentState = IDLE;

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight(); // Initialize LCD

  dht.setup(DHT_PIN, DHTesp::DHT22);  // Initialize DHT22 sensor
  Serial.begin(115200);

  // Start temperature task using FreeRTOS
  xTaskCreatePinnedToCore(tempTask, "Temperature Control", 4096, NULL, 1, NULL, 1);
}

void tempTask(void *parameter) {
  while (1) {
    TempAndHumidity data = dht.getTempAndHumidity();
    float temp = data.temperature;
    float hum = data.humidity;

    // State update logic
    if (currentState == IDLE && temp < TARGET_TEMP - 2) currentState = HEATING;
    else if (currentState == HEATING && temp >= TARGET_TEMP - HYSTERESIS) currentState = STABILIZING;
    else if (currentState == STABILIZING && temp >= TARGET_TEMP) currentState = TARGET_REACHED;
    else if (temp >= OVERHEAT_TEMP) currentState = OVERHEAT;
    else if (currentState == OVERHEAT && temp < TARGET_TEMP - 3) currentState = IDLE;
    else if (currentState == TARGET_REACHED && temp < TARGET_TEMP - 2) currentState = HEATING;

    // Output control
    digitalWrite(heaterPin, (currentState == HEATING) ? HIGH : LOW);
    digitalWrite(fanPin, (currentState == OVERHEAT) ? HIGH : LOW);
    digitalWrite(buzzerPin, (currentState == OVERHEAT) ? HIGH : LOW);

    // LCD display
    lcd.setCursor(0, 0);
    lcd.print("T:"); lcd.print(temp, 2); lcd.print("C ");
    lcd.print("H:"); lcd.print(hum, 2); lcd.print("%");

    lcd.setCursor(0, 1);
    switch (currentState) {
      case IDLE: lcd.print("IDLE         "); break;
      case HEATING: lcd.print("HEATING      "); break;
      case STABILIZING: lcd.print("STABILIZING  "); break;
      case TARGET_REACHED: lcd.print("TARGET OK    "); break;
      case OVERHEAT: lcd.print("OVERHEAT!!!  "); break;
    }

    // Serial Logging
    Serial.print("[Temp: ");
    Serial.print(temp, 2);
    Serial.print("C] [Humidity: ");
    Serial.print(hum, 2);
    Serial.print("%] [Heater: ");
    Serial.print((currentState == HEATING) ? "ON" : "OFF");
    Serial.print("] [State: ");
    switch (currentState) {
      case IDLE: Serial.print("IDLE"); break;
      case HEATING: Serial.print("HEATING"); break;
      case STABILIZING: Serial.print("STABILIZING"); break;
      case TARGET_REACHED: Serial.print("TARGET_REACHED"); break;
      case OVERHEAT: Serial.print("OVERHEAT"); break;
    }
    Serial.println("]");

    vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 second delay
  }
}

void loop() {
//   // Nothing here. Everything runs in FreeRTOS task.
 }
