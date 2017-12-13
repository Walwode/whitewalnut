#include "Arduino.h"
#include "PlantCircuit.h"
#include "StensTimer.h"

PlantCircuit::PlantCircuit() {
}
PlantCircuit::PlantCircuit(String plantName, int humidityVccPin, int humidityCheckInterval, void (*function)()) {
  this->plantName = plantName;
  this->humidityVccPin = humidityVccPin;
  
  stensTimer = StensTimer::getInstance();
  stensTimer->setStaticCallback((*function));
  humidityTimer = stensTimer->setInterval(humidityVccPin, humidityCheckInterval);
  
  Serial.println("Created " + plantName);
}
PlantCircuit::~PlantCircuit() {
}
String PlantCircuit::getName() {
  return plantName;
}
void PlantCircuit::update() {
  stensTimer->run();
}
int PlantCircuit::getHumidityVccPin() {
  return humidityVccPin;
}
int PlantCircuit::checkHumidity()
{
  lastHumidity = 200;
  Serial.println("Check Humidity: " + getName() + "... " + String(getHumidity()));
  return getHumidity();
}
int PlantCircuit::getHumidity()
{
  return lastHumidity;
}

