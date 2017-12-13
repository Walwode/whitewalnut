#ifndef PLANTCIRCUIT_H
#define PLANTCIRCUIT_H

#include "Arduino.h"
#include "StensTimer.h"

class PlantCircuit
{
  String plantName;
  int humidityVccPin;
  int lastHumidity;
  StensTimer* stensTimer = NULL;
  Timer* humidityTimer = NULL;
  
  public:
    PlantCircuit();
    PlantCircuit(String plantName, int humidityVccPin, int humidityCheckInterval, void (*)());
    ~PlantCircuit();
    String getName();
    void update();
    int getHumidityVccPin();
    int checkHumidity();
    int getHumidity();
};

#endif
