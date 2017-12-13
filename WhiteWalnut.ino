#include "PlantCircuit.h"
#include "StensTimer.h"
#include "Definer.cpp"

const int plantCount = 2;
PlantCircuit* ecoSystem = new PlantCircuit[plantCount];

void humidityTimerCallback(Timer* timer) {
  PlantCircuit circuit = getCircuitByPin(timer->getAction());
  circuit.checkHumidity();
}

void evolveEcoSystem()
{
  ecoSystem[0] = PlantCircuit("Plant Left", PIN07, 2000, humidityTimerCallback);
  ecoSystem[1] = PlantCircuit("Plant Right", PIN08, 5000, humidityTimerCallback);
}

void setup(){
  Serial.begin(9600);
  while (!Serial);
  evolveEcoSystem();
}

void loop(){
  for (int n = 0; n < plantCount; n++) { ecoSystem[n].update(); }
  delay(100);
}

PlantCircuit getCircuitByPin(int pin) {
  for (int n = 0; n < plantCount; n++) { if (ecoSystem[n].getHumidityVccPin() == pin) { return ecoSystem[n]; } }
}

