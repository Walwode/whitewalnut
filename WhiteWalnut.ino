#include "PlantCircuit.h"
#include "StensTimer.h"

StensTimer* stensTimer;

void evolveEcoSystem()
{
  PlantCircuit* plant1 = new PlantCircuit("Plant Left");
  plant1->humiditySetup(13, A0, 2000, 500);
  plant1->irrigationSetup(12, 10000, true);
  plant1->lightSetup(7, A2, 1000, 60);
  
  PlantCircuit* plant2 = new PlantCircuit("Plant Right");
  plant2->humiditySetup(11, A1, 2000, 500);
  plant2->irrigationSetup(10, 15000, true);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial);
  
  stensTimer = StensTimer::getInstance();
  evolveEcoSystem();
}

void loop()
{
  stensTimer->run();
}
