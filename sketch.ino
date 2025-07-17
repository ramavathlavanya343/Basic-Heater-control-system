#include <DHTesp.h>
#include <LiquidCrystal_I2C.h>

const int DHT_PIN = 15;
const int fanPin = 2;
const int heaterPin = 4;
const int buzzerPin = 5; 

const float TARGET_TEMP = 25.0;
const float OVERHEAT_TEMP = 35.0;
const float HYSTERESIS = 0.5;

enum State {
  IDLE,
  HEATING,
  STABILIZING,
  TARGET_REACHED,
  OVERHEAT
};

State currentState = IDLE;

DHTesp dhtSensor;
LiquidCrystal_I2C lcd(0x27, 16, 4);

void updateState(float temp) {
  switch (currentState) {
    case IDLE:
      if (temp >= OVERHEAT_TEMP) currentState = OVERHEAT;
      else if (temp < TARGET_TEMP - 2) currentState = HEATING;
      break;

    case HEATING:
      if (temp >= TARGET_TEMP - HYSTERESIS && temp < OVERHEAT_TEMP)
        currentState = STABILIZING;
      else if (temp >= OVERHEAT_TEMP)
        currentState = OVERHEAT;
      break;

    case STABILIZING:
      if (temp >= TARGET_TEMP && temp < OVERHEAT_TEMP)
        currentState = TARGET_REACHED;
      else if (temp >= OVERHEAT_TEMP)
        currentState = OVERHEAT;
      break;

    case TARGET_REACHED:
      if (temp >= OVERHEAT_TEMP)
        currentState = OVERHEAT;
      else if (temp < TARGET_TEMP - 2)
        currentState = HEATING;
      break;

    case OVERHEAT:
      if (temp < TARGET_TEMP - 3)
        currentState = IDLE;
      break;
  }
}

void controlSystem(State state) {
  switch (state) {
    case IDLE:
      digitalWrite(heaterPin, LOW);
      digitalWrite(fanPin, LOW);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 0); lcd.print("State: IDLE       ");
      break;

    case HEATING:
      digitalWrite(heaterPin, HIGH);
      digitalWrite(fanPin, LOW);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 0); lcd.print("State: HEATING    ");
      break;

    case STABILIZING:
      digitalWrite(heaterPin, LOW);
      digitalWrite(fanPin, LOW);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 0); lcd.print("State: STABILIZING");
      break;

    case TARGET_REACHED:
      digitalWrite(heaterPin, LOW);
      digitalWrite(fanPin, LOW);
      digitalWrite(buzzerPin, LOW);
      lcd.setCursor(0, 0); lcd.print("State: TARGET OK  ");
      break;

    case OVERHEAT:
      digitalWrite(heaterPin, LOW);
      digitalWrite(fanPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      lcd.setCursor(0, 0); lcd.print("State: OVERHEAT!! ");
      break;
  }
}

void displayData(float temp, float hum) {
  lcd.setCursor(0, 1); lcd.print("Temp: "); lcd.print(temp); lcd.print(" C   ");
  lcd.setCursor(0, 2); lcd.print("Hum : "); lcd.print(hum); lcd.print(" %   ");
  lcd.setCursor(0, 3);
  lcd.print("Heater: "); lcd.print(digitalRead(heaterPin) ? "ON " : "OFF");
  lcd.setCursor(12, 3);
  lcd.print("Fan: "); lcd.print(digitalRead(fanPin) ? "ON " : "OFF");

  Serial.print("Temp: "); Serial.print(temp, 1);
  Serial.print(" °C | Hum: "); Serial.print(hum, 1);
  Serial.print(" % | State: "); Serial.println(getStateName(currentState));
}

String getStateName(State state) {
  switch (state) {
    case IDLE: return "IDLE";
    case HEATING: return "HEATING";
    case STABILIZING: return "STABILIZING";
    case TARGET_REACHED: return "TARGET_REACHED";
    case OVERHEAT: return "OVERHEAT";
    default: return "UNKNOWN";
  }
}

void setup() {
  pinMode(fanPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lcd.init();
  lcd.backlight();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float temp = data.temperature;
  float hum = data.humidity;

  updateState(temp);
  controlSystem(currentState);
  displayData(temp, hum);

  delay(1000);
}