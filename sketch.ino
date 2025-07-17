#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

const int DHT_PIN = 15;
const int fanPin = 2;
const int heaterPin = 4;
const int buzzerPin = 5;

const float TARGET_TEMP = 25.0;
const float OVERHEAT_TEMP = 35.0;
const float HYSTERESIS = 0.5;

enum State { IDLE, HEATING, STABILIZING, TARGET_REACHED, OVERHEAT };
State currentState = IDLE;

DHTesp dht;
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Changed to 2 lines for simplicity

void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  dht.setup(DHT_PIN, DHTesp::DHT22);
  Serial.begin(115200);
}

void loop() {
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
  lcd.print("T:"); lcd.print(temp); lcd.print("C ");
  lcd.print("H:"); lcd.print(hum); lcd.print("% ");

  lcd.setCursor(0, 1);
  if (currentState == IDLE) lcd.print("IDLE         ");
  else if (currentState == HEATING) lcd.print("HEATING      ");
  else if (currentState == STABILIZING) lcd.print("STABILIZING  ");
  else if (currentState == TARGET_REACHED) lcd.print("TARGET OK    ");
  else if (currentState == OVERHEAT) lcd.print("OVERHEAT!!!  ");

  delay(1000);
}
