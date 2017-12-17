#include "PlantCircuit.h"

#define CHECK_HUMIDITY_ACTION 1
#define STOP_IRRIGATION_ACTION 2
#define CHECK_LIGHT_SWITCH 3

PlantCircuit::PlantCircuit() {}
PlantCircuit::PlantCircuit(String name)
{
  plantName = name;
  Serial.println("Created " + getName());
}
PlantCircuit::~PlantCircuit() {}

// >> general
String PlantCircuit::getName()
{
  return plantName;
}

void PlantCircuit::timerCallback(Timer* timer)
{
  // Serial.println("TimerCallback " + String(timer->getAction()) + " for " + getName());
  switch(timer->getAction()) {
    case CHECK_HUMIDITY_ACTION:
      checkHumidity();
      checkIrrigation();
      break;
    case STOP_IRRIGATION_ACTION:
      stopIrrigation();
      break;
    case CHECK_LIGHT_SWITCH:
      checkBrightness();
      break;
    default:
      break;
  }
}
// << general

// >> humidity
void PlantCircuit::humiditySetup(byte vccPin, byte dataPin, long interval = 60000 * 60, int threshold = 500)
{
  humidityVccPin = vccPin;
  humidityDataPin = dataPin;
  humidityThreshold = threshold;
  
  pinMode(vccPin, OUTPUT);
  digitalWrite(vccPin, LOW);

  StensTimer::getInstance()->setInterval(this, CHECK_HUMIDITY_ACTION, interval);
}

int PlantCircuit::checkHumidity()
{
  digitalWrite(humidityVccPin, HIGH);
  delay(100);
  humidity = 1024 - analogRead(humidityDataPin);
  digitalWrite(humidityVccPin, LOW);
  Serial.println("Humidity " + getName() + ": " + String(getHumidity()));
  return getHumidity();
}

int PlantCircuit::getHumidity()
{
  return humidity;
}
// << humidity

// >> irrigation
void PlantCircuit::irrigationSetup(byte vccPin, long duration, bool automatic)
{
  irrigation = automatic;
  irrigationVccPin = vccPin;
  irrigationDuration = duration;
  
  pinMode(irrigationVccPin, OUTPUT);
  digitalWrite(irrigationVccPin, LOW);
}

bool PlantCircuit::checkIrrigation()
{
  if (irrigation && isDry() && !irrigating) startIrrigation();
  return(irrigating);
}

void PlantCircuit::startIrrigation()
{
  if (!irrigating) {
    irrigating = true;
    digitalWrite(irrigationVccPin, HIGH);
    StensTimer::getInstance()->setTimer(this, STOP_IRRIGATION_ACTION, irrigationDuration);
    Serial.println("Start Irrigation: " + getName() + ": " + String(irrigationDuration));
  }
}

void PlantCircuit::stopIrrigation()
{
  irrigating = false;
  digitalWrite(irrigationVccPin, LOW);
  Serial.println("Stop Irrigation: " + getName());
}

bool PlantCircuit::isDry()
{
  return (humidity < humidityThreshold);
}
// << irrigation

// >> lamp
void PlantCircuit::lightSetup(byte vccPin, byte dataPin, long interval, int threshold = 25)
{
  lightVccPin = vccPin;
  photoDataPin = dataPin;
  lightThreshold = threshold;
  
  pinMode(lightVccPin, OUTPUT);
  digitalWrite(lightVccPin, LOW);
  
  StensTimer::getInstance()->setInterval(this, CHECK_LIGHT_SWITCH, interval);
}

void PlantCircuit::checkBrightness()
{
  int brightness = analogRead(photoDataPin);
  Serial.println("Brightness: " + getName() + ": " + String(brightness));
  if (brightness < lightThreshold) switchLightOn();
  else switchLightOff();
}

void PlantCircuit::switchLightOn()
{
  Serial.println("Switch Light On: " + getName());
  digitalWrite(lightVccPin, HIGH);
}

void PlantCircuit::switchLightOff()
{
  Serial.println("Switch Light Off: " + getName());
  digitalWrite(lightVccPin, LOW);
}
// << lamp
